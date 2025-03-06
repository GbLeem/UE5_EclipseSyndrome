#include "InteractableItem/ControlPannel.h"
#include "Components/BoxComponent.h"
#include "InteractableItem/PowerPlug.h"
#include "InteractableItem/LevelTwoDoor.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
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

	CollisionBox->SetHiddenInGame(false);
	CollisionBox->SetVisibility(true);


	//ConstructorHelpers::FObjectFinder<UMaterialInterface> MatFinder(TEXT("/Game/Yujin/Materials/M_ChestOutliner.M_ChestOutliner"));

	//if (MatFinder.Succeeded())
	//{
	//	OutlineMaterial = MatFinder.Object;
	//}


	//if (ControlPanelMesh && OutlineMaterial)
	//{
	//	ControlPanelMesh->SetMaterial(25, OutlineMaterial);
	//}


	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> NiagaraEffectAsset(TEXT("/Game/Vefects/Zap_VFX/VFX/Zap/Particles/NS_Zap_02_Blue.NS_Zap_02_Blue"));
	if (NiagaraEffectAsset.Succeeded())
	{
		NiagaraEffect = NiagaraEffectAsset.Object;
	}

}


void AControlPannel::BeginPlay()
{
	Super::BeginPlay();


	LevelTwoDoor = Cast<ALevelTwoDoor>(UGameplayStatics::GetActorOfClass(GetWorld(), ALevelTwoDoor::StaticClass()));

	if (ControlPanelMesh)
	{
		RedMID = ControlPanelMesh->CreateDynamicMaterialInstance(5);
		GreenMID = ControlPanelMesh->CreateDynamicMaterialInstance(4);

		if (RedMID)
		{
			RedMID->SetScalarParameterValue(TEXT("EmissivePower"), 10.0f);
			UE_LOG(LogTemp, Warning, TEXT("RedMID set Successfully"));
		}

		if (GreenMID)
		{
			GreenMID->SetScalarParameterValue(TEXT("EmissivePower"), 0.0f);
			UE_LOG(LogTemp, Warning, TEXT("GreenMID set Successfully"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("both light failed"));
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
	UE_LOG(LogTemp, Warning, TEXT("ControlPanel Overlapped with: %s"), *OtherActor->GetName());

	//after cable component is finished, fix Actor setting to Cable
	APowerPlug* PowerPlug = Cast<APowerPlug>(OtherActor);
	if (PowerPlug && PowerPlug->StaticMeshCompo)
	{
		UE_LOG(LogTemp, Warning, TEXT("PowerPlug detected, attaching to panel"));
		PowerPlug->AttachToPanel(this);

		bIsPlugConnected = true;
		ActivatePanel(true);

		

		if (NiagaraEffect)
		{
			FVector ForwardOffset = GetActorForwardVector() * 200.0f;

			NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				GetWorld(),
				NiagaraEffect,
				GetActorLocation() + ForwardOffset,
				GetActorRotation()
			);
			

		}
		if (NiagaraComp)
		{
			NiagaraComp->SetFloatParameter(TEXT("Floor Scorch Lifetime"), 20.0f);
		}
		

	}
}


void AControlPannel::OnOverlapEnd(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	if (!bIsPlugConnected)
	{
		ActivatePanel(false);
	}
	
}


void AControlPannel::ActivatePanel(bool IsActivated)
{
	if (RedMID)
	{
		RedMID->SetScalarParameterValue(TEXT("EmissivePower"), IsActivated ? 0.0f : 10.0f);
	}
	if (GreenMID)
	{
		GreenMID->SetScalarParameterValue(TEXT("EmissivePower"), IsActivated ? 10.0f : 0.0f);
	}
	LevelTwoDoor->OpenLevelTwoDoor();
	if (LevelTwoDoor)
	{
		LevelTwoDoor->OpenLevelTwoDoor();
	}
}


FVector AControlPannel::GetPlugPosition() const
{
	return CollisionBox->GetComponentLocation();
}

FQuat AControlPannel::GetPlugRotation() const
{
	return CollisionBox->GetComponentQuat();
}


