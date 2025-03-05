#include "InteractableItem/PuzzleItem/PuzzleBSlot.h"
#include "InteractableItem/PuzzleItem/PuzzleManager.h"
#include "Kismet/GameplayStatics.h"

APuzzleBSlot::APuzzleBSlot()
{
	PrimaryActorTick.bCanEverTick = true;
	CurrentBlock = nullptr;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetBoxExtent(FVector(50.0f, 50.0f, 30.0f));
	CollisionBox->SetCollisionProfileName(TEXT("Trigger"));
	RootComponent = CollisionBox;
	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &APuzzleBSlot::OnBlockOverlap);
	//CollisionBox->OnComponentEndOverlap.AddDynamic(this, &APuzzleBSlot::OnBlockOverlapEnd);
	

	///////
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionBox->SetCollisionObjectType(ECC_WorldDynamic);

	CollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Overlap);
	CollisionBox->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	CollisionBox->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);
	//////

	CollisionBox->SetGenerateOverlapEvents(true);
	
}

void APuzzleBSlot::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("PuzzleBSlot CollisionBox Overlap Events: %d"), CollisionBox->OnComponentBeginOverlap.IsBound());
	if (!PuzzleManager)
	{
		PuzzleManager = Cast<APuzzleManager>(UGameplayStatics::GetActorOfClass(GetWorld(), APuzzleManager::StaticClass()));
		if (PuzzleManager)
		{
			UE_LOG(LogTemp, Warning, TEXT("PuzzleManager found in Slot!"))
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to find PuzzleManager"))
		}
	}
	
}

void APuzzleBSlot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurrentLerpTime < LerpDuration)
	{
		CurrentLerpTime += DeltaTime;

		float Alpha = FMath::Clamp(CurrentLerpTime / LerpDuration, 0.0f, 1.0f);

		if (CurrentBlock)
		{
			FVector NewLocation = FMath::Lerp(CurrentBlock->GetActorLocation(), TargetLocation, Alpha);
			FRotator NewRotation = FMath::Lerp(CurrentBlock->GetActorRotation(), TargetRotation, Alpha);

			CurrentBlock->SetActorLocationAndRotation(NewLocation, NewRotation);
		}
		

		//necessary?
		if (Alpha >= 1.0f && CurrentBlock)
		{

			CurrentBlock->SlotCollision->SetSimulatePhysics(false);
			UE_LOG(LogTemp, Warning, TEXT("slot - SetSimulatePhysics false"));
		}
	}
}

void APuzzleBSlot::OnBlockOverlap(
	UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("OnBlockOverlap called! Overlapping actor: %s"), *OtherActor->GetName());

	APuzzleBlock* OverlappingBlock = Cast<APuzzleBlock>(OtherActor);
	//if otherActor type == APuzzleBlock, OverlappingBlock / else nullptr
	if (!OverlappingBlock) return;
	UE_LOG(LogTemp, Warning, TEXT("[!]Overlapping Block Set: %s"), *OverlappingBlock->GetName());
	//if current block exists, and overlapping block is not current block,
	//push back the overlapping block
	if (CurrentBlock && OverlappingBlock != CurrentBlock)
	{
		FVector PushBackLocation = OverlappingBlock->GetActorLocation() + FVector(20.0f, 0.0f, 0.0f);
		OverlappingBlock->SetActorLocation(PushBackLocation);
		UE_LOG(LogTemp, Warning, TEXT("Slot already occupied! %s is pushed back."), *OverlappingBlock->GetName());
		return;
	}
	//if overlappingBlock has current slot, but current slot is not this 
	UE_LOG(LogTemp, Warning, TEXT("Block Overlapped: %s"), *OverlappingBlock->GetName());
	/*FVector SnapOffset = FVector(0.0f, 30.0f, 5.0f);*/
	TargetLocation = GetActorLocation();
	TargetRotation = GetActorRotation();
		
	CurrentLerpTime = 0.0f;
	LerpDuration = 1.0f;

	if (CurrentBlock)
	{
		CurrentBlock->SlotCollision->SetSimulatePhysics(false);
	}


	CurrentBlock = OverlappingBlock;
	CurrentBlock->CurrentSlot = this;

	SetAllCurrentBlock();
	///original code

	/*CurrentBlock = OverlappingBlock;*/
	if (CurrentBlock->BlockID == CorrectBlockID)
	{
		UE_LOG(LogTemp, Warning, TEXT("Gear was placed correctly, slot: %d, block:%d placed"), CorrectBlockID, OverlappingBlock->BlockID);
	
	
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Gear was placed wrongly, slot: %d, block:%d placed"), CorrectBlockID, OverlappingBlock->BlockID);
	}
	
}


//void APuzzleBSlot::OnBlockOverlapEnd(
//	UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
//	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex
//)
//{
//	APuzzleBlock* OverlappingBlock = Cast<APuzzleBlock>(OtherActor);
//	if (!OverlappingBlock) return;
//	UE_LOG(LogTemp, Warning, TEXT("Block %s exited slot %d"), *OverlappingBlock->GetName(), CorrectBlockID);
//	if (CurrentBlock == OverlappingBlock)
//	{
//		RemoveBlock();
//	}
//}


bool APuzzleBSlot::PlaceBlock(APuzzleBlock* Block)
{
	if (!Block) return false;
		// if no block or block already exists -> return false
	if(CurrentBlock)
	{

		FVector PushBackLocation = CurrentBlock->GetActorLocation() + FVector(20.0f, 0.0f, 0.0f);
		CurrentBlock->SetActorLocation(PushBackLocation);
		UE_LOG(LogTemp, Warning, TEXT("Slot already occupied"));
		
	}

	
	FVector SlotLocation = GetActorLocation();
	Block->SetActorLocation(SlotLocation, false, nullptr, ETeleportType::TeleportPhysics);


	CurrentBlock = Block;
	CurrentBlock->CurrentSlot = this;

	if (CurrentBlock->BlockID == CorrectBlockID)
	{
		UE_LOG(LogTemp, Warning, TEXT("Gear was placed correctly, slot: %d, block:%d placed"), CorrectBlockID, Block->BlockID);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Gear was placed wrongly, slot: %d, block:%d placed"), CorrectBlockID, Block->BlockID);
	}
	
	
	return (Block->BlockID == CorrectBlockID);
}

void APuzzleBSlot::RemoveBlock()
{
	UE_LOG(LogTemp, Warning, TEXT("RemoveBlock called on Slot ID %d"), CorrectBlockID);
	//if current block exists, remove block when called
	if (CurrentBlock)
	{
		CurrentBlock->SlotCollision->SetSimulatePhysics(false);
		
		UE_LOG(LogTemp, Warning, TEXT("Block ID %d removed from Slot ID %d"), CurrentBlock->BlockID, CorrectBlockID);
		CurrentBlock = nullptr;
	}
}


//check if puzzle is solved when the block is attached to the slot
void APuzzleBSlot::SetAllCurrentBlock()
{

	if (PuzzleManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("Calling CheckPuzzleSolved"));
		PuzzleManager->CheckPuzzleSolved();
	}
	else 
	{
		UE_LOG(LogTemp, Warning, TEXT("puzzleManager is nullptr!!!"));
	}
}