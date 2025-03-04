// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTaskNode_GangsterAttackReady.generated.h"

class APlayerCharacter;
class AGangsterAIController;

UCLASS()
class ECLIPSE_SYNDROME_API UBTTaskNode_GangsterAttackReady : public UBTTaskNode
{
	GENERATED_BODY()
	
public:

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted, UBehaviorTreeComponent* OwnerComp);
};
