// Copyright Epic Games, Inc. All Rights Reserved.

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

//////////////////////////////////////////////////////////////////////////
// ACaptureTheFlagCharacter

ACaptureTheFlagCharacter::ACaptureTheFlagCharacter()
{
    // Character doesnt have a rifle at start
    bHasRifle = false;
    
    // Set size for collision capsule
    GetCapsuleComponent()->InitCapsuleSize(55.0f, 96.0f);
        
    // Create a CameraComponent    
    FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
    FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
    FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.0f, 0.0f, 60.0f)); // Position the camera
    FirstPersonCameraComponent->bUsePawnControlRotation = true;

    // Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
    FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterFirstPersonMesh"));
    FirstPersonMesh->SetOnlyOwnerSee(true);
    FirstPersonMesh->SetupAttachment(FirstPersonCameraComponent);
    FirstPersonMesh->bCastDynamicShadow = false;
    FirstPersonMesh->CastShadow = false;
    //FirstPersonMesh->SetRelativeRotation(FRotator(0.9f, -19.19f, 5.2f));
    FirstPersonMesh->SetRelativeLocation(FVector(-30.0f, 0.0f, -150.0f));
}

void ACaptureTheFlagCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    
    DOREPLIFETIME_CONDITION(ACaptureTheFlagCharacter, Health, COND_OwnerOnly);
    DOREPLIFETIME(ACaptureTheFlagCharacter, bHasRifle);
    DOREPLIFETIME(ACaptureTheFlagCharacter, Weapon);
}

void ACaptureTheFlagCharacter::BeginPlay()
{
    // Call the base class  
    Super::BeginPlay();

    if (HasAuthority())
    {
        Health = MaxHealth;
    }

    // Add Input Mapping Context
    if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
        {
            Subsystem->AddMappingContext(DefaultMappingContext, 0);
        }
    }

}

//////////////////////////////////////////////////////////////////////////// Input

void ACaptureTheFlagCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    // Set up action bindings
    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        // Jumping
        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

        // Moving
        EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ACaptureTheFlagCharacter::Move);

        // Looking
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

void ACaptureTheFlagCharacter::SetHasRifle(bool bNewHasRifle)
{
    bHasRifle = bNewHasRifle;
}

bool ACaptureTheFlagCharacter::GetHasRifle()
{
    return bHasRifle;
}

void ACaptureTheFlagCharacter::EquipWeaponServer_Implementation(TSubclassOf<AWeapon> WeaponClass)
{
    if (GetHasRifle()) return;
    if (!WeaponClass) return;
    
    UWorld* World = GetWorld();
    if (!World) return;

    // switch bHasRifle so the animation blueprint can switch to another animation set
    SetHasRifle(true);

    // Spawn and attach weapon
    Weapon = World->SpawnActorDeferred<AWeapon>(WeaponClass, FTransform::Identity, this, nullptr, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
    UGameplayStatics::FinishSpawningActor(Weapon, FTransform::Identity);
    
    FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
    Weapon->AttachToComponent(GetFirstPersonMesh(), AttachmentRules, FName(TEXT("GripPoint")));
    Weapon->SetCharacter(this);
    
    // Add Weapon action bindings to the player character
    Weapon->SetupActionBindingsClient();
}

void ACaptureTheFlagCharacter::ApplyDamage(ACaptureTheFlagCharacter* KillerCharacter, float Damage)
{
    if (Health <= 0.0f) return;
    RemoveHealth(Damage);

    if (Health > 0.0f) {
        UE_LOG(LogTemp, Display, TEXT("%s: Health: %f"), *GetName(), GetHealth());
        // PlaySoundAtLocationMulticast(DamageSound);
    } else {
        Die(KillerCharacter);
    }
}

void ACaptureTheFlagCharacter::Die(ACaptureTheFlagCharacter* KillerCharacter)
{
    UE_LOG(LogTemp, Display, TEXT("Died"));

    if (Weapon) {
        Weapon->Destroy();
    }
    // GameMode->OnDeath(GetController(), KillerCharacter->GetController());
}