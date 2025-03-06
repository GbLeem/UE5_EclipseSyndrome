// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/EnemyBase.h"
#include "GangsterEnemy.generated.h"


class USphereComponent;
class UNiagaraComponent;
class ADefaultBullet;

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
	float ChasingRange;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
	float AdvancingRange;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
	float ShootingRange;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MoveSpeed")
	float AimSpeed;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Weapon")
	USkeletalMeshComponent* GunMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	UNiagaraComponent* MuzzleFlashComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	USphereComponent* SphereComp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drone|Property")
	TSubclassOf<ADefaultBullet> BulletClass;

	UFUNCTION(BlueprintCallable)
	void ChangeSpeedAim();
	void CallNearbyGangster();

	virtual void BeginPlay() override;
	
	// Interface Function
	void Attack(AActor* TargetActor) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	FVector CalculateDestination();
};
