#include "CLLandingPlatform.h"
#include "CLShip.h"
#include "CrashLandingGameModeBase.h"
#include "Components/BoxComponent.h"

// Sets default values
ACLLandingPlatform::ACLLandingPlatform()
{
	PrimaryActorTick.bCanEverTick = false;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	LandingZone = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	LandingZone->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void ACLLandingPlatform::BeginPlay()
{
	Super::BeginPlay();
	
	LandingZone->OnComponentBeginOverlap.AddDynamic(this, &ACLLandingPlatform::OnBeginLandingZoneOverlap);
	
}

void ACLLandingPlatform::OnBeginLandingZoneOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ACLShip* OtherShip = Cast<ACLShip>(OtherActor);
	if(!OtherShip)
	{
		// Early return if the OtherShip is nullptr
		return;
	}
	
	ACrashLandingGameModeBase* GameMode = Cast<ACrashLandingGameModeBase>(GetWorld()->GetAuthGameMode());
	
	/*Check if the GameMode is valid,
	 *if the player has collected all of the pickups
	 *and the level has not been completed by other conditions*/
	if(GameMode && GameMode->GetPickupCount() <= 0 && !OtherShip->IsLevelComplete())
	{
		// Set the level as complete
		OtherShip->SetLevelComplete(true);
		// Handle level complete
		OtherShip->OnLevelComplete(true, NextLevel);
	}
}





