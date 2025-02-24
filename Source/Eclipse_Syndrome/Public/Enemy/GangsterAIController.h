// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "GangsterAIController.generated.h"

/**
 * 
 */
UCLASS()
class ECLIPSE_SYNDROME_API AGangsterAIController : public AAIController
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, Category="AI")
	float AttackRange;

	void UpdateAttackRange();

	virtual void OnPossess(APawn* InPawn) override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;
	
};
