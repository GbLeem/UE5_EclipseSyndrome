#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PuzzleBlock.h"
#include "Components/BoxComponent.h"
#include "PuzzleBSlot.generated.h"

class UBoxComponent;

UCLASS()
class ECLIPSE_SYNDROME_API APuzzleBSlot : public AActor
{
	GENERATED_BODY()
	
public:	
	APuzzleBSlot();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CorrectBlockID; //answer block ID

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	APuzzleBlock* CurrentBlock; // placed block

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* CollisionBox;

	UFUNCTION()
	void OnBlockOverlap(
		UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult
	);



	bool PlaceBlock(APuzzleBlock* Block);
	void RemoveBlock();


};
