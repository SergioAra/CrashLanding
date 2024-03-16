#pragma once

#include "CoreMinimal.h"
#include "CLPickup.h"
#include "CLFuelPickup.generated.h"


UCLASS()
class CRASHLANDING_API ACLFuelPickup : public ACLPickup
{
	GENERATED_BODY()

	// Amount of fuel the pickup grants the player
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup", meta = (AllowPrivateAccess = "true"))
	float Fuel;

public:	
	// Sets default values for this actor's properties
	ACLFuelPickup();
	
protected:

	/*Override PickupAction function
	 * Adds fuel to the ship on pickup
	 */
	virtual void PickupAction(ACLShip* PickerShip) override;
};
