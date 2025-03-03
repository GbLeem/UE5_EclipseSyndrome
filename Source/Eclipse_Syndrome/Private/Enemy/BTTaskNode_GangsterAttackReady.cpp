// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/BTTaskNode_GangsterAttackReady.h"
#include "Enemy/GangsterAIController.h"
#include "Enemy/GangsterEnemy.h"
#include "Character/PlayerCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"

EBTNodeResult::Type UBTTaskNode_GangsterAttackReady::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AGangsterAIController* GangsterAIController = Cast<AGangsterAIController>(OwnerComp.GetAIOwner());
	if (!GangsterAIController)
	{
		return EBTNodeResult::Failed;
	}

	AGangsterEnemy* Gangster = Cast<AGangsterEnemy>(GangsterAIController->GetPawn());
	if (!Gangster)
	{
		return EBTNodeResult::Failed;
	}

	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GangsterAIController->GetBlackboardComponent()->GetValueAsObject(TEXT("TargetActor")));
	if (!PlayerCharacter)
	{
		return EBTNodeResult::Failed;
	}

	// Chage gangster speed
	Gangster->StopEnemy();
	// Play attack animation
	if (Gangster && Gangster->GetMesh())
	{
		UAnimInstance* AnimInstance = Gangster->GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			UAnimMontage* AttackMontage = Gangster->AttackMontage;
			if (AttackMontage)
			{
				if (Gangster->Health > 0)
				{
					if (!AnimInstance->Montage_IsPlaying(AttackMontage))
					{
						AnimInstance->Montage_Play(AttackMontage);

						// binding Montage end event 
						FOnMontageBlendingOutStarted BlendingOutDelegate;
						BlendingOutDelegate.BindUObject(this, &UBTTaskNode_GangsterAttackReady::OnAttackMontageEnded, &OwnerComp);
						AnimInstance->Montage_SetEndDelegate(BlendingOutDelegate, AttackMontage);

						return EBTNodeResult::InProgress;
					}
					
				}
			}
		}
	}
	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	return EBTNodeResult::Succeeded;
}

void UBTTaskNode_GangsterAttackReady::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted, UBehaviorTreeComponent* OwnerComp)
{
	if (OwnerComp)
	{
		FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
	}
}
