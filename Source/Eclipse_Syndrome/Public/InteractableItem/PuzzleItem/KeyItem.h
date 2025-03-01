#pragma once

#include "CoreMinimal.h"
#include "Item/BaseItem.h"
#include "InteractableItem/GarageDoor.h"
#include "Components/BoxComponent.h"
#include "KeyItem.generated.h"



UCLASS()
class ECLIPSE_SYNDROME_API AKeyItem : public ABaseItem
{
	GENERATED_BODY()
	

public:
	AKeyItem();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Key")
	FName DoorTag;


	UBoxComponent* InteractionBox;

	AGarageDoor* TargetGarageDoor;

	bool bPlayerInRange = false;


	virtual void ActivateItem(AActor* Activator) override;

	virtual void BeginPlay() override;

	void TestActivateItem();


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

};
