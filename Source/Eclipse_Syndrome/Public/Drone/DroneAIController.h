#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "DroneAIController.generated.h"

enum class EDroneState : uint8;
class AAOctreeVolume;
class USplineComponent;
class ADrone;

UCLASS()
class ECLIPSE_SYNDROME_API ADroneAIController : public AAIController
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, Category= "AI")
	TObjectPtr<UBehaviorTree> DroneBehaviorTree;
	UPROPERTY(VisibleAnywhere, Category = "AI")
	UBlackboardComponent* BlackboardComp;
	UPROPERTY(VisibleAnywhere, Category = "AI")
	TObjectPtr<UEnum> EnumPtr;
	
	UPROPERTY(EditAnywhere, Category= "PID")
	FVector BaseDroneOffset;
	UPROPERTY(EditAnywhere, Category="PID")
	float Kp; //100000.f;
	UPROPERTY(EditAnywhere, Category="PID")
	float Ki; //5000.f;
	UPROPERTY(EditAnywhere, Category="PID")
	float Kd; //10000.f;
	UPROPERTY(EditAnywhere, Category="PID")
	float MaxSpeed;
	UPROPERTY(EditAnywhere, Category="PID")
	float DesiredDistance;
	UPROPERTY(EditAnywhere, Category="PID")
	bool bShowDebug;

	UPROPERTY(EditAnywhere, Category="PathFinding")
	TObjectPtr<AAOctreeVolume> CurOctreeVolume;
	UPROPERTY(EditAnywhere, Category="PathFinding")
	float PathFindModeAcceleration;
	
	FVector TargetLocation;
	FVector NewTargetLocation;
	FVector IntegralError;
	FVector PreviousError;

	float CurrentAngle = 0.0f;
	float CircleRadius = 180.0f;
	UPROPERTY(editAnywhere, Category="IdleRotation")
	float RotationSpeed = 0.4f;
	UPROPERTY(editAnywhere, Category="IdleRotation")
	float HeightNoiseSpeed = 0.3f; // 0.2 ~ 0.5
	UPROPERTY(editAnywhere, Category="IdleRotation")
	float MaxHeightVariation = 50.0f;
	UPROPERTY(editAnywhere, Category="IdleRotation")
	float DistanceScaleFactor = 0.7; // Perspective correction strength (0.3 ~ 0.5)
	
	void DroneRotation(const TObjectPtr<APawn>& PlayerPawnPtr);
	
protected:
	virtual void BeginPlay() override;

	// Use Octree Volume + A* Find Path
	void SetOctreeVolume(const TObjectPtr<AAOctreeVolume>& Volume) {CurOctreeVolume = Volume;};
	
public:
	ADroneAIController();
	virtual  void Tick(float DeltaTime) override;

	void ApplyPIDControl(float DeltaTime, bool IsFollowPath = false);
	void SetNewTargetLocation(const FVector& NTargetLocation) { NewTargetLocation = NTargetLocation; }
	void ApplySmoothMovement(float DeltaTime);
	
	// IdleMovement Getters
	float GetCurrentAngle() const { return CurrentAngle; }
	float GetCircleRadius() const { return CircleRadius; }
	float GetRotationSpeed() const { return RotationSpeed; }
	float GetHeightNoiseSpeed() const { return HeightNoiseSpeed; }
	float GetMaxHeightVariation() const { return MaxHeightVariation; }
	float GetDistanceScaleFactor() const { return DistanceScaleFactor; }

	// IdleMovement Setters
	void SetCurrentAngle(float NewAngle) { CurrentAngle = NewAngle; }
	void SetCircleRadius(float NewRadius) { CircleRadius = NewRadius; }
	void SetRotationSpeed(float NewSpeed) { RotationSpeed = NewSpeed; }
	void SetHeightNoiseSpeed(float NewSpeed) { HeightNoiseSpeed = NewSpeed; }
	void SetMaxHeightVariation(float NewVariation) { MaxHeightVariation = NewVariation; }
	void SetDistanceScaleFactor(float NewScaleFactor) { DistanceScaleFactor = NewScaleFactor; }

	// Getters
	bool GetShowDebug() const { return bShowDebug; }
	FVector GetBaseDroneOffset() const { return BaseDroneOffset; }
	TObjectPtr<AAOctreeVolume> GetOctreeVolume() { return CurOctreeVolume; }
	float GetDesiredDistance() const { return DesiredDistance; }

	//Setters
	void SetDesiredDistance(float NewDesiredDistance) { DesiredDistance = NewDesiredDistance; }
	
	void UpdateHappyMovement(float DeltaTime);

private:
	float TargetRollSpeed = 0.0f;
	float CurrentRollSpeed = 0.0f;
	float MinRollSpeed = 60.0f;
	float MaxRollSpeed = 170.0f;
	bool bHasUpdatedRollSpeed = false;
	float CurrentRollAngle = 0.0f;
};