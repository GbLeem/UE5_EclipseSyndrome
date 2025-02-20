#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Chest.generated.h"


class UBoxComponent;



UCLASS()
class ECLIPSE_SYNDROME_API AChest : public AActor
{
	GENERATED_BODY()
	
public:	
	AChest();

	UFUNCTION()
	void OpenChest();
	UFUNCTION()
	void CloseChest();

	//if F key, pressed
	void interact();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chest")
	UBoxComponent* InteractionBox;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chest")
	USkeletalMeshComponent* SkeletalMeshComp;

	UPROPERTY(EditAnywhere, Category = "Animation")
	UAnimationAsset* OpenAnimation;


	//check if player is in range
	bool bPlayerInRange = false;
	
	//Player Overlap
	UFUNCTION()
	virtual void OnPlayerOverlapBegin(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UFUNCTION()
	virtual void OnPlayerOverlapEnd(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);
	

	UFUNCTION()
	virtual void InteractWithChest();


	//Store Random Spawnable Items
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chest")
	TArray<TSubclassOf<AActor>> PossibleItems;


	//Spawn random item(Bullet / HealItem)
	void SpawnRandomItem();



};
