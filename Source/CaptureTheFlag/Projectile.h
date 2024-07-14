// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;

UCLASS(config=Game)
class AProjectile : public AActor
{
    GENERATED_BODY()

    /** Sphere collision component */
    UPROPERTY(VisibleDefaultsOnly, Category=Projectile)
    USphereComponent* SphereComponent;

    /** Projectile movement component */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
    UProjectileMovementComponent* ProjectileMovement;

    UPROPERTY(EditAnywhere, Replicated)
    float Damage;

public:
    AProjectile();

    /** called when projectile hits something */
    UFUNCTION()
    void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

    /** Returns SphereComponent subobject **/
    USphereComponent* GetSphereComponent() const { return SphereComponent; }
    /** Returns ProjectileMovement subobject **/
    UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }
};

