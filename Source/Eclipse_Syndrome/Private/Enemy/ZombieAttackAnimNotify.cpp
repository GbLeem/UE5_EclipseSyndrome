// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/ZombieAttackAnimNotify.h"
#include "Enemy/ZombieEnemy.h"
#include "Enemy/ZombieAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

void UZombieAttackAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!MeshComp) return;

	// Get animation owner zombieEnemy
	AZombieEnemy* Zombie = Cast<AZombieEnemy>(MeshComp->GetOwner());
	if (!Zombie) return;

	// Get zombieAIController
	AZombieAIController* AIController = Cast<AZombieAIController>(Zombie->GetController());
	if (!AIController) return;

	// Get distacne between player and enemy
	UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComponent();
	if (!BlackboardComp) return;

	float Distance = BlackboardComp->GetValueAsFloat(TEXT("PlayerDistance"));
	AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(TEXT("TargetActor")));

	// Attack player
	if (Distance <= Zombie->AttackRange)
	{
		Zombie->Attack(TargetActor);
		UE_LOG(LogTemp, Warning, TEXT("Enemy Attacks!!!!!!!!!!!!"));
	}
}
