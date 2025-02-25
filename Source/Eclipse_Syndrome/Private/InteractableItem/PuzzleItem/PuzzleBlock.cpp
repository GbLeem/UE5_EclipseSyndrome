#include "InteractableItem/PuzzleItem/PuzzleBlock.h"


APuzzleBlock::APuzzleBlock()
{

	PrimaryActorTick.bCanEverTick = false;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	RootComponent = CollisionBox;

	CollisionBox->SetBoxExtent(FVector(50.0f, 50.0f, 50.0f));

	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionBox->SetCollisionObjectType(ECC_PhysicsBody); 
	CollisionBox->SetCollisionResponseToAllChannels(ECR_Block); 
	CollisionBox->SetSimulatePhysics(true);

	BlockID = -1; //starting num
}

void APuzzleBlock::SetBlockID(int32 NewID)
{
	BlockID = NewID;
	UE_LOG(LogTemp, Warning, TEXT("Block ID Set: %d"), BlockID);
}

