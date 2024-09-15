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
    UPROPERTY(VisibleAnywhere)
    class USceneComponent* SceneComponent;

    UPROPERTY(VisibleAnywhere)
    class USceneComponent* MuzzleSceneComponent;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<class AProjectile> ProjectileClass;

    UPROPERTY(VisibleAnywhere)
    USoundBase* FireSound;
    
    UPROPERTY(VisibleAnywhere)
    UAnimMontage* FireAnimation;

    UPROPERTY(VisibleAnywhere)
    class UInputMappingContext* FireMappingContext;

    UPROPERTY(VisibleAnywhere)
    class UInputAction* FireAction;

    UPROPERTY(Replicated)
    ACaptureTheFlagCharacter* Character;
public:
    UFUNCTION(Server, Reliable)
    void FireServer();

    UFUNCTION(Client, Reliable)
    void SetupActionBindingsClient();
protected:
    UPROPERTY(BlueprintReadOnly)
    class UArrowComponent* ArrowComponent;
    
    virtual void SetOwner(AActor* InOwner) override;
    virtual void BeginPlay() override;

    UFUNCTION()
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
public:
    AWeapon();
    
    UFUNCTION(BlueprintPure)
    FORCEINLINE FVector GetMuzzleLocation() { return MuzzleSceneComponent->GetComponentLocation(); }
};
