#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CrashLandingGameModeBase.generated.h"

class ACLPickup;

UCLASS()
class CRASHLANDING_API ACrashLandingGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

	// Subclass of the pickup to be counted at the beginning of the level
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Property", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<ACLPickup> PickupClass;

	// Current amount of pickups 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Property", meta = (AllowPrivateAccess = "true"))
	float PickupCount;

protected:

	virtual void BeginPlay() override;

	// Cont the PickupClass actors in the level
	void CountLevelPickups();

public:

	// Decrease the PickupCount by one. Called when a Pickup actor is picked up by the player 
	void DecreasePickupCount();

	// Get the current amount of pickups 
	FORCEINLINE float GetPickupCount() const {return PickupCount;}

};
