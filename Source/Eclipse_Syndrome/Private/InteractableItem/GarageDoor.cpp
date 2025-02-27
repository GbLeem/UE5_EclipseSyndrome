#include "InteractableItem/GarageDoor.h"


AGarageDoor::AGarageDoor()
{

	PrimaryActorTick.bCanEverTick = false;
	GDoorMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GDoorMesh"));
	RootComponent = GDoorMesh;
}


void AGarageDoor::BeginPlay()
{
	Super::BeginPlay();
	
}


void AGarageDoor::OpenGarageDoor()
{
	UE_LOG(LogTemp, Warning, TEXT("Garage Door is opening!"));
	if (!GDoorMesh) return;

	FVector ClosedPosition = GDoorMesh->GetRelativeLocation();
	FVector OpenedPosition = ClosedPosition + FVector(0, 0, 200);

	GDoorMesh->SetRelativeLocation(FMath::VInterpTo(ClosedPosition, OpenedPosition, GetWorld()->GetDeltaSeconds(), 3.0f));
}