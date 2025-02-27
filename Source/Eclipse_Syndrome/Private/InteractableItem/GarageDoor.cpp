#include "InteractableItem/GarageDoor.h"


AGarageDoor::AGarageDoor()
{

	PrimaryActorTick.bCanEverTick = true;
	GDoorMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GDoorMesh"));
	RootComponent = GDoorMesh;
}


void AGarageDoor::BeginPlay()
{
	Super::BeginPlay();
	

	if (GDoorMesh)
	{
		ClosedPosition = GDoorMesh->GetRelativeLocation();
		TargetPosition = ClosedPosition + FVector(0, 0, 450);
	}
}

void AGarageDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bIsOpening)
	{
		UE_LOG(LogTemp, Warning, TEXT("MoveDoorUp working"));
		MoveDoorUp(DeltaTime);
	}
}


void AGarageDoor::OpenGarageDoor()
{
	UE_LOG(LogTemp, Warning, TEXT("Garage Door is opening!"));
	if (!GDoorMesh) return;

	bIsOpening = true;
}

void AGarageDoor::MoveDoorUp(float DeltaTime)
{
	if (!GDoorMesh) return;

	FVector CurrentLocation = GDoorMesh->GetRelativeLocation();
	FVector NewLocation = FMath::VInterpTo(CurrentLocation, TargetPosition, DeltaTime, Speed);
	
	GDoorMesh->SetRelativeLocation(NewLocation);


	if (FVector::Dist(NewLocation, TargetPosition) < 1.0f)
	{
		GDoorMesh->SetRelativeLocation(TargetPosition);
		bIsOpening = false;
		UE_LOG(LogTemp, Warning, TEXT("GarageDoor fully opened"));
	}

}