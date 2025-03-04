#include "InteractableItem/PuzzleItem/PuzzleBlock.h"
#include "InteractableItem/PuzzleItem/PuzzleBSlot.h"
#include "Character/PlayerCharacter.h"

APuzzleBlock::APuzzleBlock()
{

	PrimaryActorTick.bCanEverTick = false;
	
	ItemType = "PuzzleBlock";
	ItemNumber = 3;
	ItemAmount = 1;
	
	//SlotCollision = for collision with puzzle slot
	//InteractionCollision = for collision with player
	

	InteractionCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionCollision"));
	InteractionCollision->SetBoxExtent(FVector(200.f, 200.f, 200.f));
	InteractionCollision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	RootComponent = InteractionCollision;
	InteractionCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractionCollision->SetCollisionObjectType(ECC_WorldDynamic);
	InteractionCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	InteractionCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	InteractionCollision->OnComponentBeginOverlap.AddDynamic(this, &APuzzleBlock::OnPlayerOverlapBegin);
	InteractionCollision->OnComponentEndOverlap.AddDynamic(this, &APuzzleBlock::OnPlayerOverlapEnd);

	InteractionCollision->SetHiddenInGame(false);
	InteractionCollision->SetVisibility(true);
	
	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComp"));
	StaticMeshComp->SetupAttachment(RootComponent);
	
	SlotCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("SlotCollision"));
	SlotCollision->SetupAttachment(RootComponent);

	SlotCollision->SetBoxExtent(FVector(50.0f, 50.0f, 50.0f));

	SlotCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SlotCollision->SetCollisionObjectType(ECC_PhysicsBody);
	SlotCollision->SetCollisionResponseToAllChannels(ECR_Block);
	SlotCollision->SetSimulatePhysics(true);

	SlotCollision->SetHiddenInGame(false);
	SlotCollision->SetVisibility(true);


	BlockID = -1; //starting num
}


void APuzzleBlock::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("InteractionCollision Enabled: %d"), InteractionCollision->GetCollisionEnabled());
	UE_LOG(LogTemp, Warning, TEXT("SlotCollision Enabled: %d"), SlotCollision->GetCollisionEnabled());
}



void APuzzleBlock::OnPlayerOverlapBegin(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->IsA(APlayerCharacter::StaticClass()))
	{
		bPlayerInRange = true;
		UE_LOG(LogTemp, Warning, TEXT("Press F"));

		//Player code
		APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(OtherActor);
		if (PlayerCharacter)
		{
			PlayerCharacter->StartPeek();
		}

	}
}

void APuzzleBlock::OnPlayerOverlapEnd(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	if (OtherActor && OtherActor->IsA(APlayerCharacter::StaticClass()))
	{
		UE_LOG(LogTemp, Warning, TEXT("Player out of Range"));
		bPlayerInRange = false;

		APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(OtherActor);
		if (PlayerCharacter)
		{
			PlayerCharacter->StopPeek();
		}
	}
}


void APuzzleBlock::SetBlockID(int32 NewID)
{
	BlockID = NewID;
	UE_LOG(LogTemp, Warning, TEXT("Block ID Set: %d"), BlockID);

	UStaticMesh* NewMesh = nullptr;

	switch (BlockID)
	{
	case 1:
		NewMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Game/Yujin/Assets/gears/Gear_1"));
		break;
	case 2:
		NewMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Game/Yujin/Assets/gears/Gear_2"));
		break;
	case 3:
		NewMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Game/Yujin/Assets/gears/Gear_3"));
		break;
	case 4:
		NewMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Game/Yujin/Assets/gears/Gear_4"));
		break;
	case 5:
		NewMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Game/Yujin/Assets/gears/Gear_5"));
		break;
	default:
		UE_LOG(LogTemp, Error, TEXT("Invalid BlockID!"));
		break;
	}
	if (NewMesh)
	{
		StaticMeshComp->SetStaticMesh(NewMesh);
		UE_LOG(LogTemp, Error, TEXT("SetBlockID: Successfully loaded mesh for BlockID %d!"), BlockID);
		
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("SetBlockID: Failed to load mesh for BlockID %d!"), BlockID);
	}

}


void APuzzleBlock::ActivateItem(AActor* Activator)
{
	APlayerCharacter* Player = Cast<APlayerCharacter>(Activator);
	//need to be fixed 
	if (Player)
	{
		UE_LOG(LogTemp, Warning, TEXT("PuzzleBlock is placed!"));

		if (CurrentSlot)
		{
			SetActorLocation(CurrentSlot->GetActorLocation());

			SlotCollision->SetSimulatePhysics(false);
			SetActorEnableCollision(false);

			UE_LOG(LogTemp, Warning, TEXT("PuzzleBlock placed in slot!"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("No slot available for PuzzleBlock"));
		}
	}
}

