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

	virtual void CollectItem(AActor* Collector) override;

	virtual void ActivateItem(AActor* Activator) override;


	virtual void DestroyItem();


	int32 GetItemNumber() { return ItemNumber; }
	int32 GetItemAmount() { return ItemAmount; }

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
	
	

public:
	bool bIsPeeking;
	UPROPERTY(EditAnywhere, Category = "Item Settings")
	int32 ItemNumber; //for ui [TEST - 2/25]
	int32 ItemAmount;

	bool bIsPuzzleBlock;
};
