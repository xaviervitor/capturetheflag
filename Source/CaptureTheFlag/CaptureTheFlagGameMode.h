#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"

#include "CaptureTheFlagGameMode.generated.h"

UCLASS()
class CAPTURETHEFLAG_API ACaptureTheFlagGameMode : public AGameMode
{
	GENERATED_BODY()
private:	

	float RaspawnTime = 5.0f;

public:
	ACaptureTheFlagGameMode();
	
	void OnDeath(AController* DeadPlayerController, AController* KillerPlayerController);
	void OnFall(AController* PlayerController);
};