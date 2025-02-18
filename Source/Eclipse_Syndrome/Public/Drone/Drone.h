#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Drone.generated.h"

class UPhysicsHandleComponent;
class UCameraComponent;
class USpringArmComponent;
class USkeletalMeshComponent;
class UCapsuleComponent;

UCLASS()
class ECLIPSE_SYNDROME_API ADrone : public APawn
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Drone|Component")
	TObjectPtr<USkeletalMeshComponent> SkeletalMeshComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Drone|Component")
	TObjectPtr<UCapsuleComponent> CapsuleComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Drone|Component")
	TObjectPtr<USpringArmComponent> SpringArmComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Drone|Component")
	TObjectPtr<UCameraComponent> CameraComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Drone|Component")
	TObjectPtr<UPhysicsHandleComponent> PhysicsHandleComp;
	
	
public:
	ADrone();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};