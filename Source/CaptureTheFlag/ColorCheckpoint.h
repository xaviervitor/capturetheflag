#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "ColorCheckpoint.generated.h"

UCLASS()
class CAPTURETHEFLAG_API AColorCheckpoint : public AActor
{
    GENERATED_BODY()
private:
    UPROPERTY(VisibleAnywhere)
    class UBoxComponent* BoxComponent;

    UPROPERTY(EditAnywhere)
    int32 Team = 0;
public:
    AColorCheckpoint();
protected:
    UFUNCTION()
    void OnBeginOverlap(UPrimitiveComponent* Component, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit);
public:
    UFUNCTION(BlueprintPure)
    FORCEINLINE int32 GetTeam() const { return Team; }
    
    UFUNCTION(BlueprintCallable)
    FORCEINLINE void SetTeam(int32 InTeam) { Team = InTeam; }
};
