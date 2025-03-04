// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTaskNode_GangsterAttack.generated.h"

/**
 * 
 */
UCLASS()
class ECLIPSE_SYNDROME_API UBTTaskNode_GangsterAttack : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTaskNode_GangsterAttack();

	UBehaviorTreeComponent* CachedOwnerComp; // BT Component

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);
};
