// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/BTTaskNode_GangsterAttack.h"
#include "Enemy/GangsterAIController.h"
#include "Enemy/GangsterEnemy.h"
#include "Character/PlayerCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"

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
			UAnimMontage* AttackMontage = EnemyCharacter->WalkAttackMontage;
			if (AttackMontage)
			{
				if (EnemyCharacter->Health > 0)
				{
					if (!AnimInstance->Montage_IsPlaying(AttackMontage))
					{
						AnimInstance->Montage_Play(AttackMontage);

						// binding Montage end event 
						FOnMontageBlendingOutStarted BlendingOutDelegate;
						BlendingOutDelegate.BindUObject(this, &UBTTaskNode_GangsterAttack::OnAttackMontageEnded, &OwnerComp);
						AnimInstance->Montage_SetEndDelegate(BlendingOutDelegate, AttackMontage);

						return EBTNodeResult::InProgress;
					}
				}
			}
		}
	}
	
	return EBTNodeResult::Succeeded;
}

void UBTTaskNode_GangsterAttack::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted, UBehaviorTreeComponent* OwnerComp)
{
	if (OwnerComp)
	{
		FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
	}
}
