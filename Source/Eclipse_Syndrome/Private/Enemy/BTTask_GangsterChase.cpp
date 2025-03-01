// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/BTTask_GangsterChase.h"
#include "Enemy/GangsterAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Enemy/GangsterEnemy.h"
#include "Character/PlayerCharacter.h"
#include "Navigation/PathFollowingComponent.h"

UBTTask_GangsterChase::UBTTask_GangsterChase()
{
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_GangsterChase::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// Get AI controller
	AGangsterAIController* AIController = Cast<AGangsterAIController>(OwnerComp.GetAIOwner());
	if (!AIController)
	{
		return EBTNodeResult::Failed;
	}

	// Get enemy character
	AGangsterEnemy* EnemyCharacter = Cast<AGangsterEnemy>(AIController->GetPawn());
	if (!EnemyCharacter)
	{
		return EBTNodeResult::Failed;
	}

	// Get player character from blackboard
	APlayerCharacter* TargetActor = Cast<APlayerCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject("TargetActor"));
	if (!TargetActor)
	{
		return EBTNodeResult::Failed;
	}

	// Change Speed
	EnemyCharacter->ChangeSpeedChase();
	return EBTNodeResult::Succeeded;
}
