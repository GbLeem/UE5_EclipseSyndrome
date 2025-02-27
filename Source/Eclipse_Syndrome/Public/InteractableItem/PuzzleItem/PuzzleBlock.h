#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "PuzzleBlock.generated.h"

class APuzzleBSlot;
UCLASS()
class ECLIPSE_SYNDROME_API APuzzleBlock : public AActor
{
	GENERATED_BODY()
	
public:	

	APuzzleBlock();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 BlockID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* CollisionBox;

	UPROPERTY()
	APuzzleBSlot* CurrentSlot = nullptr;

	void SetBlockID(int32 NewID);

	int32 GetBlockID() const { return BlockID; }

};
