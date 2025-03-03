// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/GangsterAttackAnimNotify.h"
#include "Enemy/GangsterEnemy.h"
#include "Enemy/GangsterAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/PlayerCharacter.h"

void UGangsterAttackAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	//Super::Notify(MeshComp, Animation);
	if (!MeshComp) return;

	// Get animation owner GangsterEnemy
	AGangsterEnemy* Gangster = Cast<AGangsterEnemy>(MeshComp->GetOwner());
	if (!Gangster) return;

	// Get GangsterAIController
	AGangsterAIController* AIController = Cast<AGangsterAIController>(Gangster->GetController());
	if (!AIController) return;

	// Get EnemyState from Blackboard
	UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComponent();
	if (!BlackboardComp) return;

	EEnemyStateEnum CurrentEnemyState = static_cast<EEnemyStateEnum>(BlackboardComp->GetValueAsEnum(TEXT("EnemyState")));
	AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(TEXT("TargetActor")));

	if (CurrentEnemyState != EEnemyStateEnum::Chasing)
	{
		Gangster->Attack(TargetActor);
	}

}
