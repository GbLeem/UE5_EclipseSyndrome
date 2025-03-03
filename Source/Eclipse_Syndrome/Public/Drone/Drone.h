#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Drone.generated.h"

class AEnemyBase;
class USphereComponent;
class AAOctreeVolume;
struct FInputActionValue;
class UPhysicsHandleComponent;
class UCameraComponent;
class USpringArmComponent;
class USkeletalMeshComponent;
class UCapsuleComponent;
class USceneComponent;

UENUM(BlueprintType)
enum class EDroneControlMode : uint8
{
	Manual UMETA(DisplayName = "Manual"),
	AIControlled UMETA(DisplayName = "AI Controlled")
};

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
	TObjectPtr<USphereComponent> DetectionSphere;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Drone|Component")
	TObjectPtr<UPhysicsHandleComponent> PhysicsHandleComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="PathFinding")
	TObjectPtr<AAOctreeVolume> OctreeVolume;
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drone|Property")
	float DetectionRange;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drone|Property")
	float AttackRange;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drone|Property")
	float FollowRange;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drone|Property")
	float AttackDamage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drone|Property")
	float AttackCooldown;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drone|Property")
	float SpreadAngle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Drone|Property")
	TSet<TObjectPtr<AEnemyBase>> NearbyEnemies;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Drone|Property")
	TObjectPtr<AEnemyBase> CurrentTarget;
	
private:
	FVector MoveInput;
	bool bIsGrabbing;
	
public:
	ADrone();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	void SetMoveInput(const FVector& Value) {MoveInput = Value;}
	TObjectPtr<USceneComponent> GetCameraSceneComponent() {return CameraSceneComp;};
	TObjectPtr<AAOctreeVolume> GetOctreeVolume() { return OctreeVolume; };

	void SetEnhancedInput();

	float GetDetectionRange() { return DetectionRange; }
	float GetAttackRange() { return AttackRange; }
	float GetFollowRange() { return FollowRange; }
	float GetAttackDamage() { return AttackDamage; }
	float GetAttackCooldown() { return AttackCooldown; }
	const TSet<TObjectPtr<AEnemyBase>>& GetNearbyEnemies() { return NearbyEnemies; }

	void Attack(AEnemyBase* Target);

	void DetachGrabActor(bool OnPhysics = false);
	
protected:
	UFUNCTION()
	void OnDetectionOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnDetectionOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	UFUNCTION()
	void Move(const FInputActionValue& Value);
	UFUNCTION()
	void Look(const FInputActionValue& Value);
	UFUNCTION()
	void PossessToCharacter(const FInputActionValue& Value);
	UFUNCTION()
	void Grab(const FInputActionValue& Value);
	
private:
	void ComponentInit();
	void BindingFunction();
	void TiltDrone(float DeltaTime);
	void AddAntiGravity() const;
	void AddAirResistance() const;

	void AttackSingleArm(AEnemyBase* Target, FName BoneName);
	FVector GetBulletDirection();
};