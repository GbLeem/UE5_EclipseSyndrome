// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/BTTask_EnemyChase.h"
#include "Enemy/GangsterAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Enemy/GangsterEnemy.h"
#include "Character/PlayerCharacter.h"
#include "Navigation/PathFollowingComponent.h"

UBTTask_EnemyChase::UBTTask_EnemyChase()
{
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_EnemyChase::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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

void UBTTask_EnemyChase::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	// GangsterAIController null check
	if (!GangsterAIController)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	// PlayerCharacter null check
	if (!PlayerCharacter)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	// Get PlayerDetected from Blackboard
	bool PlayerDetected = GangsterAIController->GetBlackboardComponent()->GetValueAsBool(TEXT("PlayerDetected"));

	if (PlayerDetected)
	{
		// Move To Player
		EPathFollowingRequestResult::Type MoveResult = GangsterAIController->MoveToActor(PlayerCharacter, 5.0f, true, true, false, nullptr, true);

		// When Enemy at Goal
		if (MoveResult == EPathFollowingRequestResult::AlreadyAtGoal)
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		}

		// When EneyState change
		if (static_cast<uint8>(CurrentState) != GangsterAIController->GetBlackboardComponent()->GetValueAsEnum(TEXT("EnemyState")))
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		}
	}
	else
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
