#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Actor.h"

#include "Weapon.generated.h"

class ACaptureTheFlagCharacter;

UCLASS()
class CAPTURETHEFLAG_API AWeapon : public AActor
{
    GENERATED_BODY()
private:

    UPROPERTY(EditAnywhere)
    class USceneComponent* SceneComponent;

    UPROPERTY(EditAnywhere)
    class USceneComponent* MuzzleSceneComponent;

    UPROPERTY(EditDefaultsOnly, Category=Projectile)
    TSubclassOf<class AProjectile> ProjectileClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay, meta=(AllowPrivateAccess = "true"))
    USoundBase* FireSound;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay, meta=(AllowPrivateAccess = "true"))
    UAnimMontage* FireAnimation;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
    class UInputMappingContext* FireMappingContext;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
    class UInputAction* FireAction;

public:
    // Sets default values for this actor's properties
    AWeapon();

    UFUNCTION(Server, Reliable)
    void FireServer();

    UFUNCTION(Client, Reliable)
    void SetupActionBindingsClient();
    
protected:
    UPROPERTY(BlueprintReadOnly) class UArrowComponent* ArrowComponent;
    
    virtual void BeginPlay() override;

    UFUNCTION()
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
private:
    UPROPERTY(EditAnywhere, Replicated)
    ACaptureTheFlagCharacter* Character;

public:
    UFUNCTION(BlueprintPure)
    FORCEINLINE ACaptureTheFlagCharacter* GetCharacter() { return Character; }
    
    UFUNCTION(BlueprintCallable)
    FORCEINLINE void SetCharacter(ACaptureTheFlagCharacter* InCharacter) { Character = InCharacter; }

    UFUNCTION(BlueprintPure)
    FORCEINLINE FVector GetMuzzleLocation() { return MuzzleSceneComponent->GetComponentLocation(); }
};
