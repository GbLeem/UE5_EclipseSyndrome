// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyInterface.h"
#include "EnemyBase.generated.h"

class APatrolPath;

UCLASS()
class ECLIPSE_SYNDROME_API AEnemyBase : public ACharacter, public IEnemyInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyBase();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
	FName Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
	float Health;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
	float MaxHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
	float Damage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
	float AttackRange;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="PatrolPath")
	APatrolPath* PatrolPath;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MoveSpeed")
	float PatrolSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MoveSpeed")
	float ChaseSeed;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	USkeletalMeshComponent* EnemyMesh;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	UFUNCTION(BlueprintCallable)
	void ChangeSpeedPatrol();
	UFUNCTION(BlueprintCallable)
	void ChangeSpeedChase();
	UFUNCTION(BlueprintCallable)
	void StopEnemy();

	// Interface Function
	virtual FName GetName() const override;
	virtual int32 GetHealth() const override;
	virtual float GetDamage() const override;
	virtual void OnDeath() override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	virtual void Attack(AActor* TargetActor) override;
	virtual void DestroyEnemy() override;

	// General Function

};
