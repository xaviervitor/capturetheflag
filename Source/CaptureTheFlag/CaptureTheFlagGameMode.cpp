#include "CaptureTheFlagGameMode.h"

#include "CaptureTheFlagCharacter.h"
#include "CaptureTheFlagPlayerController.h"

ACaptureTheFlagGameMode::ACaptureTheFlagGameMode()
{
    DefaultPawnClass = ACaptureTheFlagCharacter::StaticClass();
    PlayerControllerClass = ACaptureTheFlagPlayerController::StaticClass();
}

void ACaptureTheFlagGameMode::OnDeath(AController* DeadPlayerController, AController* KillerPlayerController)
{
    // handle death
}

void ACaptureTheFlagGameMode::OnFall(AController* PlayerController)
{
    if (!PlayerController) return;

    APawn* PlayerPawn = PlayerController->GetPawn();
    if (PlayerPawn) PlayerPawn->Destroy();
    RestartPlayer(PlayerController);
}