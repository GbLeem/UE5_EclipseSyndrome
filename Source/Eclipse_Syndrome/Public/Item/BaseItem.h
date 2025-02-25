#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item/ItemInterface.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "BaseItem.generated.h"


class UBoxComponent;
class UWidgetComponent;

UCLASS()
class ECLIPSE_SYNDROME_API ABaseItem : public AActor, public IItemInterface
{
	GENERATED_BODY()
	
public:	
	ABaseItem();
	virtual void Tick(float DeltaTime) override;

	virtual FName GetItemType() const override;
	void ShowUI();
	void StopUI();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FName ItemType;

	//for picking up item after pressing key
	bool bCanPickUp = false;

	//Collision Box for line tracing
	/*UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item")
	USceneComponent* SceneRootComp;*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	UStaticMeshComponent* StaticMeshComp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	UNiagaraComponent* GlowEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TObjectPtr<UWidgetComponent> ItemHoverUI;

	////currently not working
	////-> will be deleted when this code is completely useless
	////Player Overlap
	//UFUNCTION()
	//virtual void OnPlayerOverlapBegin(
	//	UPrimitiveComponent* OverlappedComponent,
	//	AActor* OtherActor,
	//	UPrimitiveComponent* OtherComp,
	//	int32 OtherBodyIndex,
	//	bool bFromSweep,
	//	const FHitResult& SweepResult
	//);
	////currently not working
	////-> will be deleted when this code is completely useless
	//UFUNCTION()
	//virtual void OnPlayerOverlapEnd(
	//	UPrimitiveComponent* OverlappedComponent,
	//	AActor* OtherActor,
	//	UPrimitiveComponent* OtherComp,
	//	int32 OtherBodyIndex);

	
	virtual void CollectItem(AActor* Collector) override;
	/*virtual void TestCollectItem() override;*/

	virtual void ActivateItem(AActor* Activator) override;
	
	
	virtual void DestroyItem();

public:
	bool bIsPeeking;

	////for test
	//void BeginPlay();
};
