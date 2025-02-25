#include "InteractableItem/PuzzleItem/PuzzleBSlot.h"

APuzzleBSlot::APuzzleBSlot()
{
	PrimaryActorTick.bCanEverTick = false;
	CurrentBlock = nullptr;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetBoxExtent(FVector(50.0f, 50.0f, 50.0f));
	CollisionBox->SetCollisionProfileName(TEXT("Trigger"));
	RootComponent = CollisionBox;
	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &APuzzleBSlot::OnBlockOverlap);
	CollisionBox->SetGenerateOverlapEvents(true);

}

void APuzzleBSlot::OnBlockOverlap(
	UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{

	APuzzleBlock* OverlappingBlock = Cast<APuzzleBlock>(OtherActor);
	//if otherActor type == APuzzleBlock, OverlappingBlock / else nullptr
	if (OverlappingBlock)
	{
		OverlappingBlock->SetActorLocation(GetActorLocation());

		OverlappingBlock->CollisionBox->SetSimulatePhysics(false);

		CurrentBlock = OverlappingBlock;

		UE_LOG(LogTemp, Warning, TEXT("BlockID %d placed in SlotID %d"), OverlappingBlock->BlockID, CorrectBlockID);

	}
	
}


bool APuzzleBSlot::PlaceBlock(APuzzleBlock* Block)
{
	if (!Block || CurrentBlock) 
		// if no block or block already exists -> return false
	{
		return false;
	}

	CurrentBlock = Block;
	UE_LOG(LogTemp, Warning, TEXT("slot: %d, block:%d placed"), CorrectBlockID, Block->BlockID);
	
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

