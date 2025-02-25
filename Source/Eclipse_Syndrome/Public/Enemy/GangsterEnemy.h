// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/EnemyBase.h"
#include "GangsterEnemy.generated.h"

/**
 * 
 */
UCLASS()
class ECLIPSE_SYNDROME_API AGangsterEnemy : public AEnemyBase
{
	GENERATED_BODY()
public:
	AGangsterEnemy();

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* HitMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
	float AttackReadyRange;
	
	// Interface Function
	void OnDeath() override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
};
