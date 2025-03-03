#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_RangeCheck.generated.h"

UCLASS()
class ECLIPSE_SYNDROME_API UBTTask_RangeCheck : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_RangeCheck();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
