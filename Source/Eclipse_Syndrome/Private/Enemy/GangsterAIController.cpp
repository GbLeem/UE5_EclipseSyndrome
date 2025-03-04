// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/GangsterAIController.h"
#include "Character/PlayerCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Enemy/GangsterEnemy.h"


AGangsterAIController::AGangsterAIController()
{
	
}

void AGangsterAIController::UpdateAttackRange()
{
	APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn) return;

	APlayerCharacter* Player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (!Player) return;

	float Distance = FVector::Dist(ControlledPawn->GetActorLocation(), Player->GetActorLocation());

	// Update Current EnemyState
	if (Distance <= ShootingRange)
	{
		CurrentState = EEnemyStateEnum::Shooting; // Stop and shoot
	}
	else if (CurrentState==EEnemyStateEnum::Shooting && Distance > AdvancingRange && Distance <= ChasingRange) // Shooting -> Advancing
	{	
		CurrentState = EEnemyStateEnum::Advancing; // Walk and Shoot
	}
	else if (CurrentState != EEnemyStateEnum::Shooting && Distance > ShootingRange && Distance <= ChasingRange) // Chasing -> Advancing
	{
		CurrentState = EEnemyStateEnum::Advancing; // Walk and Shoot
	}
	else if (Distance > ChasingRange)
	{
		CurrentState = EEnemyStateEnum::Chasing; // Chasing
	}

	// Update EnemyState(Blackboard) when CurrentState changed
	if (CurrentState != static_cast<EEnemyStateEnum>(GetBlackboardComponent()->GetValueAsEnum(TEXT("EnemyState"))))
	{
		FString StateStr = UEnum::GetValueAsString(CurrentState);
		FString DebugMessage = FString::Printf(TEXT("change state : %s"), *StateStr);
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Magenta, DebugMessage);

		GetBlackboardComponent()->SetValueAsEnum(TEXT("EnemyState"), static_cast<uint8>(CurrentState));
	}
}

void AGangsterAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	AGangsterEnemy* ControlledEnemy = Cast<AGangsterEnemy>(GetPawn());
	if (!ControlledEnemy) return;

	ChasingRange = ControlledEnemy->ChasingRange;
	AdvancingRange = ControlledEnemy->AdvancingRange;
	ShootingRange = ControlledEnemy->ShootingRange;
}

void AGangsterAIController::BeginPlay()
{
	Super::BeginPlay();
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
