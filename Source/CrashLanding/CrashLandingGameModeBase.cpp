#include "CrashLandingGameModeBase.h"
#include "CLPickup.h"
#include "Kismet/GameplayStatics.h"

void ACrashLandingGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	
	CountLevelPickups();
}

void ACrashLandingGameModeBase::CountLevelPickups()
{
	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(this, PickupClass,OutActors);
	PickupCount = OutActors.Num();
}

void ACrashLandingGameModeBase::DecreasePickupCount()
{
	PickupCount -= 1;
}
