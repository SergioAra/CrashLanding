#include "CLShip.h"
#include "Camera/CameraActor.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ACLShip::ACLShip():
	TurnRate(100.f),
	ImpulseMagnitude(0.f),
	ChargeTimeStart(0.f),
	ChargeTime(0.f),
	bCharging(false),
	MaxChargeTime(0.f),
	ChargePercentage(0.f),
	MaxFuel(100.f),
	FuelUsedPerFullCharge(0.f),
	bCanTurn(true),
	CurrentViewpointIndex(-1),
	ThrusterType(EThrusterType::ETT_Jet),
	ThrusterTablePath(TEXT("DataTable'/Game/DataTables/ThrusterDataTable.ThrusterDataTable'")),
	bLevelComplete(false)
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create mesh component
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	// Set it to simulate physics
	MeshComp->SetSimulatePhysics(true);
	// Set it to notify hit collisions 
	MeshComp->SetNotifyRigidBodyCollision(true);
	// Set to ignore radial impulses and force so that the ship does not spin after hitting surfaces
	MeshComp->bIgnoreRadialForce = true;
	MeshComp->bIgnoreRadialImpulse = true;
	// Set the root component as the mesh
	RootComponent = MeshComp;

	// Create the spring arm
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	// Attach it to the root
	SpringArmComp->SetupAttachment(RootComponent);
	// Prevent the spring arm to follow the rotation of the ship
	SpringArmComp->bInheritPitch = false;
	SpringArmComp->bInheritRoll = false;
	SpringArmComp->bInheritYaw = false;

	// Create the camera
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	// Attach it to the spring arm
	CameraComp->SetupAttachment(SpringArmComp);

}

// Called when the game starts or when spawned
void ACLShip::BeginPlay()
{
	Super::BeginPlay();
	
	MeshComp->OnComponentHit.AddDynamic(this, &ACLShip::OnHit);

	Fuel = MaxFuel;

	// Get all actors of the AlternateViewpoint class
	UGameplayStatics::GetAllActorsOfClass(this, AlternateViewpointClass, AlternateViewpoints);

	// Load data from the ship data table
	ThrusterTableObject = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, *ThrusterTablePath));

	// Set the thruster type to the default value
	SetThrusterType(ThrusterType);
}

void ACLShip::TurnRight(float Value)
{
	// Check if the ship should be able to turn
	if(bCanTurn)
	{
		// Create a rotator
		FRotator DeltaRotation = FRotator::ZeroRotator;
		// Fill the Roll with the new value scaled by the TurnRate and DeltaTime
		DeltaRotation.Roll = Value * TurnRate * UGameplayStatics::GetWorldDeltaSeconds(this);
		// Reset the angular velocity of the ship
		MeshComp->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
		// Add the rotation to the ship
		AddActorLocalRotation(DeltaRotation, false, nullptr, ETeleportType::TeleportPhysics);
	}
	
}

void ACLShip::Impulse()
{
	if(ChargePercentage<= 0.f)
	{
		// Early return if there is no charge
		return;
	}

	// Get the ImpulseVector directed at the ship's direction and scaled by the charge and the ImpulseMagnitude
	const FVector ImpulseVector = MeshComp->GetUpVector() * ImpulseMagnitude * ChargePercentage;
	// Add impulse to the ship
	MeshComp->AddImpulse(ImpulseVector, NAME_None, true);
	// Allow the ship to be able to turn
	bCanTurn = true;
}

void ACLShip::StartCharge()
{
	bCharging = true;
	ChargeTimeStart = GetWorld()->TimeSeconds;
}

void ACLShip::Charge()
{
	if(!bCharging || Fuel <= 0.f)
	{
		// Early return if it is not charging or if there is no fuel
		return;
	}

	// Get current charge by subtracting the current game time and the charge time start 
	ChargeTime = GetWorld()->TimeSeconds - ChargeTimeStart;

	// If the charge time is more or equal to the charge time stop charging and set the ChargeTime to the MaxChargeTime
	if(ChargeTime >= MaxChargeTime)
	{
		bCharging = false;
		ChargeTime = MaxChargeTime;
	}

	// Get the quotient of the ChargeTime and the MaxChargeTime 
	ChargePercentage = ChargeTime/MaxChargeTime;
}

void ACLShip::EndCharge()
{
	// Stop charging 
	bCharging = false;

	// If the thruster type is Burst, use the fuel and impulse the ship
	if(ThrusterType == EThrusterType::ETT_Burst)
	{
		UseFuel();
		Impulse();
	}

	// Reset the ChargePercentage and ChargeTime;
	ChargePercentage = 0.f;
	ChargeTime = 0.f;
}

void ACLShip::UseFuel()
{

	if(ChargePercentage <= 0.f)
	{
		// Early return if there is no ChargePercentage
		return;
	}

	// Get the used by the charge 
	const float FuelUsed = FuelUsedPerFullCharge * ChargePercentage;
	// Get the new fuel value after subtracting the used fuel
	const float NewFuel = Fuel - FuelUsed;
	// If the used fuel is more than 0, set the new fuel as the current fuel
	if(NewFuel > 0)
	{
		Fuel = NewFuel;
	}
	else
	{
		// if the used fuel is more than the available fuel, recalculate the ChargePercentage that can be used
		ChargePercentage = Fuel/FuelUsedPerFullCharge;
		// and set the fuel to 0
		Fuel = 0.f;
		
	}
}

