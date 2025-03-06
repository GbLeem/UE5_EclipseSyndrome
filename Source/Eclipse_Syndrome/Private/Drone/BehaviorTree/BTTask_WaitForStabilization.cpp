#include "Drone/BehaviorTree/BTTask_WaitForStabilization.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Drone/Drone.h"
#include "Drone/DroneAIController.h"
#include "Kismet/GameplayStatics.h"

UBTTask_WaitForStabilization::UBTTask_WaitForStabilization()
	: WaitTime(0.0f)
	, AccumulateTime(0.0f)
	, bLocationStable(false)
{
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_WaitForStabilization::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	AccumulateTime = 0.0f;
	bLocationStable = false;
	//WaitTime = 5.0f;
	WaitTime = OwnerComp.GetBlackboardComponent()->GetValueAsEnum("CurrentState") == 3 ? 5.0f : 1.0f;

	return EBTNodeResult::InProgress;
}

void UBTTask_WaitForStabilization::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	if (OwnerComp.GetBlackboardComponent()->GetValueAsEnum("CurrentState") != 3
		&& OwnerComp.GetBlackboardComponent()->GetValueAsEnum("CurrentState") != 0)
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	const TObjectPtr<AAIController> AIController = OwnerComp.GetAIOwner();
	const TObjectPtr<ADrone> Drone = Cast<ADrone>(AIController->GetPawn());

	if (!Drone || !AIController)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	// 드론이 안정적으로 멈췄는지 확인
	bLocationStable = OwnerComp.GetBlackboardComponent()->GetValueAsEnum("CurrentState") == 3 ? true : IsDroneStable(OwnerComp, DeltaSeconds);

	if (bLocationStable)
	{
		AccumulateTime += DeltaSeconds;

		// 안정적으로 도착하고, 대기 시간이 지나면 완료
		if (AccumulateTime >= WaitTime)
		{
			OwnerComp.GetBlackboardComponent()->SetValueAsEnum("CurrentState", 1);
			Cast<ADroneAIController>(AIController)->EndExecuteCommand();
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		}
	}
	else
	{
		// 아직 안정되지 않았다면 대기 시간 리셋
		AccumulateTime = 0.0f;
	}
}

bool UBTTask_WaitForStabilization::IsDroneStable(const UBehaviorTreeComponent& OwnerComp, float DeltaTime) const
{
	const TObjectPtr<AAIController> AIController = OwnerComp.GetAIOwner();
	const TObjectPtr<ADrone> Drone = Cast<ADrone>(AIController->GetPawn());
	const TObjectPtr<AActor> Player = UGameplayStatics::GetPlayerPawn(Drone->GetWorld(), 0);
	const TObjectPtr<ADroneAIController> DroneAIController = Cast<ADroneAIController>(OwnerComp.GetAIOwner());

	if (!Drone || !Player) return false;

	//const FVector TargetLocation = OwnerComp.GetBlackboardComponent()->GetValueAsVector("TargetLocation");
	constexpr float StableDistanceThreshold = 20.0f;


	const FVector Location = Player->GetActorLocation() + Player->GetActorRotation().RotateVector(DroneAIController->GetBaseDroneOffset());
	float Distance = FVector::Dist(Drone->GetActorLocation(), Location);
	const bool bResult = Distance <= StableDistanceThreshold;

	if (!bResult)
	{
		DroneAIController->SetNewTargetLocation(Location);
		DroneAIController->ApplySmoothMovement(DeltaTime);
		DroneAIController->ApplyPIDControl(DeltaTime);
	}

	return bResult;
}