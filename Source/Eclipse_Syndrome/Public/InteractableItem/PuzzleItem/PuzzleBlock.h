#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Item/BaseItem.h"
#include "PuzzleBlock.generated.h"

class APuzzleBSlot;
UCLASS()
class ECLIPSE_SYNDROME_API APuzzleBlock : public ABaseItem
{
	GENERATED_BODY()
	
public:	

	APuzzleBlock();

	virtual void BeginPlay() override;
	

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 BlockID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* SlotCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* InteractionCollision;

	UPROPERTY()
	APuzzleBSlot* CurrentSlot = nullptr;

	bool bPlayerInRange = false;

	UFUNCTION()
	void SetBlockID(int32 NewID);

	int32 GetBlockID() const { return BlockID; }

	UFUNCTION()
	void OnPlayerOverlapBegin(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	void OnPlayerOverlapEnd(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

protected:
	virtual void ActivateItem(AActor* Activator) override;
};
