#include "Flag.h"

#include "Components/BoxComponent.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "CaptureTheFlagCharacter.h"

AFlag::AFlag()
{
    bReplicates = true;
    PrimaryActorTick.bCanEverTick = false;
    
    BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
    BoxComponent->SetCollisionProfileName("OverlapAll");
    BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AFlag::OnBeginOverlap);
    RootComponent = BoxComponent;
    
    RotatingMovementComponent = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("RotatingMovementComponent"));
    RotatingMovementComponent->RotationRate = FRotator(0.0f, 90.0f, 0.0f);
}

void AFlag::OnBeginOverlap(UPrimitiveComponent* Component, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit)
{
    if (!HasAuthority()) return;
    ACaptureTheFlagCharacter* Character = Cast<ACaptureTheFlagCharacter>(OtherActor);
    if (Character)
    {
        // Attach the flag to the Character
        FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
        AttachToComponent(Character->GetMesh(), AttachmentRules, FName(TEXT("FlagPosition")));
        Character->SetFlag(this);

        if (GEngine)
            GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, FString::Printf(TEXT("Player %s has the flag!"), *Character->GetName()));
    }
}

void AFlag::ResetFlagMulticast_Implementation(FVector Location)
{
    TeleportTo(Location, FRotator::ZeroRotator, false, true);
    DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
}

void AFlag::DetachFlagMulticast_Implementation()
{
    AActor* AttachedActor = GetAttachParentActor();
    if (!AttachedActor) return;
    TeleportTo(AttachedActor->GetActorLocation(), FRotator::ZeroRotator, false, true);
    DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
}