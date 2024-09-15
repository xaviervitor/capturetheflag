#include "Weapon.h"

#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/ArrowComponent.h"
#include "Net/UnrealNetwork.h"

#include "CaptureTheFlagCharacter.h"
#include "Projectile.h"

AWeapon::AWeapon()
{
    bReplicates = true;

    SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
    RootComponent = SceneComponent;
    
    MuzzleSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleSceneComponent"));
    MuzzleSceneComponent->SetupAttachment(RootComponent);

    ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComponent"));
    ArrowComponent->SetupAttachment(MuzzleSceneComponent);
}

void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    
    DOREPLIFETIME_CONDITION(AWeapon, Character, COND_OwnerOnly);
}

void AWeapon::BeginPlay()
{
    Super::BeginPlay();    
}

void AWeapon::SetOwner(AActor* InOwner)
{
    Super::SetOwner(InOwner);
    Character = Cast<ACaptureTheFlagCharacter>(InOwner);
}

void AWeapon::FireServer_Implementation()
{
    if (!Character || !Character->GetController()) return;
    if (ProjectileClass)
    {
        UWorld* const World = GetWorld();
        if (World)
        {
            APlayerController* PlayerController = Cast<APlayerController>(Character->GetController());
            FTransform ProjectileTransform = FTransform::Identity;
            ProjectileTransform.SetLocation(MuzzleSceneComponent->GetComponentLocation());
            ProjectileTransform.SetRotation(PlayerController->PlayerCameraManager->GetCameraRotation().Quaternion());
            
            AProjectile* Projectile = World->SpawnActorDeferred<AProjectile>(ProjectileClass, ProjectileTransform, this, Character, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
            UGameplayStatics::FinishSpawningActor(Projectile, ProjectileTransform);
        }
    }
    
    Character->PlaySoundAtLocationMulticast(FireSound);
    Character->PlayAnimMontageMulticast(FireAnimation);
}

void AWeapon::SetupActionBindingsClient_Implementation()
{
    ACaptureTheFlagCharacter* ControllerCharacter = Cast<ACaptureTheFlagCharacter>(UGameplayStatics::GetPlayerController(this, 0)->GetPawn());
    if (Character != ControllerCharacter) return;

    APlayerController* PlayerController = Cast<APlayerController>(Character->GetController());
    if (PlayerController)
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
        {
            // Set the priority of the mapping to 1, so that it overrides the Jump action with the Fire action when using touch input
            Subsystem->AddMappingContext(FireMappingContext, 1);
        }

        if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
        {
            // Fire
            EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &AWeapon::FireServer);
        }
    }
}

void AWeapon::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (!Character) return;

    if (APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
        {
            Subsystem->RemoveMappingContext(FireMappingContext);
        }
    }
}