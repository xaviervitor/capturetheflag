#include "ColorCheckpoint.h"

#include "Components/BoxComponent.h"
#include "GameFramework/GameMode.h"
#include "CaptureTheFlagCharacter.h"
#include "Flag.h"

AColorCheckpoint::AColorCheckpoint()
{
    BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
    BoxComponent->SetCollisionProfileName("OverlapAll");
    BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AColorCheckpoint::OnBeginOverlap);
    RootComponent = BoxComponent;
}

void AColorCheckpoint::OnBeginOverlap(UPrimitiveComponent* Component, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit)
{
    if (!HasAuthority()) return;

    ACaptureTheFlagCharacter* Character = Cast<ACaptureTheFlagCharacter>(OtherActor);
    if (!Character) return;
    AController* CharacterController = Character->GetController();
    if (!CharacterController) return;
    int32 PlayerTeam = CharacterController->GetPlayerState<ACaptureTheFlagPlayerState>()->GetTeam();
    if (PlayerTeam == -1) return;
    
    if (Character->HasFlag() && PlayerTeam == GetTeam())
    {
        Character->GetGameMode()->OnScore(GetTeam(), Character);
    }
}
