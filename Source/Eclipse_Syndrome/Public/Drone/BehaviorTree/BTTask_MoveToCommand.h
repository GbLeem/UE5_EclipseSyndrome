#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Drone/DroneAIController.h"
#include "BTTask_MoveToCommand.generated.h"

UCLASS()
class ECLIPSE_SYNDROME_API UBTTask_MoveToCommand : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_MoveToCommand();
	
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	void UpdateDesiredTarget(const UBehaviorTreeComponent& OwnerComp, const TObjectPtr<ADrone>& Drone);
	void UpdatePath(const TObjectPtr<ADroneAIController>& DroneAIController);
	void FindPath(const TObjectPtr<ADroneAIController>& DroneAIController);
	void CanFindPath();
	void FollowPath(const TObjectPtr<ADroneAIController>& DroneAIController, float DeltaTime);
	void DrawDebugPath();

private:
	FVector PrevTargetLocation;
	FVector DesiredTarget;
	bool bCanFindPath;
	bool bEndFollowPath;
	TArray<FVector> PathPoints;
	int CurIndex;
	FTimerHandle PathTimerHandle;
	float NextNodeIgnoreRadius;
	bool CheckOnce = false;
};