// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/ZombieAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Character/PlayerCharacter.h"
#include "Enemy/ZombieEnemy.h"

void AZombieAIController::UpdateAttackRange()
{
	APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn) return;

	APlayerCharacter* Player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (!Player) return;

	float Distance = FVector::Dist(ControlledPawn->GetActorLocation(), Player->GetActorLocation());

	bool bInRange = Distance <= AttackRange;

	GetBlackboardComponent()->GetValueAsBool(TEXT("IsInAttackRange"));
	UE_LOG(LogTemp, Warning, TEXT("%.2f"), Distance);
	GetBlackboardComponent()->SetValueAsBool(TEXT("IsInAttackRange"), bInRange);
}

void AZombieAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
}

void AZombieAIController::BeginPlay()
{
	Super::BeginPlay();

	AZombieEnemy* ControlledZombieEnemy = Cast<AZombieEnemy>(GetPawn());
	if (!ControlledZombieEnemy) return;

	AttackRange = ControlledZombieEnemy->AttackRange;
}

void AZombieAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateAttackRange();
}

void AZombieAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	Super::OnMoveCompleted(RequestID, Result);
}
