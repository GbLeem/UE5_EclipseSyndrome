// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/BTTaskNode_GangsterAttack.h"
#include "Enemy/GangsterAIController.h"
#include "Enemy/GangsterEnemy.h"
#include "Character/PlayerCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTaskNode_GangsterAttack::UBTTaskNode_GangsterAttack()
{
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTaskNode_GangsterAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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
	EnemyCharacter->ChangeSpeedAim();

	// Play attack animation
	if (EnemyCharacter && EnemyCharacter->GetMesh())
	{
		UAnimInstance* AnimInstance = EnemyCharacter->GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			UAnimMontage* AttackMontage = EnemyCharacter->AttackMontage;
			if (AttackMontage)
			{
				if (EnemyCharacter->Health > 0)
				{
					AnimInstance->Montage_Play(AttackMontage);

					// Save OwnerComp (to use OnAttackMontageEnded)
					CachedOwnerComp = &OwnerComp;

					// binding Montage end event 
					FOnMontageEnded MontageEndDelegate;
					MontageEndDelegate.BindUObject(this, &UBTTaskNode_GangsterAttack::OnAttackMontageEnded);
					AnimInstance->Montage_SetEndDelegate(MontageEndDelegate, AttackMontage);

					return EBTNodeResult::InProgress;
				}
			}
		}
	}
	return EBTNodeResult::Succeeded;
}

void UBTTaskNode_GangsterAttack::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (CachedOwnerComp)
	{
		FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Succeeded);
		CachedOwnerComp = nullptr; // 다음 실행을 위해 초기화
	}
}
