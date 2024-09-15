#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "CaptureTheFlagGameState.generated.h"

UCLASS()
class CAPTURETHEFLAG_API ACaptureTheFlagGameState : public AGameState
{
    GENERATED_BODY()
private:
    TArray<int32> TeamPlayers;
public:
    ACaptureTheFlagGameState();

    UFUNCTION(BlueprintPure)
    FORCEINLINE TArray<int32> GetTeamPlayers() { return TeamPlayers; }

    UFUNCTION(BlueprintPure)
    FORCEINLINE int32 GetPoints(int32 Team) { return TeamPlayers[Team]; }
    
    UFUNCTION(BlueprintCallable)
    FORCEINLINE void AddPlayerToTeam(int32 Team) { TeamPlayers[Team] = TeamPlayers[Team] + 1; }
    
    UFUNCTION(BlueprintCallable)
    int32 GetAvailableTeamIndex();
};
