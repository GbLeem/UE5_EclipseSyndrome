#include "InteractableItem/PuzzleItem/PuzzleManager.h"
#include "Kismet/GameplayStatics.h"


APuzzleManager::APuzzleManager()
{
	static ConstructorHelpers::FClassFinder<APuzzleBlock> BlockBP(TEXT("/Script/Engine.Blueprint'/Game/Yujin/Blueprints/Puzzle/BP_PuzzleBlock.BP_PuzzleBlock'"));
	if (BlockBP.Succeeded())
	{
		BlockClass = BlockBP.Class;
	}
}



void APuzzleManager::BeginPlay()
{
	Super::BeginPlay();
	SetUpSlots();

	AssignBlockIDs();
}

//answer of the puzzle
void APuzzleManager::SetUpSlots()
{
	for (int i = 0; i < Slots.Num(); i++)
	{
		if (!Slots[i])
		{
			UE_LOG(LogTemp, Error, TEXT("Error: Slots[%d] is nullptr!"), i);
			return;
		}
	}

	Slots[0]->CorrectBlockID = 5;
	Slots[1]->CorrectBlockID = 3;
	Slots[2]->CorrectBlockID = 1;
	Slots[3]->CorrectBlockID = 2;
}

//if all block id is correct, door will open
bool APuzzleManager::CheckPuzzleSolved()
{
	for (APuzzleBSlot* Slot : Slots)
	{
		//
		if (!Slot || !Slot->CurrentBlock || Slot->CurrentBlock->BlockID != Slot->CorrectBlockID)
		{
			UE_LOG(LogTemp, Warning, TEXT("...Wrong one"));
			return false;
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("Puzzle Solved! Door Opening"));
	OpenDoor();
	return true;
}




void APuzzleManager::AssignBlockIDs()
{
	TArray<AActor*> FoundBlocks;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APuzzleBlock::StaticClass(), FoundBlocks);
	UE_LOG(LogTemp, Warning, TEXT("Found Blocks: %d"), FoundBlocks.Num());

	
	for (int i = 0; i < FoundBlocks.Num(); i++)
	{
		APuzzleBlock* Block = Cast<APuzzleBlock>(FoundBlocks[i]);
		if (Block)
		{
			Block->SetBlockID(i + 1);
			UE_LOG(LogTemp, Warning, TEXT("Assigned Block ID: %d"), i + 1);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to cast to APuzzleBlock!"));
		}
	}
}


void APuzzleManager::OpenDoor()
{
	if (Door)
	{
		Door->OpenDoor();
		UE_LOG(LogTemp, Warning, TEXT("Door is now opening!!!!!!!!"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Door null!!!!!!!!"));
	}
	

}