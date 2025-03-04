// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AEnemyBase::AEnemyBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Name = "EnemyBase";
	MaxHealth = 100.0f;
	Health = MaxHealth;
	Damage = 10.0f;
	AttackRange = 150.0f;
	PatrolSpeed = 100.0f;
	ChaseSeed = 200.0f;

	EnemyMesh = GetMesh();
}

// Called when the game starts or when spawned
void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();
	this->Tags.Add(FName("Enemy"));
}

void AEnemyBase::ChangeSpeedPatrol()
{
	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
	if (MovementComponent)
	{
		MovementComponent->MaxWalkSpeed = PatrolSpeed;
	}
}

void AEnemyBase::ChangeSpeedChase()
{
	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
	if (MovementComponent)
	{
		MovementComponent->MaxWalkSpeed = ChaseSeed;
	}
}

void AEnemyBase::StopEnemy()
{
	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
	if (MovementComponent)
	{
		MovementComponent->MaxWalkSpeed = 0.0f;
	}
}

FName AEnemyBase::GetName() const
{
	return Name;
}

int32 AEnemyBase::GetHealth() const
{
	return Health;
}

float AEnemyBase::GetDamage() const
{
	return Damage;
}

void AEnemyBase::OnDeath()
{
	StopEnemy();

	// Ragdoll Effect On
	EnemyMesh->SetSimulatePhysics(true);
	EnemyMesh->SetCollisionProfileName(TEXT("Ragdoll"));

	FTimerHandle DestoryHandle;
	GetWorldTimerManager().SetTimer(DestoryHandle, this, &AEnemyBase::DestroyEnemy, 5.0f);
}

float AEnemyBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	Health = FMath::Clamp(Health - DamageAmount, 0.0f, MaxHealth);
	UE_LOG(LogTemp, Warning, TEXT("Enemy Health decreased to : %f"), Health);

	if (Health <= 0.0f)
	{
		OnDeath();
	}
	return ActualDamage;
}

void AEnemyBase::Attack(AActor* TargetActor)
{
	// Apply damage
	UGameplayStatics::ApplyDamage(TargetActor, Damage, nullptr, this, UDamageType::StaticClass());
}

void AEnemyBase::DestroyEnemy()
{
	Destroy();
}

