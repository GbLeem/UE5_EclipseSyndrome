#include "InteractableItem/LevelTwoDoor.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"

ALevelTwoDoor::ALevelTwoDoor()
{
	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	RootComponent = StaticMeshComp;

	static ConstructorHelpers::FObjectFinder<UStaticMesh>StaticMeshAsset(TEXT("/Game/Yujin/Assets/door/Door_MetalWire"));
	if (StaticMeshAsset.Succeeded())
	{
		StaticMeshComp->SetStaticMesh(StaticMeshAsset.Object);
	}
	PrimaryActorTick.bCanEverTick = true;
	
	//sound effect
	static ConstructorHelpers::FObjectFinder<USoundBase> SoundAsset(TEXT("/Game/Yujin/Audio/IronDoorOpen.IronDoorOpen"));
	if (SoundAsset.Succeeded())
	{
		IDoorOpeningSound = SoundAsset.Object;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to load IDoorOpeningSound sound!"));
	}



}

void ALevelTwoDoor::BeginPlay()
{
	Super::BeginPlay();
	ClosedPosition = GetActorLocation();
	OpenPosition = ClosedPosition + FVector(0.0f, 330.0f, 0.0f);
}

void ALevelTwoDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bIsOpening)
	{
		FVector NewLocation = FMath::VInterpTo(GetActorLocation(), OpenPosition, DeltaTime, MoveSpeed);
		SetActorLocation(NewLocation);

		if (FVector::Dist(NewLocation, OpenPosition) < 1.0f)
		{
			bIsOpening = false;
		}
	}
}
void ALevelTwoDoor::OpenLevelTwoDoor()
{
	bIsOpening = true;
	if (IDoorOpeningSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			GetWorld(),
			IDoorOpeningSound,
			GetActorLocation(),
			0.1f
		);
	}
}

