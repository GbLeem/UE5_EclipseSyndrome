#include "Drone/BehaviorTree/BTTask_FindClosestEnemy.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Drone/Drone.h"
#include "Drone/DroneAIController.h"
#include "Enemy/EnemyBase.h"

UBTTask_FindClosestEnemy::UBTTask_FindClosestEnemy()
{
}

EBTNodeResult::Type UBTTask_FindClosestEnemy::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return EBTNodeResult::Failed;

	ADrone* Drone = Cast<ADrone>(AIController->GetPawn());
	if (!Drone) return EBTNodeResult::Failed;

	float ClosestDistance = Drone->GetDetectionRange();
	AEnemyBase* ClosestEnemy = nullptr;

	for (AEnemyBase* Enemy : Drone->GetNearbyEnemies())
	{
		if (IsValid(Enemy))
		{
			if (Enemy->GetHealth() <= 0)
			{
				continue;
			}
			float Distance = FVector::Dist(Drone->GetActorLocation(), Enemy->GetActorLocation());
			if (Distance < ClosestDistance)
			{
				ClosestDistance = Distance;
				ClosestEnemy = Enemy;
			}
		}
	}

	if (ClosestEnemy)
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsObject(TEXT("TargetEnemy"), ClosestEnemy);
		OwnerComp.GetBlackboardComponent()->SetValueAsInt(TEXT("AttackType"), 1);
		return EBTNodeResult::Succeeded;
	}

	OwnerComp.GetBlackboardComponent()->ClearValue(TEXT("TargetEnemy"));
	OwnerComp.GetBlackboardComponent()->SetValueAsEnum(TEXT("CurrentState"), 1);
	Cast<ADroneAIController>(AIController)->EndExecuteCommand();
	return EBTNodeResult::Failed;
}
