#include "InteractableItem/Door.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"

ADoor::ADoor()
{
 	
	PrimaryActorTick.bCanEverTick = true;
	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
	RootComponent = DoorMesh;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetupAttachment(DoorMesh);
	
	MoveSpeed = 2.0f;
	RotationSpeed = 90.0f;


	static ConstructorHelpers::FObjectFinder<USoundBase> SoundAsset(TEXT("/Game/Yujin/Audio/LvOneDoorOpen.LvOneDoorOpen"));
	if (SoundAsset.Succeeded())
	{
		IDoorOpeningSound = SoundAsset.Object;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to load IDoorOpeningSound sound!"));
	}
}


void ADoor::BeginPlay()
{
	Super::BeginPlay();
	ClosedLocation = DoorMesh->GetRelativeLocation();
	ClosedRotation = DoorMesh->GetRelativeRotation();

	OpenLocation = ClosedLocation + FVector(-100.0f, 90.0f, 0.0f);
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
	if (IDoorOpeningSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			GetWorld(),
			IDoorOpeningSound,
			GetActorLocation()
		);
	}
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
}
