#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CaptureTheFlagPlayerController.generated.h"

class UInputMappingContext;

UCLASS()
class CAPTURETHEFLAG_API ACaptureTheFlagPlayerController : public APlayerController
{
    GENERATED_BODY()
protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    UInputMappingContext* InputMappingContext;
protected:
    virtual void BeginPlay() override;
};
