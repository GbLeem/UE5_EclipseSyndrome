// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/GangsterEnemy.h"
#include "Enemy/GangsterAIController.h"

AGangsterEnemy::AGangsterEnemy()
{
	Name = "GangsterEnemy";
	MaxHealth = 100.0f;
	Health = MaxHealth;
	Damage = 20.0f;
	AttackRange = 700.0f;
	AttackReadyRange = 450.0f;

	// AI
	AIControllerClass = AGangsterAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void AGangsterEnemy::OnDeath()
{

}

float AGangsterEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	// Hit Animation
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		if (HitMontage)
		{
			AnimInstance->Montage_Play(HitMontage);
		}
	}

	return ActualDamage;
}


