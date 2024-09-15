#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Flag.generated.h"

UCLASS()
class CAPTURETHEFLAG_API AFlag : public AActor
{
    GENERATED_BODY()
private:
    UPROPERTY(VisibleAnywhere)
    class UBoxComponent* BoxComponent;

    UPROPERTY(VisibleAnywhere)
    class URotatingMovementComponent* RotatingMovementComponent;
public:
    AFlag();

    UFUNCTION()
    void OnBeginOverlap(UPrimitiveComponent* Component, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit);

    UFUNCTION(NetMulticast, Reliable)
    void ResetFlagMulticast(FVector Location);

    UFUNCTION(NetMulticast, Reliable)
    void DetachFlagMulticast();
};