#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_HappyMovement.generated.h"

UCLASS()
class ECLIPSE_SYNDROME_API UBTTask_HappyMovement : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_HappyMovement();
	
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	void UpdateHappyMovement(const TObjectPtr<AAIController>& AIController, const TObjectPtr<APawn>& ControlledPawn, const TObjectPtr<APawn>& PlayerPawn, float DeltaSeconds);
	void EndTask(UBehaviorTreeComponent& OwnerComp);
	
	float CurrentTime;
	float PrevDroneAIDesiredDistance;
};
