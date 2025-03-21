// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Enemy/GangsterAIController.h"
#include "BTTask_EnemyChase.generated.h"

class APlayerCharacter;
class AGangsterAIController;

UCLASS()
class ECLIPSE_SYNDROME_API UBTTask_EnemyChase : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_EnemyChase();

	APlayerCharacter* PlayerCharacter = nullptr;
	AGangsterAIController* GangsterAIController = nullptr;
	EEnemyStateEnum CurrentState;
	
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
