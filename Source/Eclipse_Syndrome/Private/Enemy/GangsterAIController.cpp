// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/GangsterAIController.h"
#include "Character/PlayerCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Enemy/GangsterEnemy.h"

void AGangsterAIController::UpdateAttackRange()
{
	APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn) return;

	APlayerCharacter* Player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (!Player) return;

	float Distance = FVector::Dist(ControlledPawn->GetActorLocation(), Player->GetActorLocation());

	bool bInRange = Distance <= AttackRange;
	GetBlackboardComponent()->SetValueAsBool(TEXT("IsInAttackRange"), bInRange);
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
