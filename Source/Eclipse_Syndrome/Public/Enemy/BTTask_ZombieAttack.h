// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_ZombieAttack.generated.h"

/**
 * 
 */
UCLASS()
class ECLIPSE_SYNDROME_API UBTTask_ZombieAttack : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_ZombieAttack();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted, UBehaviorTreeComponent* OwnerComp);
};
