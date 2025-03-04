// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Enemy/GangsterAIController.h"
#include "BTTask_GangsterChase.generated.h"

class APlayerCharacter;
class AGangsterAIController;
/**
 * 
 */
UCLASS()
class ECLIPSE_SYNDROME_API UBTTask_GangsterChase : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_GangsterChase();

	UBehaviorTreeComponent* CachedOwnerComp; // BT Component

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
