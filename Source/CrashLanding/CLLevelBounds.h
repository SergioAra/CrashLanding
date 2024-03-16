// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CLLevelBounds.generated.h"

class UBoxComponent;

// Delegate for when the player enters or leaves the level bounds
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FShipLeavingBoundsDelegate, bool, bIsLeaving);

UCLASS()
class CRASHLANDING_API ACLLevelBounds : public AActor
{
	GENERATED_BODY()

	// Bounds for the level
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* LevelBounds;

	// TimerHandle to check the amount of time the player is out of bounds
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level", meta = (AllowPrivateAccess = "true"))
	FTimerHandle OutOfBoundsTimer;

	// Time the player can be out of bounds
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level", meta = (AllowPrivateAccess = "true"))
	float OutOfBoundsTime;

	// Delegate to send information regarding the player leaving or entering the level bounds
	UPROPERTY(BlueprintAssignable)
	FShipLeavingBoundsDelegate LeavingBoundsDelegate;

	
	
public:	
	// Sets default values for this actor's properties
	ACLLevelBounds();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//Called when the player starts overlapping with the level bounds
	UFUNCTION()
	void OnBeginLevelBoundsOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	//Called when the player stops overlapping with the level bounds
    UFUNCTION()
	void OnEndLevelBoundsOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	

	
};
