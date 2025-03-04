// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/BTTask_ZombieAttack.h"
#include "Enemy/ZombieAIController.h"
#include "Character/PlayerCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Enemy/ZombieEnemy.h"

UBTTask_ZombieAttack::UBTTask_ZombieAttack()
{
	NodeName = TEXT("Attack Player");
}

EBTNodeResult::Type UBTTask_ZombieAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// Get AI controller
	AZombieAIController* AIController = Cast<AZombieAIController>(OwnerComp.GetAIOwner());
	if (!AIController)
	{
		return EBTNodeResult::Failed;
	}

	// Get enemy character
	AZombieEnemy* EnemyCharacter = Cast<AZombieEnemy>(AIController->GetPawn());
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

	// Play attack animation
	if (EnemyCharacter && EnemyCharacter->GetMesh())
	{
		UAnimInstance* AnimInstance = EnemyCharacter->GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			UAnimMontage* AttackMontage = EnemyCharacter->GetRandomAttackMontage();
			if (AttackMontage)
			{
				if (EnemyCharacter->Health > 0)
				{
					if (!AnimInstance->Montage_IsPlaying(AttackMontage))
					{
						AnimInstance->Montage_Play(AttackMontage);
						// Set Event called when montage end
						FOnMontageBlendingOutStarted BlendingOutDelegate;
						BlendingOutDelegate.BindUObject(this, &UBTTask_ZombieAttack::OnAttackMontageEnded, &OwnerComp);
						AnimInstance->Montage_SetBlendingOutDelegate(BlendingOutDelegate, AttackMontage);

						return EBTNodeResult::InProgress;
					}
				}
			}
		}
	}
	return EBTNodeResult::Succeeded;
}

void UBTTask_ZombieAttack::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted, UBehaviorTreeComponent* OwnerComp)
{
	if (OwnerComp)
	{
		FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
	}
}
