#include "CaptureTheFlagGameMode.h"

#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/PlayerStart.h"
#include "CaptureTheFlagPlayerController.h"
#include "CaptureTheFlagCharacter.h"
#include "ColorCheckpoint.h"
#include "Flag.h"

ACaptureTheFlagGameMode::ACaptureTheFlagGameMode()
{
    DefaultPawnClass = ACaptureTheFlagCharacter::StaticClass();
    PlayerControllerClass = ACaptureTheFlagPlayerController::StaticClass();
    PlayerStateClass = ACaptureTheFlagPlayerState::StaticClass();
    GameStateClass = ACaptureTheFlagGameState::StaticClass();

    GameScore.Init(0, TeamCount);
}

bool ACaptureTheFlagGameMode::ShouldSpawnAtStartSpot(AController* Player)
{
    return false;
}

void ACaptureTheFlagGameMode::InitPlayerState(AController* Player)
{
    ACaptureTheFlagGameState* CaptureTheFlagGameState = GetGameState<ACaptureTheFlagGameState>();
    
    int32 AvailableTeamIndex = CaptureTheFlagGameState->GetAvailableTeamIndex();
    ACaptureTheFlagPlayerState* PlayerState = Player->GetPlayerState<ACaptureTheFlagPlayerState>();
    PlayerState->SetTeam(AvailableTeamIndex);
    CaptureTheFlagGameState->AddPlayerToTeam(AvailableTeamIndex);
}

AActor* ACaptureTheFlagGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
    // Get or Init PlayerState to assign a Team to the player
    ACaptureTheFlagPlayerState* PlayerState = Player->GetPlayerState<ACaptureTheFlagPlayerState>();
    if (PlayerState->GetTeam() == -1)
    {
        InitPlayerState(Player);
    }
    FString TeamTag = FString::FromInt(PlayerState->GetTeam());
    
    // Choose a PlayerStart
    APlayerStart* FoundPlayerStart = nullptr;
    UClass* PawnClass = GetDefaultPawnClassForController(Player);
    APawn* PawnToFit = PawnClass ? PawnClass->GetDefaultObject<APawn>() : nullptr;
    TArray<APlayerStart*> UnOccupiedStartPoints;
    TArray<APlayerStart*> OccupiedStartPoints;
    UWorld* World = GetWorld();
    for (TActorIterator<APlayerStart> It(World); It; ++It)
    {
        APlayerStart* PlayerStart = *It;
        
        // Ignore other teams PlayerStarts
        if (!(PlayerStart->PlayerStartTag.ToString() == TeamTag))
        {            
            continue;
        }

        FVector ActorLocation = PlayerStart->GetActorLocation();
        const FRotator ActorRotation = PlayerStart->GetActorRotation();
        if (!World->EncroachingBlockingGeometry(PawnToFit, ActorLocation, ActorRotation))
        {
            UnOccupiedStartPoints.Add(PlayerStart);
        }
        else if (World->FindTeleportSpot(PawnToFit, ActorLocation, ActorRotation))
        {
            OccupiedStartPoints.Add(PlayerStart);
        }
    }

    if (UnOccupiedStartPoints.Num() > 0)
    {
        FoundPlayerStart = UnOccupiedStartPoints[FMath::RandRange(0, UnOccupiedStartPoints.Num() - 1)];
    }
    else if (OccupiedStartPoints.Num() > 0)
    {
        FoundPlayerStart = OccupiedStartPoints[FMath::RandRange(0, OccupiedStartPoints.Num() - 1)];
    }
    return FoundPlayerStart;
}

void ACaptureTheFlagGameMode::OnScore(int32 Team, ACaptureTheFlagCharacter* Character)
{
    AddScore(Team);

    if (GEngine)
        GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::White, FString::Printf(TEXT("Score! Team %d has %d points now."), Team, GetScore(Team)));
    
    AFlag* Flag = Character->GetFlag();
    Flag->ResetFlagMulticast(LevelFlagLocation);
    Character->SetFlag(nullptr);

    int32 IndexOfMaxValue, MaxValue;
    UKismetMathLibrary::MaxOfIntArray(GameScore, IndexOfMaxValue, MaxValue);
    WinningHighScore = MaxValue;
    WinningTeamIndex = IndexOfMaxValue;
}

void ACaptureTheFlagGameMode::OnDeath(AController* DeadPlayerController, AController* KillerPlayerController)
{
    if (!DeadPlayerController || !KillerPlayerController) return;

    APawn* PlayerPawn = DeadPlayerController->GetPawn();
    if (!PlayerPawn) return;

    ACaptureTheFlagCharacter* Character = Cast<ACaptureTheFlagCharacter>(DeadPlayerController->GetCharacter());
    if (Character && Character->HasFlag())
    {
        AFlag* Flag = Character->GetFlag();
        Flag->DetachFlagMulticast();
        Character->SetFlag(nullptr);
    }

    PlayerPawn->DetachFromControllerPendingDestroy();
    FTimerDelegate TimerDelegate;
    TimerDelegate.BindUFunction(this, FName("DestroyAndRespawn"), PlayerPawn, DeadPlayerController);
    FTimerHandle TimerHandle;
    GetWorldTimerManager().SetTimer(TimerHandle, TimerDelegate, PlayerRespawnTime, false);
}

void ACaptureTheFlagGameMode::DestroyAndRespawn(APawn* PlayerPawn, AController* DeadPlayerController)
{
    PlayerPawn->Destroy();
    RestartPlayer(DeadPlayerController);
}

void ACaptureTheFlagGameMode::OnFall(AController* PlayerController)
{
    if (!PlayerController) return;

    APawn* PlayerPawn = PlayerController->GetPawn();
    if (PlayerPawn) PlayerPawn->Destroy();
    RestartPlayer(PlayerController);
}

bool ACaptureTheFlagGameMode::ReadyToEndMatch_Implementation()
{
    return (WinningHighScore >= MatchScoreLimit);
}

void ACaptureTheFlagGameMode::HandleMatchHasEnded()
{
    Super::HandleMatchHasEnded();
    
    if (GEngine)
        GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, FString::Printf(TEXT("Game Ended! Team %d wins!"), WinningTeamIndex));
    
    TArray<AActor*> CharacterActors;
    UGameplayStatics::GetAllActorsOfClass(this, ACaptureTheFlagCharacter::StaticClass(), CharacterActors);
    for (AActor* CharacterActor : CharacterActors)
    {
        ACaptureTheFlagCharacter* Character = Cast<ACaptureTheFlagCharacter>(CharacterActor);
        if (Character->HasWeapon())
        {
            Character->GetWeapon()->Destroy();
        }
        Character->Destroy();
    }
    FTimerHandle TimerHandle;
    GetWorldTimerManager().SetTimer(TimerHandle, this, &ACaptureTheFlagGameMode::RestartMap, 3.0f);
}

void ACaptureTheFlagGameMode::RestartMap() const
{
    GetWorld()->ServerTravel(GetWorld()->GetName(), false, false);
}