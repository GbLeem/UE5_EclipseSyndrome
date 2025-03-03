#include "InteractableItem/ControlPannel.h"
#include "Components/BoxComponent.h"
#include "CableActor.h"

AControlPannel::AControlPannel()
{

	PrimaryActorTick.bCanEverTick = false;

	ControlPanelMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ControlPanelMesh"));
	RootComponent = ControlPanelMesh;


	static ConstructorHelpers::FObjectFinder<UStaticMesh>StaticMeshAsset(TEXT("/Game/Yujin/Assets/Level2/ControlPannel/test/ControlPanel"));
	if (StaticMeshAsset.Succeeded())
	{
		ControlPanelMesh->SetStaticMesh(StaticMeshAsset.Object);
	}



	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetupAttachment(RootComponent);

	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	CollisionBox->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Overlap);


	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AControlPannel::OnOverlapBegin);
	CollisionBox->OnComponentEndOverlap.AddDynamic(this, &AControlPannel::OnOverlapEnd);

}


void AControlPannel::BeginPlay()
{
	Super::BeginPlay();

	if (ControlPanelMesh)
	{
		RedMID = ControlPanelMesh->CreateDynamicMaterialInstance(0);
		GreenMID = ControlPanelMesh->CreateDynamicMaterialInstance(1);

		if (RedMID)
		{
			RedMID->SetScalarParameterValue(TEXT("EmissivePower"), 10.0f);
		}

		if (GreenMID)
		{
			GreenMID->SetScalarParameterValue(TEXT("EmissivePower"), 0.0f);
		}
	}
}


void AControlPannel::OnOverlapBegin(
	UPrimitiveComponent* Overlappedcomponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	//after cable component is finished, fix Actor setting to Cable
	if (OtherActor)
	{
		ActivatePanel(true);
	}
}


void AControlPannel::OnOverlapEnd(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	ActivatePanel(false);
}


void AControlPannel::ActivatePanel(bool IsActivated)
{
	if (RedMID)
	{
		RedMID->SetScalarParameterValue(TEXT("EmissvePower"), bIsActivated ? 0.0f : 10.0f);
	}
	if (GreenMID)
	{
		GreenMID->SetScalarParameterValue(TEXT("EmissvePower"), bIsActivated ? 10.0f : 0.0f);
	}
}