// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlayerCamera.generated.h"

class UCameraComponent;

UCLASS()
class ECLIPSE_SYNDROME_API APlayerCamera : public AActor
{
	GENERATED_BODY()
	
public:	
	APlayerCamera();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
	TObjectPtr<UCameraComponent> CameraComp;
};
