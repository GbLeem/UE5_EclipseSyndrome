// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/ZombieEnemy.h"
#include "Enemy/ZombieAIController.h"
#include "System/DefaultGameState.h"
#include "Character/PlayerCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"

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

	// Animation
	SelectedAttackMontage = nullptr;
}

UAnimMontage* AZombieEnemy::GetRandomAttackMontage()
{
	if (!AttackMontage1 || !AttackMontage2 || !AttackMontage3)
	{
		return nullptr;
	}

	TArray<UAnimMontage*> AttackMontages = { AttackMontage1, AttackMontage2, AttackMontage3 };

	int32 RandomIndex = FMath::RandRange(0, AttackMontages.Num() - 1);
	SelectedAttackMontage = AttackMontages[RandomIndex];

	return SelectedAttackMontage;
}

void AZombieEnemy::OnDeath()
{
	Super::OnDeath();
	AZombieAIController* AIController = Cast<AZombieAIController>(GetController());
	if (!AIController) return;
	AIController->GetBlackboardComponent()->SetValueAsBool(TEXT("IsAlive"), false);

	FTimerHandle DestoryHandle;
	GetWorldTimerManager().SetTimer(DestoryHandle, this, &AZombieEnemy::DestroyEnemy, 5.0f);

}

float AZombieEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	
	// Get ZombieAIController
	AZombieAIController* AIController = Cast<AZombieAIController>(GetController());
	if (!AIController) return ActualDamage;


	if (AIController->GetBlackboardComponent()->GetValueAsObject(TEXT("TargetActor")) == nullptr)
	{
		ADefaultGameState* DefaultGameState = Cast<ADefaultGameState>(GetWorld()->GetGameState());
		if (DefaultGameState)
		{
			APlayerCharacter* Player = DefaultGameState->GetPlayerCharacter();
			if (Player)
			{
				AIController->GetBlackboardComponent()->SetValueAsBool(TEXT("PlayerDetected"), true);
				AIController->GetBlackboardComponent()->SetValueAsObject(TEXT("TargetActor"), Player);
			}
		}
	}
	
	// Hit Animation
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		if (HitMontage)
		{
			if (Health > 0)
			{
				AnimInstance->Montage_Stop(0.1f, SelectedAttackMontage); // 기존 Attack 멈춤
				AnimInstance->Montage_Play(HitMontage);
			}			
		}
	}

	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, DamageCauser->GetName()); // 추후 삭제
	return ActualDamage;
}