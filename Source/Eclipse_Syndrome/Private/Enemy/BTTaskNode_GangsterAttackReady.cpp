// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/BTTaskNode_GangsterAttackReady.h"
#include "Enemy/GangsterAIController.h"
#include "Enemy/GangsterEnemy.h"
#include "Character/PlayerCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"

EBTNodeResult::Type UBTTaskNode_GangsterAttackReady::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	GangsterAIController = Cast<AGangsterAIController>(OwnerComp.GetAIOwner());
	if (!GangsterAIController)
	{
		return EBTNodeResult::Failed;
	}

	AGangsterEnemy* Gangster = Cast<AGangsterEnemy>(GangsterAIController->GetPawn());
	if (!Gangster)
	{
		return EBTNodeResult::Failed;
	}

	PlayerCharacter = Cast<APlayerCharacter>(GangsterAIController->GetBlackboardComponent()->GetValueAsObject(TEXT("TargetActor")));
	if (!PlayerCharacter)
	{
		return EBTNodeResult::Failed;
	}

	// Chage gangster speed
	Gangster->StopEnemy();
	UE_LOG(LogTemp, Warning, TEXT("Stop Enemy!!!!!!!!!!!!!!!!"));

	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	return EBTNodeResult::Succeeded;
}
