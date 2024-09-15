#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "CaptureTheFlagGameState.h"
#include "CaptureTheFlagPlayerState.h"

#include "CaptureTheFlagGameMode.generated.h"

UCLASS()
class CAPTURETHEFLAG_API ACaptureTheFlagGameMode : public AGameMode
{
    GENERATED_BODY()
private:
    UPROPERTY(EditDefaultsOnly)
    float PlayerRespawnTime = 5.0f;
    
    UPROPERTY(EditDefaultsOnly)
    int32 MatchScoreLimit = 3;

    UPROPERTY(EditDefaultsOnly)
    FVector LevelFlagLocation = FVector::ZeroVector;

    // TODO: Remove magic number, add the possibility of infering the number 
    // of teams in a level by counting how many AColorCheckpoints exist in it.
    int32 TeamCount = 2;
    TArray<int32> GameScore;
    int32 WinningHighScore = 0;
    int32 WinningTeamIndex = 0;
private:
    void InitPlayerState(AController* Player);

    UFUNCTION()
    void DestroyAndRespawn(APawn* PlayerPawn, AController* DeadPlayerController);

    UFUNCTION()
    void RestartMap() const;
public:
    ACaptureTheFlagGameMode();
    
    virtual bool ShouldSpawnAtStartSpot(AController* Player) override;
    virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
    virtual bool ReadyToEndMatch_Implementation() override;
    virtual void HandleMatchHasEnded() override;
    
    void OnDeath(AController* DeadPlayerController, AController* KillerPlayerController);
    void OnFall(AController* PlayerController);
    void OnScore(int32 Team, class ACaptureTheFlagCharacter* Character);
        
    UFUNCTION(BlueprintCallable)
    FORCEINLINE void AddScore(int32 Team) { GameScore[Team]++; }

    UFUNCTION(BlueprintPure)
    FORCEINLINE int32 GetScore(int32 Team) { return GameScore[Team]; }
};