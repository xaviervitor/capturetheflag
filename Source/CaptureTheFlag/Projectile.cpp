// Copyright Epic Games, Inc. All Rights Reserved.

#include "Projectile.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"

#include "CaptureTheFlagCharacter.h"

AProjectile::AProjectile() 
{
    bReplicates = true;
    SetReplicateMovement(true);
    
    // Use a sphere as a simple collision representation
    SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
    SphereComponent->InitSphereRadius(5.0f);
    SphereComponent->BodyInstance.SetCollisionProfileName("Projectile");
    SphereComponent->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);        // set up a notification for when this component hits something blocking

    // Players can't walk on it
    SphereComponent->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.0f));
    SphereComponent->CanCharacterStepUpOn = ECB_No;

    // Firing player is not blocked
    SphereComponent->IgnoreActorWhenMoving(GetOwner(), true);

    // Set as root component
    RootComponent = SphereComponent;

    // Use a ProjectileMovementComponent to govern this projectile's movement
    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
    ProjectileMovement->UpdatedComponent = SphereComponent;
    ProjectileMovement->InitialSpeed = 3000.0f;
    ProjectileMovement->MaxSpeed = 3000.0f;
    ProjectileMovement->bRotationFollowsVelocity = true;
    ProjectileMovement->bShouldBounce = true;

    // Die after 3 seconds by default
    InitialLifeSpan = 3.0f;
}

void AProjectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    
    DOREPLIFETIME(AProjectile, Damage);
}

void AProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if (!HasAuthority()) return;

    ACaptureTheFlagCharacter* OtherCharacter = Cast<ACaptureTheFlagCharacter>(OtherActor);
    ACaptureTheFlagCharacter* Character = Cast<ACaptureTheFlagCharacter>(GetOwner());

    if (OtherCharacter && OtherActor != this)
    {
        Character->RemoveHealth(Damage);
        OtherCharacter->ApplyDamage(Character, Damage);
        Destroy();
    }
}