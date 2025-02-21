#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "DroneAIController.generated.h"

class AAOctreeVolume;
class USplineComponent;
class ADrone;

UCLASS()
class ECLIPSE_SYNDROME_API ADroneAIController : public AAIController
{
	GENERATED_BODY()

private:
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
	TArray<FVector> PathPoints;
	FTimerHandle PathTimerHandle;
	int32 CurIndex;
	UPROPERTY(EditAnywhere, Category="PathFinding")
	float PathFindModeAcceleration;
	float NextNodeIgnoreRadius;
	FVector PathTargetLocation;
	bool bEndFollowPath;
	bool bCanFindPath;
	
	FVector TargetLocation;
	FVector IntegralError;
	FVector PreviousError;
	FVector DesiredTarget;
	
	ADroneAIController();
	void ApplyPIDControl(float DeltaTime);
	void DroneRotation(const TObjectPtr<APawn>& PlayerPawnPtr);
	
protected:
	virtual void BeginPlay() override;

	// Use Octree Volume + A* Find Path
	void SetOctreeVolume(const TObjectPtr<AAOctreeVolume>& Volume) {CurOctreeVolume = Volume;};
	void FindPath();
	void CanFindPath();
	void DrawDebugPath();
	void FollowPath(float DeltaTime);
	void UpdatePath();
	void UpdateDesiredTarget(const TObjectPtr<APawn>& PlayerPawn);

public:
	virtual  void Tick(float DeltaTime) override;
};
