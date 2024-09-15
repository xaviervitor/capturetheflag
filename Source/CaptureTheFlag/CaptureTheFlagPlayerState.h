#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "CaptureTheFlagPlayerState.generated.h"

UCLASS()
class CAPTURETHEFLAG_API ACaptureTheFlagPlayerState : public APlayerState
{
    GENERATED_BODY()
private:
    int32 Team = -1;
public:
    UFUNCTION(BlueprintPure)
    FORCEINLINE int32 GetTeam() { return Team; }

    UFUNCTION(BlueprintCallable)
    FORCEINLINE void SetTeam(int32 InTeam) { Team = InTeam; }
};
