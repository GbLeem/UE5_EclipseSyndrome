// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/BTTask_ZombieAttack.h"
#include "Enemy/ZombieAIController.h"
#include "Character/PlayerCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
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

	// Attack
	PerformAttack(EnemyCharacter, TargetActor);

	FTimerHandle TimerHandle;
	EnemyCharacter->GetWorldTimerManager().SetTimer(TimerHandle, [this, &OwnerComp]()
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		}, 2.0f, false);
	return EBTNodeResult::InProgress;
}

void UBTTask_ZombieAttack::PerformAttack(AActor* EnemyActor, AActor* TargetActor)
{
	if (!EnemyActor || !TargetActor)
	{
		return;
	}

	AZombieEnemy* EnemyCharacter = Cast<AZombieEnemy>(EnemyActor);
	if (!EnemyCharacter)
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Enemy Attacks!"));
	// Play attack animation
	//if (EnemyCharacter && EnemyCharacter->GetMesh())
	//{
	//	UAnimInstance* AnimInstance = EnemyCharacter->GetMesh()->GetAnimInstance();
	//	if (AnimInstance)
	//	{
	//		UAnimMontage* AttackMontage = nullptr; // here set attack animation montage
	//		if (AttackMontage)
	//		{
	//			AnimInstance->Montage_Play(AttackMontage);
	//		}
	//	}
	//}

	// Apply damage
	UGameplayStatics::ApplyDamage(TargetActor, EnemyCharacter->Damage, nullptr, EnemyActor, UDamageType::StaticClass());
	
}