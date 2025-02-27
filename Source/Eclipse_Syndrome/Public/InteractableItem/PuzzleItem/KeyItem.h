#pragma once

#include "CoreMinimal.h"
#include "Item/BaseItem.h"
#include "InteractableItem/GarageDoor.h"
#include "KeyItem.generated.h"


UCLASS()
class ECLIPSE_SYNDROME_API AKeyItem : public ABaseItem
{
	GENERATED_BODY()
	

public:
	AKeyItem();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Key")
	FName DoorTag;

	AGarageDoor* TargetGarageDoor;

	virtual void ActivateItem(AActor* Activator) override;

	virtual void BeginPlay() override;

	void TestActivateItem();
};
