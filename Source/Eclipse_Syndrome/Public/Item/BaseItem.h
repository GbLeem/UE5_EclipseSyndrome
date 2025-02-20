#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item/ItemInterface.h"
#include "BaseItem.generated.h"


class UBoxComponent;

UCLASS()
class ECLIPSE_SYNDROME_API ABaseItem : public AActor, public IItemInterface
{
	GENERATED_BODY()
	
public:	
	ABaseItem();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FName ItemType;

	//for picking up item after pressing key
	bool bCanPickUp = false;

	//Collision Box for line tracing
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item")
	USceneComponent* SceneRootComp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	UStaticMeshComponent* StaticMeshComp;


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

	virtual void CollectItem(AActor* Collector) override;
	virtual void ActivateItem(AActor* Activator) override;
	virtual FName GetItemType() const override;
	
	
	virtual void DestroyItem();
	
	
};
