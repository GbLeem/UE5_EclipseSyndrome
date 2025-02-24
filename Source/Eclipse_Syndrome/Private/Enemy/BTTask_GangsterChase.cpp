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

	// Change gangster speed
	Gangster->ChangeSpeedChase();
	
	return EBTNodeResult::InProgress;
}

void UBTTask_GangsterChase::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	if (!GangsterAIController)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	if (!PlayerCharacter)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	
	bool PlayerDetected = GangsterAIController->GetBlackboardComponent()->GetValueAsBool(TEXT("PlayerDetected"));
	if (PlayerDetected)
	{
		EPathFollowingRequestResult::Type MoveResult = GangsterAIController->MoveToActor(PlayerCharacter, 5.0f, true, true, false, nullptr, true);
		if (MoveResult == EPathFollowingRequestResult::AlreadyAtGoal)
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		}
	}
	else
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
