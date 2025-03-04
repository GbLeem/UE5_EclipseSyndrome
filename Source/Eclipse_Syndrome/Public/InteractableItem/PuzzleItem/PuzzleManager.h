#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PuzzleBlock.h"
#include "PuzzleBSlot.h"
#include "InteractableItem/Door.h"
#include "PuzzleManager.generated.h"



UCLASS()
class ECLIPSE_SYNDROME_API APuzzleManager : public AActor
{
	GENERATED_BODY()
	
public:	
	UPROPERTY(EditAnywhere, Category="Puzzle")
	ADoor* Door;

	APuzzleManager();


	virtual void BeginPlay() override;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite)
	TArray<APuzzleBSlot*> Slots;
	
	bool CheckPuzzleSolved();

	UFUNCTION(BlueprintCallable)
	void SetUpSlots();

	void AssignBlockIDs();
	
	void OpenDoor();

protected:
	UPROPERTY(EditAnywhere, Category = "Puzzle")
	TSubclassOf<APuzzleBlock> BlockClass;
	
	int32 NextBlockID = 1;
};
