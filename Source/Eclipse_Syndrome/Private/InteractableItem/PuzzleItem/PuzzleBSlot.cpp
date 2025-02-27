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
	CollisionBox->SetGenerateOverlapEvents(true);

}

void APuzzleBSlot::BeginPlay()
{
	Super::BeginPlay();

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

			
			CurrentBlock->CollisionBox->SetSimulatePhysics(false);
		}
	}
}

void APuzzleBSlot::OnBlockOverlap(
	UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{

	APuzzleBlock* OverlappingBlock = Cast<APuzzleBlock>(OtherActor);
	//if otherActor type == APuzzleBlock, OverlappingBlock / else nullptr
	if (!OverlappingBlock || OverlappingBlock->CurrentSlot && OverlappingBlock->CurrentSlot != this)
	{
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
		CurrentBlock->CollisionBox->SetSimulatePhysics(false);
	}


	CurrentBlock = OverlappingBlock;

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
	/*UE_LOG(LogTemp, Warning, TEXT("BlockID %d placed in SlotID %d"), OverlappingBlock->BlockID, CorrectBlockID);*/
	
}


bool APuzzleBSlot::PlaceBlock(APuzzleBlock* Block)
{
	if (!Block || CurrentBlock) 
		// if no block or block already exists -> return false
	{
		return false;
	}
	FVector SlotLocation = GetActorLocation();
	Block->SetActorLocation(SlotLocation, false, nullptr, ETeleportType::TeleportPhysics);
	/*Block->CollisionBox->SetSimulatePhysics(false);*/


	CurrentBlock = Block;
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
	//if current block exists, remove block when called
	if (CurrentBlock)
	{
		CurrentBlock->CollisionBox->SetSimulatePhysics(true);
		
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