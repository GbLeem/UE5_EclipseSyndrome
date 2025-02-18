#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item/ItemInterface.h"
#include "BaseItem.generated.h"

UCLASS()
class ECLIPSE_SYNDROME_API ABaseItem : public AActor, public IItemInterface
{
	GENERATED_BODY()
	
public:	
	ABaseItem();

protected:
	virtual void ActivateItem(AActor* Activator) override;
	virtual FName GetItemType() const override;
	
	
	virtual void DestroyItem();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FName ItemType;
};
