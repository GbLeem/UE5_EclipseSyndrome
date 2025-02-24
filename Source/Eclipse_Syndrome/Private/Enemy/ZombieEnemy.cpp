// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/ZombieEnemy.h"
#include "Enemy/ZombieAIController.h"

AZombieEnemy::AZombieEnemy()
{
	Name = "ZombieEnemy";
	MaxHealth = 100.0f;
	Health = MaxHealth;
	Damage = 10.0f;
	AttackRange = 150.0f;

	// AI
	AIControllerClass = AZombieAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

UAnimMontage* AZombieEnemy::GetRandomAttackMontage()
{
	if (!AttackMontage1 || !AttackMontage2 || !AttackMontage3)
	{
		return nullptr;
	}

	TArray<UAnimMontage*> AttackMontages = { AttackMontage1, AttackMontage2, AttackMontage3 };

	UAnimMontage* SelectedMontage = nullptr;
	int32 RandomIndex = FMath::RandRange(0, AttackMontages.Num() - 1);
	SelectedMontage = AttackMontages[RandomIndex];

	return SelectedMontage;
}

void AZombieEnemy::OnDeath()
{
	// Play Dead Animation
	// Stop Move
	// Destroy
}

float AZombieEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	return ActualDamage;
}
