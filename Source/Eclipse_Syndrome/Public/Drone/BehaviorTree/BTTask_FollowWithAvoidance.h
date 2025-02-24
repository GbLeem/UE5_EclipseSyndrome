#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_FollowWithAvoidance.generated.h"

class ADroneAIController;

UCLASS()
class ECLIPSE_SYNDROME_API UBTTask_FollowWithAvoidance : public UBTTaskNode
{
	GENERATED_BODY()

private:
	FVector DesiredTarget;
	bool bEndFollowPath;
	bool bCanFindPath;
	TArray<FVector> PathPoints;
	int32 CurIndex;
	FTimerHandle PathTimerHandle;
	float NextNodeIgnoreRadius;
	
public:
	UBTTask_FollowWithAvoidance();
	
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	void UpdateDesiredTarget(const TObjectPtr<AActor>& PlayerPawn, const TObjectPtr<ADroneAIController>& DroneAIController);
	void UpdatePath(const TObjectPtr<ADroneAIController>& DroneAIController);
	void FindPath(const TObjectPtr<ADroneAIController>& DroneAIController);
	void FollowPath(const TObjectPtr<ADroneAIController>& DroneAIController, float DeltaTime);
	void CanFindPath();
	void DrawDebugPath();
};
