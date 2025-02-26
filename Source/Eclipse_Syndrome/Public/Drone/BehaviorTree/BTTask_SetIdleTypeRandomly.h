#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_SetIdleTypeRandomly.generated.h"

UCLASS()
class ECLIPSE_SYNDROME_API UBTTask_SetIdleTypeRandomly : public UBTTaskNode
{
	GENERATED_BODY()

private:
	int32 PrevIdleType;
	
public:
	UBTTask_SetIdleTypeRandomly();
	
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};