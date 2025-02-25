#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "PuzzleBlock.generated.h"

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


	void SetBlockID(int32 NewID);

	int32 GetBlockID() const { return BlockID; }

};
