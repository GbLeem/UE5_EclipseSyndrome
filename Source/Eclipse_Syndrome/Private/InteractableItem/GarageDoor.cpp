#include "InteractableItem/GarageDoor.h"
#include "Character/PlayerCharacter.h"

AGarageDoor::AGarageDoor()
{

	PrimaryActorTick.bCanEverTick = true;
	GDoorMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GDoorMesh"));
	RootComponent = GDoorMesh;

	InteractionZone = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionZone"));
	InteractionZone->SetupAttachment(RootComponent);

	InteractionZone->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractionZone->SetCollisionResponseToAllChannels(ECR_Ignore);
	InteractionZone->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	InteractionZone->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Overlap);


	InteractionZone->OnComponentBeginOverlap.AddDynamic(this, &AGarageDoor::OnOverlapBegin);
	InteractionZone->OnComponentEndOverlap.AddDynamic(this, &AGarageDoor::OnOverlapEnd);

	InteractionZone->SetHiddenInGame(false);
	InteractionZone->SetVisibility(true);

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


void AGarageDoor::OnOverlapBegin(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
) {
	APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);
	if (Player)
	{
		bCanUseKey = true;
		UE_LOG(LogTemp, Warning, TEXT("Inside Gdoor collision"));
	}
}

void AGarageDoor::OnOverlapEnd(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);
	if (Player)
	{
		bCanUseKey = false;
		UE_LOG(LogTemp, Warning, TEXT("Outside of Gdoor collision."));
	}
}

bool AGarageDoor::UnlockDoor()
{
	if (bCanUseKey)
	{
		UE_LOG(LogTemp, Warning, TEXT("Using Key - Garage Door Unlocking"));
		OpenGarageDoor();
		return true;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Too far from the door! Cannot use Key."));
		return false;
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