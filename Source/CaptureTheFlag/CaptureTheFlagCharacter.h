// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "Weapon.h"
#include "CaptureTheFlagGameMode.h"

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
private:
    UPROPERTY(VisibleAnywhere)
    USkeletalMeshComponent* FirstPersonMesh;

    UPROPERTY(VisibleAnywhere)
    UCameraComponent* FirstPersonCameraComponent;

    UPROPERTY(EditAnywhere)
    UInputMappingContext* DefaultMappingContext;

    UPROPERTY(EditAnywhere)
    UInputAction* JumpAction;

    UPROPERTY(EditAnywhere)
    UInputAction* MoveAction;
    
    UPROPERTY(EditAnywhere)
    class UInputAction* LookAction;
    
    UPROPERTY(VisibleAnywhere)
    float MaxHealth = 100.0f;

    UPROPERTY(VisibleAnywhere, Replicated)
    float Health;

    UPROPERTY(VisibleAnywhere)
    ACaptureTheFlagGameMode* GameMode;

    UPROPERTY(Replicated)
    AWeapon* Weapon = nullptr;

    UPROPERTY(Replicated)
    AFlag* Flag = nullptr;
protected:
    virtual void BeginPlay();
    virtual void FellOutOfWorld(const UDamageType& DmgType) override;
    
    // APawn interface
    virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
    // End of APawn interface

    void Move(const FInputActionValue& Value);

    void Look(const FInputActionValue& Value);

    void Die(ACaptureTheFlagCharacter* KillerCharacter, FName BoneName, FVector ImpulseDirection);
public:
    ACaptureTheFlagCharacter();

    UFUNCTION(NetMulticast, Unreliable, BlueprintCallable)
    void PlaySoundAtLocationMulticast(USoundBase* Sound);

    UFUNCTION(NetMulticast, Unreliable, BlueprintCallable)
    void PlayAnimMontageMulticast(UAnimMontage* AnimMontage);

    UFUNCTION(Server, Reliable, BlueprintCallable)
    void EquipWeaponServer(TSubclassOf<AWeapon> WeaponClass);

    UFUNCTION(NetMulticast, Reliable)
    void RagdollMulticast(FVector Impulse = FVector::ZeroVector, FName BoneName = NAME_None);
    
    void ApplyDamage(ACaptureTheFlagCharacter* KillerCharacter, FName BoneName, float Damage, FVector ImpulseDirection);

    FORCEINLINE USkeletalMeshComponent* GetFirstPersonMesh() const { return FirstPersonMesh; }
    
    FORCEINLINE UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }
    
    FORCEINLINE ACaptureTheFlagGameMode* GetGameMode() const { return GameMode; }

    UFUNCTION(BlueprintPure)
    FORCEINLINE bool HasWeapon() const { return (Weapon != nullptr); }

    UFUNCTION(BlueprintPure)
    FORCEINLINE AWeapon* GetWeapon() const { return Weapon; }

    UFUNCTION(BlueprintPure)
    FORCEINLINE bool HasFlag() const { return (Flag != nullptr); }
    
    UFUNCTION(BlueprintPure)
    FORCEINLINE AFlag* GetFlag() const { return Flag; }

    UFUNCTION(BlueprintCallable)
    FORCEINLINE void SetFlag(AFlag* InFlag) { Flag = InFlag; }

    UFUNCTION(BlueprintPure)
    FORCEINLINE float GetHealth() const { return Health; }
    
    UFUNCTION(BlueprintCallable)
    FORCEINLINE void SetHealth(float InHealth) { Health = InHealth; }
    
    UFUNCTION(BlueprintCallable)
    FORCEINLINE void AddHealth(float Amount) { Health += Amount; }
    
    UFUNCTION(BlueprintCallable)
    FORCEINLINE void RemoveHealth(float Amount) { Health -= Amount; }
};

