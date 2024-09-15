#include "CaptureTheFlagCharacter.h"
#include "Projectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Engine/LocalPlayer.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

ACaptureTheFlagCharacter::ACaptureTheFlagCharacter()
{
    GetCapsuleComponent()->InitCapsuleSize(55.0f, 96.0f);
        
    FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
    FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
    FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.0f, 0.0f, 60.0f));
    FirstPersonCameraComponent->bUsePawnControlRotation = true;

    // Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
    FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterFirstPersonMesh"));
    FirstPersonMesh->SetOnlyOwnerSee(true);
    FirstPersonMesh->SetupAttachment(FirstPersonCameraComponent);
    FirstPersonMesh->bCastDynamicShadow = false;
    FirstPersonMesh->CastShadow = false;
    FirstPersonMesh->SetRelativeLocation(FVector(-30.0f, 0.0f, -150.0f));
}

void ACaptureTheFlagCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    
    DOREPLIFETIME_CONDITION(ACaptureTheFlagCharacter, Health, COND_OwnerOnly);

    DOREPLIFETIME(ACaptureTheFlagCharacter, Weapon);
    DOREPLIFETIME(ACaptureTheFlagCharacter, Flag);
}

void ACaptureTheFlagCharacter::BeginPlay()
{
    Super::BeginPlay();

    if (HasAuthority())
    {
        Health = MaxHealth;
    }

    if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
        {
            Subsystem->AddMappingContext(DefaultMappingContext, 0);
        }
    }

    GameMode = Cast<ACaptureTheFlagGameMode>(GetWorld()->GetAuthGameMode());
}

void ACaptureTheFlagCharacter::FellOutOfWorld(const UDamageType& DmgType)
{
    GameMode->OnFall(GetController());
}

void ACaptureTheFlagCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
        EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ACaptureTheFlagCharacter::Move);
        EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ACaptureTheFlagCharacter::Look);
    }
}


void ACaptureTheFlagCharacter::Move(const FInputActionValue& Value)
{
    FVector2D MovementVector = Value.Get<FVector2D>();

    if (Controller)
    {
        AddMovementInput(GetActorForwardVector(), MovementVector.Y);
        AddMovementInput(GetActorRightVector(), MovementVector.X);
    }
}

void ACaptureTheFlagCharacter::Look(const FInputActionValue& Value)
{
    FVector2D LookAxisVector = Value.Get<FVector2D>();

    if (Controller)
    {
        AddControllerYawInput(LookAxisVector.X);
        AddControllerPitchInput(LookAxisVector.Y);
    }
}

void ACaptureTheFlagCharacter::PlaySoundAtLocationMulticast_Implementation(USoundBase* Sound)
{
    if (Sound)
    {
        UGameplayStatics::PlaySoundAtLocation(GetWorld(), Sound, GetActorLocation());
    }
}

void ACaptureTheFlagCharacter::PlayAnimMontageMulticast_Implementation(UAnimMontage* AnimMontage)
{
    UAnimInstance* AnimInstance = GetFirstPersonMesh()->GetAnimInstance();
    if (AnimInstance && AnimMontage)
    {
        AnimInstance->Montage_Play(AnimMontage, 1.0f);
    }
}

void ACaptureTheFlagCharacter::EquipWeaponServer_Implementation(TSubclassOf<AWeapon> WeaponClass)
{
    if (HasWeapon()) return;
    if (!WeaponClass) return;
    
    UWorld* World = GetWorld();
    if (!World) return;

    // Spawn and attach weapon
    Weapon = World->SpawnActorDeferred<AWeapon>(WeaponClass, FTransform::Identity, this, nullptr, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
    UGameplayStatics::FinishSpawningActor(Weapon, FTransform::Identity);
    
    FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
    Weapon->AttachToComponent(GetFirstPersonMesh(), AttachmentRules, FName(TEXT("GripPoint")));
    
    // Add Weapon action bindings to the player character
    Weapon->SetupActionBindingsClient();
}

void ACaptureTheFlagCharacter::ApplyDamage(ACaptureTheFlagCharacter* KillerCharacter, FName BoneName, float Damage, FVector ImpulseDirection)
{
    if (Health <= 0.0f) return;
    RemoveHealth(Damage);

    if (Health <= 0.0f)
    {
        Die(KillerCharacter, BoneName, ImpulseDirection);
    }
}

void ACaptureTheFlagCharacter::Die(ACaptureTheFlagCharacter* KillerCharacter, FName BoneName, FVector ImpulseDirection)
{
    RagdollMulticast(ImpulseDirection * 30.0f, BoneName);
    if (HasWeapon())
    {
        Weapon->Destroy();
    }
    GameMode->OnDeath(GetController(), KillerCharacter->GetController());
}

void ACaptureTheFlagCharacter::RagdollMulticast_Implementation(FVector Impulse, FName BoneName)
{
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    USkeletalMeshComponent* ThirdPersonMesh = GetMesh();
    ThirdPersonMesh->SetCollisionProfileName("Ragdoll");
    ThirdPersonMesh->SetEnableGravity(true);
    ThirdPersonMesh->SetSimulatePhysics(true);
    ThirdPersonMesh->AddImpulse(Impulse, BoneName, true);
}