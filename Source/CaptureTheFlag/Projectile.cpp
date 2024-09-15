// Copyright Epic Games, Inc. All Rights Reserved.

#include "Projectile.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"

#include "CaptureTheFlagCharacter.h"

AProjectile::AProjectile() 
{
    bReplicates = true;
    SetReplicateMovement(true);
    
    SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
    SphereComponent->InitSphereRadius(5.0f);
    SphereComponent->BodyInstance.SetCollisionProfileName("Projectile");
    SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AProjectile::OnBeginOverlap);
    SphereComponent->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.0f));
    SphereComponent->CanCharacterStepUpOn = ECB_No;
    RootComponent = SphereComponent;

    // Firing player is not blocked
    SphereComponent->IgnoreActorWhenMoving(GetInstigator(), true);

    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    ProjectileMovement->UpdatedComponent = SphereComponent;
    ProjectileMovement->InitialSpeed = 6000.0f;
    ProjectileMovement->MaxSpeed = 6000.0f;
    ProjectileMovement->bRotationFollowsVelocity = true;
    ProjectileMovement->bShouldBounce = true;
    ProjectileMovement->ProjectileGravityScale = 0.0f;
    
    InitialLifeSpan = 60.0f;
}

void AProjectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    
    DOREPLIFETIME(AProjectile, Damage);
}

void AProjectile::OnBeginOverlap(UPrimitiveComponent* Component, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit)
{
    if (!HasAuthority()) return;

    ACaptureTheFlagCharacter* ShotCharacter = Cast<ACaptureTheFlagCharacter>(OtherActor);
    ACaptureTheFlagCharacter* ShooterCharacter = Cast<ACaptureTheFlagCharacter>(GetInstigator());

    if (ShotCharacter)
    { 
        if (ShotCharacter == ShooterCharacter) return;
        FVector ImpulseDirection = UKismetMathLibrary::GetDirectionUnitVector(GetActorLocation(), ShotCharacter->GetActorLocation());
        ShotCharacter->ApplyDamage(ShooterCharacter, Hit.BoneName, Damage, ImpulseDirection);
    }
    Destroy();
}