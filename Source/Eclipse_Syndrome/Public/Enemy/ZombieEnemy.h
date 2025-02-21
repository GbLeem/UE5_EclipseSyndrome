// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/EnemyBase.h"
#include "ZombieEnemy.generated.h"

/**
 * 
 */
UCLASS()
class ECLIPSE_SYNDROME_API AZombieEnemy : public AEnemyBase
{
	GENERATED_BODY()

public:
	AZombieEnemy();

	// Interface Function
	void OnDeath() override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
};
