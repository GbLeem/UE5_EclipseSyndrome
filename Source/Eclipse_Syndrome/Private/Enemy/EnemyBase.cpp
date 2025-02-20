// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyBase.h"

// Sets default values
AEnemyBase::AEnemyBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Name = "EnemyBase";
	MaxHealth = 100.0f;
	Health = MaxHealth;
	Damage = 10.0f;
}

// Called when the game starts or when spawned
void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FName AEnemyBase::GetName() const
{
	return FName();
}

int32 AEnemyBase::GetHealth() const
{
	return int32();
}

float AEnemyBase::GetDamage() const
{
	return 0.0f;
}

void AEnemyBase::OnDeath()
{
}

float AEnemyBase::TakeDamage()
{
	return 0.0f;
}

void AEnemyBase::Attack()
{
}

