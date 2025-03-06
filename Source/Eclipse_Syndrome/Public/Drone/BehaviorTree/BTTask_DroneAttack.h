#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_DroneAttack.generated.h"

class AEnemyBase;
class ADroneAIController;

UCLASS()
class ECLIPSE_SYNDROME_API UBTTask_DroneAttack : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_DroneAttack();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	FTimerHandle AttackCooldownTimer;
	TObjectPtr<AEnemyBase> PrevTarget;
	
	FVector DesiredTarget;
	bool bCanFindPath = true;
	bool bEndFollowPath = true;
	TArray<FVector> PathPoints;
	int CurIndex;
	void UpdateDesiredTarget(const TObjectPtr<AActor>& PlayerPawn, const TObjectPtr<ADroneAIController>& DroneAIController);
	void UpdatePath(const TObjectPtr<ADroneAIController>& DroneAIController);
	void FindPath(const TObjectPtr<ADroneAIController>& DroneAIController);
	void CanFindPath() { bCanFindPath = true; }
	void FollowPath(const TObjectPtr<ADroneAIController>& DroneAIController, float DeltaTime);
	void DrawDebugPath();
};
