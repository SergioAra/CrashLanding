#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CLLandingPlatform.generated.h"

class UBoxComponent;

UCLASS()
class CRASHLANDING_API ACLLandingPlatform : public AActor
{
	GENERATED_BODY()

	// Mesh for the Landing Platform
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* MeshComp;

	// Area the player overlaps with to complete the level
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* LandingZone;

	// Name of the next level, leave as None to close the came
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Level", meta = (AllowPrivateAccess = "true"))
	FName NextLevel;

	
	
public:	
	// Sets default values for this actor's properties
	ACLLandingPlatform();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called when the player overlaps with the landing zone.
	UFUNCTION()
	void OnBeginLandingZoneOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);



};
