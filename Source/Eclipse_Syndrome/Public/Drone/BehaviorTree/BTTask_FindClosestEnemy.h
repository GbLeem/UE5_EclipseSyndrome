#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_FindClosestEnemy.generated.h"

UCLASS()
class ECLIPSE_SYNDROME_API UBTTask_FindClosestEnemy : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_FindClosestEnemy();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
