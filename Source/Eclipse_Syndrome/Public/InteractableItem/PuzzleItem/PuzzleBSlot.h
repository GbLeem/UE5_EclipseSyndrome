#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PuzzleBlock.h"
#include "Components/BoxComponent.h"
#include "PuzzleBSlot.generated.h"

class APuzzleManager;
class UBoxComponent;

UCLASS()
class ECLIPSE_SYNDROME_API APuzzleBSlot : public AActor
{
	GENERATED_BODY()
	
public:	
	APuzzleBSlot();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CorrectBlockID; //answer block IDdw a

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	APuzzleBlock* CurrentBlock; // placed block

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* CollisionBox;

	APuzzleManager* PuzzleManager;


	float CurrentLerpTime = 0.0f;
	float LerpDuration = 1.0f;
	FVector TargetLocation;
	FRotator TargetRotation;


	UFUNCTION()
	void OnBlockOverlap(
		UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult
	);

	//UFUNCTION()
	//void OnBlockOverlapEnd(
	//	UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	//	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex
	//);

	bool PlaceBlock(APuzzleBlock* Block);
	
	void RemoveBlock();

	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;

	void SetAllCurrentBlock();
};
