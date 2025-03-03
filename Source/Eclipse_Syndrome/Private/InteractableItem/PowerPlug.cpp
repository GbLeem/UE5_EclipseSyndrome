#include "InteractableItem/PowerPlug.h"
#include "CableComponent.h"

APowerPlug::APowerPlug()
{
	PrimaryActorTick.bCanEverTick = true;

	StaticMeshCompo = CreateDefaultSubobject<UStaticMeshComponent>("StaticMesh");
	SetRootComponent(StaticMeshCompo);
	StaticMeshCompo->SetSimulatePhysics(true);

	CableCompo = CreateDefaultSubobject<UCableComponent>("Cable");
	CableCompo->SetupAttachment(StaticMeshCompo);
	CableCompo->CableWidth = 25.0f;

	this->Tags.AddUnique(FName("Grabbable"));
}

void APowerPlug::BeginPlay()
{
	Super::BeginPlay();
	
}

void APowerPlug::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

