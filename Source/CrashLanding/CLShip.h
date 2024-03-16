#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameFramework/Pawn.h"
#include "CLShip.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UBoxComponent;

// Types of thrusters the ship can use
UENUM(BlueprintType)
enum class EThrusterType: uint8
{
	ETT_Jet UMETA(DisplayName = "JetThruster"),
	ETT_Burst UMETA(DisplayName = "BurstThruster"),
	
	ETT_MAX UMETA(DisplayName = "Default Max"),
};

// Table structure to hold data according to the thruster type
USTRUCT(BlueprintType)
struct FShipDataTable : public FTableRowBase
{
	GENERATED_BODY();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ImpulseMagnitude;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxChargeTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FuelUsedPerFullCharge;
	
};

UCLASS()
class CRASHLANDING_API ACLShip : public APawn
{
	GENERATED_BODY()

	// Mesh of the ship
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* MeshComp;

	// Camera that follows the ship
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* CameraComp;

	// Spring arm positioning the camera behind the character
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* SpringArmComp;

	// Rate at which the ship can turn from side to side
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float TurnRate;

	// Magnitude value for the impulse to be applied at Max charge
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float ImpulseMagnitude;

	// Time in seconds when the charge started
	float ChargeTimeStart;

	// Time the charge has been held
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float ChargeTime;

	// True if the impulse is currently charging, false otherwise 
	bool bCharging;

	// Time it takes to have a full charge for the thruster
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float MaxChargeTime;

	// Quotient of the ChargeTime and MaxChargeTime
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float ChargePercentage;

	// Current amount of fuel
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float Fuel;

	// Max amount of fuel
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float MaxFuel;

	// Amount of fuel used then the ship is thrusting at the maximum charge
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float FuelUsedPerFullCharge;

	// True when the ship is able to turn sideways, false when the player landed to prevent turning
	bool bCanTurn;

	// Class of the alternate viewpoints in the level
	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	TSubclassOf<ACameraActor> AlternateViewpointClass;

	// Array of alternate viewpoint in the level
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TArray<AActor*> AlternateViewpoints;

	// Index of the current viewpoint
	int32 CurrentViewpointIndex;

	// Enum that holds the current Thruster type
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	EThrusterType ThrusterType;

	// Path to the table that holds the Thruster type data
	FString ThrusterTablePath;

	// Object that holds the imported data of the Ship data table
	UPROPERTY(VisibleAnywhere)
	UDataTable* ThrusterTableObject;

	// True if the level has been completed, either by winning or losing. False while still playing 
	bool bLevelComplete;
	

public:
	// Sets default values for this pawn's properties
	ACLShip();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called when the "TurnRight" keys are pressed, used for rotating the ship
	void TurnRight(float Value);

	/* Adds impulse to the ship proportional the charge
	 * Called each tick if ThrusterType is ETT_Jet
	 * Called in EndCharge() if ThrusterType is ETT_Burst */
	void Impulse();

	// Called when the "Impulse" key is pressed, sets the bCharging and ChargeTimeStart variables
	void StartCharge();

	// Called each tick, updates the Charge and ChargePercentage variables. 
	void Charge();

	// Called when the "Impulse" key is released, resets the bCharging, Charge and ChargePercentage variables. 
	void EndCharge();

	/* Uses fuel proportional the charge
	 * Called each tick if ThrusterType is ETT_Jet
	 * Called in EndCharge() if ThrusterType is ETT_Burst */
	void UseFuel();

	// Called when the ship hits a surface, handles resetting the ship and ending the level if it has ran out of fuel
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	// Called when the "SwitchViewPoint" key is pressed, switches the player viewpoint 
	void SwitchViewpoint();

	// Called when switching thruster type, sets the thruster type and its related variables
	void SetThrusterType(EThrusterType NewThruster);

	// Called when the "SwitchThruster" key is pressed, currently switches between the two available modes
	void SwitchThrusterType();
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Called to set the ship upright and stop its movement when landing
	void SetShipPositionOnLanding(FRotator NewRotator);

	// Returns the current fuel amount
	FORCEINLINE float GetFuel() const {return Fuel;}

	// Returns true if the level has been completed, false while still playing 
	FORCEINLINE float IsLevelComplete() const {return bLevelComplete;}

	// Sets the level complete
	FORCEINLINE void SetLevelComplete(bool bNewLevelComplete) {bLevelComplete = bNewLevelComplete;}

	/*Called when the conditions for completing a level are met, either by winning or losing
	 * bSuccess is true when the level was complete by winning, false if complete by losing
	 * NextLevel holds the name of the next level to load on completion
	 */
	UFUNCTION(BlueprintImplementableEvent)
	void OnLevelComplete(bool bSuccess, FName NextLevel);

	// Add fuel to the ship
	void AddFuel(float FuelToAdd);

};
