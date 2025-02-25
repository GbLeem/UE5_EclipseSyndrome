#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_CircleAroundPlayer.generated.h"

UCLASS()
class ECLIPSE_SYNDROME_API UBTTask_CircleAroundPlayer : public UBTTaskNode
{
	GENERATED_BODY()

private:
	float AccumulateAngle;

	void IsIdleCircleMovementDone(UBehaviorTreeComponent& OwnerComp);
	
public:
	UBTTask_CircleAroundPlayer();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	void UpdateIdleMovement(const TObjectPtr<AActor>& PlayerPawn, AAIController* AIController, float DeltaTime);
	void CalculateStartAngle(const TObjectPtr<AActor>& PlayerPawn, AAIController* AIController);
};