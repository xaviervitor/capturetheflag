#include "Pickup.h"

#include "Components/SphereComponent.h"
#include "GameFramework/RotatingMovementComponent.h"

APickup::APickup()
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = true;
    
    SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
    SphereComponent->SetCollisionProfileName("OverlapAll");
    SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &APickup::OnBeginOverlap);
    SphereComponent->InitSphereRadius(32.0f);
    RootComponent = SphereComponent;

    RotatingMovementComponent = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("RotatingMovementComponent"));
    RotatingMovementComponent->RotationRate = FRotator(0.0f, 90.0f, 0.0f);
}

void APickup::OnBeginOverlap(UPrimitiveComponent* Component, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit)
{
    ACaptureTheFlagCharacter* Character = Cast<ACaptureTheFlagCharacter>(OtherActor);
    if (Character)
    {
        AController* PlayerController = Character->GetController();
        if (PlayerController && PlayerController->IsLocalController())
        {
            DeactivatePickup();
            Character->PlaySoundAtLocationMulticast(PickupSound);
            FTimerHandle TimerHandle;
            GetWorldTimerManager().SetTimer(TimerHandle, this, &APickup::ReactivatePickup, ReactivationTime, false);
            OnPickup(Character);
        }
    }
}

void APickup::DeactivatePickup()
{
    SetActorEnableCollision(false);
    SetActorHiddenInGame(true);
}

void APickup::ReactivatePickup()
{
    SetActorEnableCollision(true);
    SetActorHiddenInGame(false);
}