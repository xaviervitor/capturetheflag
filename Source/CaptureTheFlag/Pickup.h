// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CaptureTheFlagCharacter.h"

#include "Pickup.generated.h"

UCLASS()
class CAPTURETHEFLAG_API APickup : public AActor
{
    GENERATED_BODY()
private:
    UPROPERTY(VisibleAnywhere)
    class USphereComponent* SphereComponent;

    UPROPERTY(VisibleAnywhere)
    class URotatingMovementComponent* RotatingMovementComponent;

    UPROPERTY(VisibleAnywhere)
    class USoundBase* PickupSound;

    UPROPERTY(EditDefaultsOnly)
    float ReactivationTime = 1.0f;
public:
    APickup();

    UFUNCTION() 
    void OnBeginOverlap(UPrimitiveComponent* Component, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit);

    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
    void OnPickup(ACaptureTheFlagCharacter* CollidingCharacter);

    void DeactivatePickup();
    void ReactivatePickup();
};

