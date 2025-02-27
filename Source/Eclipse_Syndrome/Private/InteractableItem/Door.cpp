#include "InteractableItem/Door.h"


ADoor::ADoor()
{
 	
	PrimaryActorTick.bCanEverTick = true;
	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
	RootComponent = DoorMesh;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetupAttachment(DoorMesh);
	

	
}


void ADoor::OpenDoor()
{
	GetWorldTimerManager().SetTimer(DoorOpenTimer, this, &ADoor::RotateDoor, 0.01f, true);
	
}

void ADoor::RotateDoor()
{
	FRotator CurrentRotation = GetActorRotation();
	CurrentRotation.Yaw += RotationSpeed * 0.01f;
	if (CurrentRotation.Yaw >= TargetYaw)
	{
		CurrentRotation.Yaw = TargetYaw;
		GetWorldTimerManager().ClearTimer(DoorOpenTimer);
	}
	
	SetActorRotation(CurrentRotation);
}
