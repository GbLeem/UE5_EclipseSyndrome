#include "InteractableItem/PuzzleItem/PuzzleBlock.h"
#include "InteractableItem/PuzzleItem/PuzzleBSlot.h"
#include "Character/PlayerCharacter.h"

APuzzleBlock::APuzzleBlock()
{
	if (!RootComponent)
	{
		RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	}
	PrimaryActorTick.bCanEverTick = false;
	
	ItemType = "PuzzleBlock";
	ItemNumber = 3;
	ItemAmount = 1;
	bIsPuzzleBlock = true;

	SlotCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("SlotCollision"));
	SlotCollision->SetupAttachment(RootComponent);


	SlotCollision->SetBoxExtent(FVector(25.0f, 25.0f, 25.0f));
	SlotCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SlotCollision->SetCollisionObjectType(ECC_WorldDynamic);
	SlotCollision->SetRelativeLocation(FVector(0.f, 0.f, 0.f));

	SlotCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	SlotCollision->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);
	//SlotCollision->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Ignore);
	////
	SlotCollision->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	SlotCollision->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Overlap);
	////
	SlotCollision->SetSimulatePhysics(false);
	SlotCollision->SetHiddenInGame(true);
	SlotCollision->SetVisibility(false);

	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComp"));
	StaticMeshComp->SetupAttachment(SlotCollision);
	StaticMeshComp->SetRelativeLocation(FVector(0.f, 0.f, 0.f));
	StaticMeshComp->SetSimulatePhysics(false);
	//StaticMeshComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	//StaticMeshComp->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Ignore);
	//StaticMeshComp->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Ignore);



	InteractionCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionCollision"));
	InteractionCollision->SetBoxExtent(FVector(150.f, 150.f, 150.f));
	InteractionCollision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	InteractionCollision->SetupAttachment(RootComponent);
	InteractionCollision->SetRelativeLocation(FVector(0.f, 0.f, 0.f));


	InteractionCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractionCollision->SetCollisionObjectType(ECC_WorldDynamic);
	InteractionCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	InteractionCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	InteractionCollision->OnComponentBeginOverlap.AddDynamic(this, &APuzzleBlock::OnPlayerOverlapBegin);
	InteractionCollision->OnComponentEndOverlap.AddDynamic(this, &APuzzleBlock::OnPlayerOverlapEnd);

	InteractionCollision->SetHiddenInGame(true);
	InteractionCollision->SetVisibility(false);
	
	BlockID = -1; //starting num
}


void APuzzleBlock::BeginPlay()
{
	Super::BeginPlay();
	//UE_LOG(LogTemp, Warning, TEXT("InteractionCollision Enabled: %d"), InteractionCollision->GetCollisionEnabled());
	UE_LOG(LogTemp, Warning, TEXT("SlotCollision Enabled: %d"), SlotCollision->GetCollisionEnabled());

	UE_LOG(LogTemp, Warning, TEXT("%s SlotCollision: SimulatePhysics=%d, CollisionEnabled=%d"),
		*GetName(), SlotCollision->IsSimulatingPhysics(), SlotCollision->GetCollisionEnabled());
	//if (SlotCollision)
	//{
	//	SlotCollision->SetSimulatePhysics(true);
	//	UE_LOG(LogTemp, Warning, TEXT("[!] PuzzleBlock Physics Enabled on Spawn"));
	//}
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
	UE_LOG(LogTemp, Warning, TEXT("SlotCollision Location: %s"), *SlotCollision->GetComponentLocation().ToString());
	UE_LOG(LogTemp, Warning, TEXT("StaticMeshComp Location Before: %s"), *StaticMeshComp->GetComponentLocation().ToString());

	if (Player)
	{
		UE_LOG(LogTemp, Warning, TEXT("PuzzleBlock is placed!"));

		if (CurrentSlot)
		{
			
			/*FVector TargetLocation = CurrentSlot->GetActorLocation();
			SetActorLocation(TargetLocation);

			SetActorEnableCollision(false);

			FVector SlotCenter = SlotCollision->GetComponentLocation();
			StaticMeshComp->SetWorldLocation(SlotCenter);

			UE_LOG(LogTemp, Warning, TEXT("PuzzleBlock moved to Slot! TargetLocation: %s, SlotCenter: %s"),
				*TargetLocation.ToString(), *SlotCenter.ToString());*/

			////
			SetActorLocation(CurrentSlot->GetActorLocation());
			UE_LOG(LogTemp, Warning, TEXT("StaticMeshComp Location After: %s"), *StaticMeshComp->GetComponentLocation().ToString());

			SlotCollision->SetSimulatePhysics(false);
			UE_LOG(LogTemp, Warning, TEXT("SetSimulatePhysics(false) called for puzzleblock"));
			

			UE_LOG(LogTemp, Warning, TEXT("PuzzleBlock placed in slot!"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("No slot available for PuzzleBlock"));
		}
	}
}