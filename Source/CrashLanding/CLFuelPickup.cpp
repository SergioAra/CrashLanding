#include "CLFuelPickup.h"
#include "CLShip.h"

ACLFuelPickup::ACLFuelPickup():
	Fuel(50.f)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

void ACLFuelPickup::PickupAction(ACLShip* PickerShip)
{
	// Add fuel to the ship
	PickerShip->AddFuel(Fuel);
	// Destroy the pickup
	Destroy();
}
