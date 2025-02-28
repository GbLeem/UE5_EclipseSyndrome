// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/GangsterAIController.h"
#include "Character/PlayerCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Enemy/GangsterEnemy.h"


AGangsterAIController::AGangsterAIController()
{
	AGangsterEnemy* ControlledEnemy = Cast<AGangsterEnemy>(GetPawn());
	if (!ControlledEnemy) return;
	AttackReadyRange = ControlledEnemy->AttackReadyRange;
	//UE_LOG(LogTemp, Warning, TEXT("AttackReadyRange : %f!!!!!!!!!!!!!!!!!!!!!!!!"), AttackReadyRange);
	AttackRange = ControlledEnemy->AttackRange;
}

void AGangsterAIController::UpdateAttackRange()
{
	APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn) return;

	APlayerCharacter* Player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (!Player) return;

	float Distance = FVector::Dist(ControlledPawn->GetActorLocation(), Player->GetActorLocation());

	// Update Current EnemyState
	if (Distance <= 450)
	{
		CurrentState = EEnemyStateEnum::Shooting; // Stop and shoot
	}
	else if (Distance > 450 && Distance <= 2000)
	{	
		CurrentState = EEnemyStateEnum::Advancing; // Walk and Shoot
	}
	else if (Distance > 2000)
	{
		CurrentState = EEnemyStateEnum::Chasing; // Chasing
	}
	if (CurrentState != static_cast<EEnemyStateEnum>(GetBlackboardComponent()->GetValueAsEnum(TEXT("EnemyState"))))
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Magenta, TEXT("change state"));
		GetBlackboardComponent()->SetValueAsEnum(TEXT("EnemyState"), static_cast<uint8>(CurrentState));
	}
}

void AGangsterAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
}

void AGangsterAIController::BeginPlay()
{
	Super::BeginPlay();

	AGangsterEnemy* ControlledGansterEnemy = Cast<AGangsterEnemy>(GetPawn());
	if (!ControlledGansterEnemy) return;

	AttackRange = ControlledGansterEnemy->AttackRange;
}

void AGangsterAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateAttackRange();
}

void AGangsterAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	Super::OnMoveCompleted(RequestID, Result);
}
