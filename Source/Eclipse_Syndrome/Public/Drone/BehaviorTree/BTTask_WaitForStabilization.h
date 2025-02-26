#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_WaitForStabilization.generated.h"

UCLASS()
class ECLIPSE_SYNDROME_API UBTTask_WaitForStabilization : public UBTTaskNode
{
	GENERATED_BODY()

private:
	float	WaitTime;
	float	AccumulateTime;
	bool	bLocationStable;

	bool IsDroneStable(const UBehaviorTreeComponent& OwnerComp, float DeltaTime) const;
	
public:
	UBTTask_WaitForStabilization();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
