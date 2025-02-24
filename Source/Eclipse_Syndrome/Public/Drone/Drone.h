#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Drone.generated.h"

struct FInputActionValue;
class UPhysicsHandleComponent;
class UCameraComponent;
class USpringArmComponent;
class USkeletalMeshComponent;
class UCapsuleComponent;
class USceneComponent;

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
	TObjectPtr<USceneComponent> CameraSceneComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Drone|Component")
	TObjectPtr<USceneComponent> TiltSceneComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Drone|Component")
	TObjectPtr<USpringArmComponent> SpringArmComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Drone|Component")
	TObjectPtr<UCameraComponent> CameraComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Drone|Component")
	TObjectPtr<UPhysicsHandleComponent> PhysicsHandleComp;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DroneMovement|Property")
	float MoveForce;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DroneMovement|Property")
	float AntiForce;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DroneMovement|Property")
	float AirResistance; // 감속 비율 0 ~ 1
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DroneMovement|Property")
	float MaxTiltAngle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DroneMovement|Property")
	float InterpSpeed;

private:
	FVector MoveInput;
	
public:
	ADrone();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	void SetMoveInput(const FVector& Value) {MoveInput = Value;}
	TObjectPtr<USceneComponent> GetCameraSceneComponent() {return CameraSceneComp;};

protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	UFUNCTION()
	void Move(const FInputActionValue& Value);
	UFUNCTION()
	void Look(const FInputActionValue& Value);

private:
	void ComponentInit();
	void TiltDrone(float DeltaTime);
	void AddAntiGravity() const;
	void AddAirResistance() const;
};