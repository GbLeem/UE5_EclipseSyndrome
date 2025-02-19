#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ItemInterface.generated.h"

UINTERFACE(MinimalAPI)
class UItemInterface : public UInterface
{
	GENERATED_BODY()
};


class ECLIPSE_SYNDROME_API IItemInterface
{
	GENERATED_BODY()

public:
	virtual void ActivateItem(AActor* Activator) = 0;
	virtual FName GetItemType() const = 0;
};
