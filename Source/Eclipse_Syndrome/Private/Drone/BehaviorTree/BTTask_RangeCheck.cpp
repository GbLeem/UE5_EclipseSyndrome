#include "Drone/BehaviorTree/BTTask_RangeCheck.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Drone/Drone.h"
#include "Enemy/EnemyBase.h"

UBTTask_RangeCheck::UBTTask_RangeCheck()
{
}

EBTNodeResult::Type UBTTask_RangeCheck::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return EBTNodeResult::Failed;

	ADrone* Drone = Cast<ADrone>(AIController->GetPawn());
	if (!Drone) return EBTNodeResult::Failed;
	
	AEnemyBase* Target = Cast<AEnemyBase>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TEXT("TargetEnemy")));
	if (!Target) return EBTNodeResult::Failed;

	if (Target->GetHealth() <= 0)
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsInt(TEXT("AttackType"), 0);
		OwnerComp.GetBlackboardComponent()->ClearValue(TEXT("TargetEnemy"));
		return EBTNodeResult::Succeeded;
	}
	
	float Distance = FVector::Dist(Drone->GetActorLocation(), Target->GetActorLocation());
	if (Distance <= Drone->GetDetectionRange())
	{
		if (Distance <= Drone->GetAttackRange())
		{
			OwnerComp.GetBlackboardComponent()->SetValueAsInt(TEXT("AttackType"), 1);
			return EBTNodeResult::Succeeded;
		}
		else
		{
			// Move to Target
			OwnerComp.GetBlackboardComponent()->SetValueAsVector(TEXT("MoveCommandLocation"), Target->GetActorLocation() + FVector(0, 0, 100));
			OwnerComp.GetBlackboardComponent()->SetValueAsInt(TEXT("AttackType"), 3);
			return EBTNodeResult::Succeeded;
		}
	}

	OwnerComp.GetBlackboardComponent()->SetValueAsInt(TEXT("AttackType"), 0);
	OwnerComp.GetBlackboardComponent()->ClearValue(TEXT("TargetEnemy"));
	return EBTNodeResult::Succeeded;
}