void ACLShip::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	// When the ship hits a surface, do line trace downwards to check if there is ground
	FHitResult OutHitResult;
	const FVector TraceStart = GetActorLocation();
	const FVector TraceEnd = TraceStart + FVector(0.f, 0.f, -100.f);
	GetWorld()->LineTraceSingleByChannel(OutHitResult, TraceStart, TraceEnd, ECC_Visibility);
	// If the hit was successful  
	if(OutHitResult.bBlockingHit)
	{
		// Create a rotator from the Z axis of the hit result normal  
		FRotator NewRotation = FRotationMatrix::MakeFromZ(OutHitResult.Normal).Rotator();
		// Check the Y axis magnitude of the hit result normal and multiply the Roll by the inverse magnitude 
		NewRotation.Roll *= (OutHitResult.Normal.Y < 0 ? 1.f : -1.f);
		// Set the Yaw and Pitch to 0.f as the ship does not rotate on those axes
		NewRotation.Yaw = 0.f;
		NewRotation.Pitch = 0.f;

		// Set the ship position to the new rotation
		SetShipPositionOnLanding(NewRotation);
		//Prevent the ship from being able to turn
		bCanTurn = false;

		// If the fuel is empty and the level has not been completed
		if(Fuel <= 0.f && !bLevelComplete)
		{
			// Set the level as complete
			bLevelComplete = true;
			// Reload the level as the player lost
			OnLevelComplete(false, FName(UGameplayStatics::GetCurrentLevelName(this)));
		}
	}
}

void ACLShip::SwitchViewpoint()
{
	if(!AlternateViewpointClass)
	{
		// Early return if no AlternateViewpointClass is set
		return;
	}
	
	if(AlternateViewpoints.Num() <= 0)
	{
		// Early return if no alternate viewpoints were found
		return;
	}

	// Get the player controller
	APlayerController* PlayerController = Cast<APlayerController>(GetController());

	// Increment the CurrentViewpointIndex
	CurrentViewpointIndex++;

	// Check if the current index is inside the AlternateViewpoints array
	if(AlternateViewpoints.Num() > CurrentViewpointIndex)
	{
		// Get the viewpoint in the current index
		AActor* NewViewpoint = AlternateViewpoints[CurrentViewpointIndex];
		// If the player controller is valid
		if(PlayerController)
		{
			// Set the view target to the new viewpoint
			PlayerController->SetViewTarget(NewViewpoint);
		}
	}
	// If the current index is not inside the array
	else
	{
		// Set the current index to -1 (default player camera)
		CurrentViewpointIndex = -1;
		// If the player controller is valid
		if(PlayerController)
		{
			// Set the view target to the default player camera
			PlayerController->SetViewTarget(this);
		}
	}
}

void ACLShip::SetThrusterType(EThrusterType NewThruster)
{
	// Set new thruster type
	ThrusterType = NewThruster;
	// Check if the ship data table is valid
	if(ThrusterTableObject)
	{
		// Create a data row
		FShipDataTable* ThrusterDataRow = nullptr;
		// Switch on the thruster type to get the corresponding data form the ship data table
		switch (ThrusterType)
		{
			case EThrusterType::ETT_Jet:
				ThrusterDataRow = ThrusterTableObject->FindRow<FShipDataTable>(FName("JetThruster"), TEXT(""));
				break;

			case EThrusterType::ETT_Burst:
				ThrusterDataRow = ThrusterTableObject->FindRow<FShipDataTable>(FName("BurstThruster"), TEXT(""));
				break;
			default: ;
		}

		// Check if the data row is valid
		if(ThrusterDataRow)
		{
			// Set the variables related to the thruster type
			ImpulseMagnitude = ThrusterDataRow->ImpulseMagnitude;
			MaxChargeTime = ThrusterDataRow->MaxChargeTime;
			FuelUsedPerFullCharge = ThrusterDataRow->FuelUsedPerFullCharge;
		}
	}
}

void ACLShip::SwitchThrusterType()
{
	// Switch on the thruster type and rotate between the two available types 
	switch(ThrusterType)
	{
	case EThrusterType::ETT_Jet:
		SetThrusterType(EThrusterType::ETT_Burst);
		break;
	case EThrusterType::ETT_Burst:
		SetThrusterType(EThrusterType::ETT_Jet);
		break;
	default:
		;
	}
}


// Called every frame
void ACLShip::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Charge by setting the ChargeTime and ChargePercentage on every frame bCharging is true
	Charge();
	// If the thruster type is set to Jet
	if(ThrusterType == EThrusterType::ETT_Jet)
	{
		// Use fuel and impulse the ship on every frame there is a charge and fuel available 
		UseFuel();
		Impulse();
	}
}

// Called to bind functionality to input
void ACLShip::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("TurnRight", this, &ACLShip::TurnRight);
	PlayerInputComponent->BindAction("Impulse", IE_Pressed, this, &ACLShip::StartCharge);
	PlayerInputComponent->BindAction("Impulse", IE_Released, this, &ACLShip::EndCharge);
	PlayerInputComponent->BindAction("SwitchViewPoint", IE_Pressed, this, &ACLShip::SwitchViewpoint);
	PlayerInputComponent->BindAction("SwitchThruster", IE_Pressed, this, &ACLShip::SwitchThrusterType);

}

void ACLShip::SetShipPositionOnLanding(FRotator NewRotator)
{
	// Set the ships rotation to the new rotation
	SetActorRotation(NewRotator, ETeleportType::ResetPhysics);
	// Stop the ship's movement 
	MeshComp->SetPhysicsLinearVelocity(FVector::ZeroVector);
}

void ACLShip::AddFuel(float FuelToAdd)
{
	// Add fuel up to the max fuel amount
	Fuel = FMath::Min(MaxFuel, Fuel + FuelToAdd);
}

