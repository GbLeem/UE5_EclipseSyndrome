#include "InteractableItem/Door.h"


ADoor::ADoor()
{
 	
	PrimaryActorTick.bCanEverTick = true;
	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
	RootComponent = DoorMesh;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetupAttachment(DoorMesh);
	
	MoveSpeed = 2.0f;
	RotationSpeed = 90.0f;
}


void ADoor::BeginPlay()
{
	Super::BeginPlay();
	//CurrentYaw = DoorMesh->GetRelativeRotation().Yaw;
	ClosedLocation = DoorMesh->GetRelativeLocation();
	ClosedRotation = DoorMesh->GetRelativeRotation();

	OpenLocation = ClosedLocation + FVector(-60.0f, 60.0f, 0.0f);
	OpenRotation = ClosedRotation + FRotator(0.0f, 90.0f, 0.0f);
}

void ADoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(bIsOpening)
	{
		MoveAndRotateDoor(DeltaTime);
	}
}

void ADoor::OpenDoor()
{
	bIsOpening = true;
}

void ADoor::MoveAndRotateDoor(float DeltaTime)
{
	if (!DoorMesh) return;
	
	FVector NewLocation = FMath::VInterpTo(DoorMesh->GetRelativeLocation(), OpenLocation, DeltaTime, MoveSpeed);
	FRotator NewRotation = FMath::RInterpTo(DoorMesh->GetRelativeRotation(), OpenRotation, DeltaTime, MoveSpeed);
	if (FVector::Dist(NewLocation, OpenLocation) < 1.0f && FMath::Abs(NewRotation.Yaw - OpenRotation.Yaw) < 1.0f)
	{
		NewLocation = OpenLocation;
		NewRotation = OpenRotation;
		bIsOpening = false;
	}
	
	DoorMesh->SetRelativeLocation(NewLocation);
	DoorMesh->SetRelativeRotation(NewRotation);
	/*if (!DoorMesh) return;

	FRotator NewRotation = DoorMesh->GetRelativeRotation();
	NewRotation.Yaw = FMath::FInterpTo(NewRotation.Yaw, TargetYaw, DeltaTime, RotationSpeed);
	
	if (FMath::Abs(NewRotation.Yaw - TargetYaw) < 1.0f)
	{
		NewRotation.Yaw = TargetYaw;
		bIsOpening = false;
	}
	
	DoorMesh->SetRelativeRotation(NewRotation);*/
}
