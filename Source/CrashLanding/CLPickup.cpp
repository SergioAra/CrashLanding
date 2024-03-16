#include "CLPickup.h"

#include "CLShip.h"
#include "CrashLandingGameModeBase.h"

// Sets default values
ACLPickup::ACLPickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;
	MeshComp->SetCollisionResponseToAllChannels(ECR_Overlap);

}

// Called when the game starts or when spawned
void ACLPickup::BeginPlay()
{
	Super::BeginPlay();

	MeshComp->OnComponentBeginOverlap.AddDynamic(this, &ACLPickup::OnPickup);
	
}

void ACLPickup::OnPickup(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ACLShip* OtherShip = Cast<ACLShip>(OtherActor);
	// Check if the overlapping ship is valid
	if(OtherShip)
	{
		// Call the action to be executed once the item is picked up by the player
		PickupAction(OtherShip);
	}
}

void ACLPickup::PickupAction(ACLShip* PickerShip)
{
	ACrashLandingGameModeBase* GameMode = Cast<ACrashLandingGameModeBase>(GetWorld()->GetAuthGameMode());
	//Check if the GameMode is valid
	if(GameMode)
	{
		// Decrease the counter that holds the amount of pickups in the level
		GameMode->DecreasePickupCount();
		// Destroy the actor
		Destroy();
	}
	
}


