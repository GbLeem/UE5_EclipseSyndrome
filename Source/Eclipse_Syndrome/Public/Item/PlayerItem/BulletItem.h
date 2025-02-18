#pragma once

#include "CoreMinimal.h"
#include "Item/BaseItem.h"
#include "BulletItem.generated.h"


UCLASS()
class ECLIPSE_SYNDROME_API ABulletItem : public ABaseItem
{
	GENERATED_BODY()

public:
	ABulletItem();

	//ÃÑ¾ËÀÇ °³¼ö
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	float NumOfBullet;

	virtual void ActivateItem(AActor* Activator) override;
	
};
