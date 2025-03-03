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
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* AttackMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* WalkAttackMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
	float AttackReadyRange;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
	float ShootRange;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MoveSpeed")
	float AimSpeed;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Weapon")
	USkeletalMeshComponent* GunMesh;

	UFUNCTION(BlueprintCallable)
	void ChangeSpeedAim();

	virtual void BeginPlay() override;
	
	// Interface Function
	void Attack(AActor* TargetActor) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	FVector CalculateDestination();
};
