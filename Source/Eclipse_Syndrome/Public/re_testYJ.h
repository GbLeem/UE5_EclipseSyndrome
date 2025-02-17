// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "re_testYJ.generated.h"

UCLASS()
class ECLIPSE_SYNDROME_API Are_testYJ : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	Are_testYJ();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
