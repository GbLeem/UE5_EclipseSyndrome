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

void AZombieEnemy::OnDeath()
{

}

float AZombieEnemy::TakeDamage()
{
	return 0.0f;
}

void AZombieEnemy::Attack()
{
}
