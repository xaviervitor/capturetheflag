// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "Weapon.h"

#include "CaptureTheFlagCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;
class USoundBase;
class UAnimMontage;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class ACaptureTheFlagCharacter : public ACharacter
{
    GENERATED_BODY()

    UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
    USkeletalMeshComponent* FirstPersonMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    UCameraComponent* FirstPersonCameraComponent;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
    UInputMappingContext* DefaultMappingContext;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
    UInputAction* JumpAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
    UInputAction* MoveAction;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    class UInputAction* LookAction;
    
    UPROPERTY(EditAnywhere)
    float MaxHealth = 100.0f;

    UPROPERTY(EditAnywhere, Replicated)
    float Health;

    UPROPERTY(Replicated)
    AWeapon* Weapon = nullptr;

public:
    ACaptureTheFlagCharacter();

    UFUNCTION(NetMulticast, Unreliable, BlueprintCallable)
    void PlaySoundAtLocationMulticast(USoundBase* Sound);

    UFUNCTION(NetMulticast, Unreliable, BlueprintCallable)
    void PlayAnimMontageMulticast(UAnimMontage* AnimMontage);

    UFUNCTION(Server, Reliable, BlueprintCallable)
    void EquipWeaponServer(TSubclassOf<AWeapon> WeaponClass);
protected:
    virtual void BeginPlay();

    // APawn interface
    virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
    // End of APawn interface

    void Move(const FInputActionValue& Value);

    void Look(const FInputActionValue& Value);

public:
    
    USkeletalMeshComponent* GetFirstPersonMesh() const { return FirstPersonMesh; }
    
    UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
    bool bHasRifle;

    UFUNCTION(BlueprintCallable)
    void SetHasRifle(bool bNewHasRifle);

    UFUNCTION(BlueprintCallable)
    bool GetHasRifle();

    void ApplyDamage(ACaptureTheFlagCharacter* KillerCharacter, float Damage);

    void Die(ACaptureTheFlagCharacter* KillerCharacter);

    UFUNCTION(BlueprintPure)
    FORCEINLINE float GetHealth() { return Health; }
    
    UFUNCTION(BlueprintCallable)
    FORCEINLINE void SetHealth(float InHealth) { Health = InHealth; }
    
    UFUNCTION(BlueprintCallable)
    FORCEINLINE void AddHealth(float Amount) { Health += Amount; }
    
    UFUNCTION(BlueprintCallable)
    FORCEINLINE void RemoveHealth(float Amount) { Health -= Amount; }
};

