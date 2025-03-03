#include "InteractableItem/PowerPlug.h"
#include "CableComponent.h"

APowerPlug::APowerPlug()
{
	PrimaryActorTick.bCanEverTick = false;

	StaticMeshCompo = CreateDefaultSubobject<UStaticMeshComponent>("StaticMesh");
	SetRootComponent(StaticMeshCompo);
	StaticMeshCompo->SetSimulatePhysics(true);

	CableCompo = CreateDefaultSubobject<UCableComponent>("Cable");
	CableCompo->SetupAttachment(StaticMeshCompo);
	CableCompo->CableWidth = 25.0f;

	this->Tags.AddUnique(FName("Grabbable"));

	StaticMeshCompo->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	StaticMeshCompo->SetGenerateOverlapEvents(true);

}

void APowerPlug::BeginPlay()
{
	Super::BeginPlay();
	StaticMeshCompo->OnComponentBeginOverlap.AddDynamic(this, &APowerPlug::OnOverlapBegin);
	
}


void APowerPlug::OnOverlapBegin(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	AControlPannel* ControlPannel = Cast<AControlPannel>(OtherActor);

	if (ControlPannel)
	{
		UE_LOG(LogTemp, Warning, TEXT("PowerPlug Overlapped with ControlPannel"));
	}
}

void APowerPlug::AttachToPanel(AControlPannel* ControlPannel)
{
	if (!ControlPannel) return;


	FVector SocketLocation = ControlPannel->GetPlugPosition();
	FQuat TargetRotation = ControlPannel->GetPlugRotation();

	FVector PlugSize = StaticMeshCompo->Bounds.BoxExtent;
	SocketLocation.Z -= PlugSize.Z;

	//FVector Offset = TargetRotation.GetForwardVector() * -10.0f;
	//SocketLocation += Offset;


	FRotator AdjustedRotation = TargetRotation.Rotator();
	AdjustedRotation.Pitch -= 90.0f;
	AdjustedRotation.Yaw -= 90.0f;
	

	SetActorLocation(SocketLocation);
	SetActorRotation(AdjustedRotation);

	StaticMeshCompo->SetSimulatePhysics(false);
	StaticMeshCompo->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
}