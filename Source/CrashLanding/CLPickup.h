#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CLPickup.generated.h"

class ACLShip;

UCLASS()
class CRASHLANDING_API ACLPickup : public AActor
{
	GENERATED_BODY()

	// Mesh of the pickup
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* MeshComp;
	
public:	
	// Sets default values for this actor's properties
	ACLPickup();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called when the Pickup is overlapped
	UFUNCTION()
	void OnPickup(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	// Action to be executed once the item is picked up by a player
	virtual void PickupAction(ACLShip* PickerShip);
};
