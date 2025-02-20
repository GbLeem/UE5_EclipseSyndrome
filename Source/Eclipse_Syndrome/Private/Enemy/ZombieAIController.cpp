// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/ZombieAIController.h"

void AZombieAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
}

void AZombieAIController::BeginPlay()
{
	Super::BeginPlay();
}

void AZombieAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	Super::OnMoveCompleted(RequestID, Result);
}
