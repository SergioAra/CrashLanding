#include "CLLevelBounds.h"
#include "CLShip.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ACLLevelBounds::ACLLevelBounds():
	OutOfBoundsTime(5.f)
{
	PrimaryActorTick.bCanEverTick = false;

	LevelBounds = CreateDefaultSubobject<UBoxComponent>(TEXT("LevelBounds"));
	RootComponent = LevelBounds;

}

// Called when the game starts or when spawned
void ACLLevelBounds::BeginPlay()
{
	Super::BeginPlay();

	LevelBounds->OnComponentBeginOverlap.AddDynamic(this, &ACLLevelBounds::OnBeginLevelBoundsOverlap);
	LevelBounds->OnComponentEndOverlap.AddDynamic(this, &ACLLevelBounds::OnEndLevelBoundsOverlap);
	
}

void ACLLevelBounds::OnBeginLevelBoundsOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ACLShip* OtherShip = Cast<ACLShip>(OtherActor);
	
	// Check if the OtherShip is valid
	if(OtherShip)
	{
		// Clear the OutOfBoundsTimer as the player returned to the level bounds
		GetWorldTimerManager().ClearTimer(OutOfBoundsTimer);
		// Broadcast the LeavingBoundsDelegate
		LeavingBoundsDelegate.Broadcast(false);
	}
}

void ACLLevelBounds::OnEndLevelBoundsOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ACLShip* OtherShip = Cast<ACLShip>(OtherActor);
	
	// Check if the OtherShip is valid
	if(OtherShip)
	{
		// Create TimerDelegate
		FTimerDelegate TimerDelegate;
		
		/*Bind the OnLevelComplete function to the TimerDelegate
		 * if the timer ends the level is set as completed and the level is reloaded as the player lost*/
		TimerDelegate.BindUFunction(OtherShip, "OnLevelComplete", false, FName(UGameplayStatics::GetCurrentLevelName(this)));
		
		//Start a timer to end the level if the player stays out of bounds too long
		GetWorldTimerManager().SetTimer(OutOfBoundsTimer, TimerDelegate, OutOfBoundsTime, false);
		
		// Broadcast the LeavingBoundsDelegate
		LeavingBoundsDelegate.Broadcast(true);
	}
}



