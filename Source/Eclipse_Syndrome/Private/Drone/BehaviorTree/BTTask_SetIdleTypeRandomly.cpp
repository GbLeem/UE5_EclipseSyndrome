#include "Drone/BehaviorTree/BTTask_SetIdleTypeRandomly.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_SetIdleTypeRandomly::UBTTask_SetIdleTypeRandomly()
{
	NodeName = "SetIdleTypeRandomly";
}

EBTNodeResult::Type UBTTask_SetIdleTypeRandomly::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp) return EBTNodeResult::Failed;

	// 0 또는 1의 랜덤 값 선택
	const int32 RandomIdleType = FMath::RandRange(0, 1);
	BlackboardComp->SetValueAsInt("IdleType", RandomIdleType);

	return EBTNodeResult::Succeeded;
}

