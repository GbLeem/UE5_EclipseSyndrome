#include "Drone/BehaviorTree/BTTask_SetIdleTypeRandomly.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Drone/DroneAIController.h"
#include "Kismet/GameplayStatics.h"

UBTTask_SetIdleTypeRandomly::UBTTask_SetIdleTypeRandomly()
	: PrevIdleType(0)
{
	NodeName = "SetIdleTypeRandomly";
}

EBTNodeResult::Type UBTTask_SetIdleTypeRandomly::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp) return EBTNodeResult::Failed;
	
	if (PrevIdleType == 1)
	{
		BlackboardComp->SetValueAsInt("IdleType", 0);
		PrevIdleType = 0;
	}
	else
	{
		const int32 RandomIdleType = FMath::RandRange(0, 1);
		PrevIdleType = RandomIdleType;
		BlackboardComp->SetValueAsInt("IdleType", RandomIdleType);

		if (RandomIdleType == 1)
		{
			const TObjectPtr<APawn> Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
			const TObjectPtr<ADroneAIController> DroneAIController = Cast<ADroneAIController>(OwnerComp.GetAIOwner());
			FVector Location = Player->GetActorLocation() + Player->GetActorRotation().RotateVector(DroneAIController->GetBaseDroneOffset());
			BlackboardComp->SetValueAsVector("TargetLocation", Location);
		}
	}
	
	return EBTNodeResult::Succeeded;
}