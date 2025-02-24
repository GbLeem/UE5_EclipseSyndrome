#include "Drone/DroneAIController.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Drone/Drone.h"

ADroneAIController::ADroneAIController()
	: BaseDroneOffset(FVector(25, 80, 100))
	, Kp(140000.0f)
	, Ki(2000.0f)
	, Kd(8000.0f)
	, MaxSpeed(150000.f)
	, DesiredDistance(15.0f)
	, bShowDebug(true)
	, PathFindModeAcceleration(20.0f)
{
}
void ADroneAIController::BeginPlay()
{
	Super::BeginPlay();

	CircleRadius = (BaseDroneOffset - FVector(0.0f, 0.0f, 100.0f)).Length();

	UseBlackboard(DroneBehaviorTree->GetBlackboardAsset(), BlackboardComp);
	RunBehaviorTree(DroneBehaviorTree);

	if (const TObjectPtr<APawn> Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0))
	{
		BlackboardComp->SetValueAsObject("PlayerActor", Player);
	}

	EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("BlackboardEnum"));
}

void ADroneAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//UpdateRollingCircleMovement(DeltaTime);
	
	if (const TObjectPtr<APawn> Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0))
	{
		const FVector PlayerVelocity = Player->GetVelocity();
		const FVector DroneVelocity = GetPawn()->GetVelocity();
		const float DistanceToPlayer = FVector::Dist(Player->GetActorLocation(), GetPawn()->GetActorLocation());
    	
		static float IdleTransitionTime = 0.0f;
		static float FollowTransitionTime = 0.0f;
		static float LastDistanceToPlayer = DistanceToPlayer;
	
		const bool bPlayerStopped = PlayerVelocity.Length() < 10.0f;
		const bool bDroneAlmostStopped = DroneVelocity.Length() <= 10.0f;
		const bool bPlayerClose = DistanceToPlayer < 300.0f;
		const bool bPlayerFar = DistanceToPlayer > 1000.0f;
	
		float DistanceChange = FMath::Abs(DistanceToPlayer - LastDistanceToPlayer);
	
		if (bPlayerStopped && bDroneAlmostStopped && bPlayerClose && DistanceChange < 5.0f)
		{
			IdleTransitionTime += DeltaTime;
			FollowTransitionTime = 0.0f;
	
			if (IdleTransitionTime >= 1.0f)
			{
				BlackboardComp->SetValueAsEnum("CurrentState", 0); // Idle State
			}
		}
		else if (bPlayerFar)
		{
			BlackboardComp->SetValueAsEnum("CurrentState", 1); // Follow State
			IdleTransitionTime = 0.0f;
			FollowTransitionTime = 0.0f;
		}
		else
		{
			FollowTransitionTime += DeltaTime;
			IdleTransitionTime = 0.0f;
	
			if (FollowTransitionTime >= 0.5f && DistanceChange > 5.0f)
			{
				BlackboardComp->SetValueAsEnum("CurrentState", 1); // Follow State
			}
		}
		LastDistanceToPlayer = DistanceToPlayer;
	
		DroneRotation(Player);
	}
}

void ADroneAIController::ApplyPIDControl(float DeltaTime, bool IsFollowPath)
{
	TObjectPtr<ADrone> ControlledDrone = Cast<ADrone>(GetPawn());
	if (!ControlledDrone) return;
	const FVector DroneLocation = ControlledDrone->GetActorLocation();
	const FVector Error = TargetLocation - DroneLocation;
	if (bShowDebug)
	{
		DrawDebugSphere(GetWorld(), TargetLocation, 30, 30, FColor::Red);
	}
	
	if (Error.Size() < DesiredDistance)
	{
		 ControlledDrone->SetMoveInput(FVector::ZeroVector);
		PreviousError = FVector::ZeroVector;
		IntegralError = FVector::ZeroVector;
		 return;
	}
	
	IntegralError += Error * DeltaTime;
	const FVector DerivativeError = (Error - PreviousError) / DeltaTime;
	FVector PIDForce = (Error * Kp) + (IntegralError * Ki) + (DerivativeError * Kd);
	
	float DistanceToTarget = Error.Size();
	
	float BaseSpeedFactor = DistanceToTarget / 500.0f; 
	float PathSpeedBoost = IsFollowPath ? PathFindModeAcceleration : 1.0f;
	float SpeedFactor = FMath::Clamp(BaseSpeedFactor * PathSpeedBoost, 0.1f, 1.0f);
	
	PIDForce = PIDForce.GetClampedToMaxSize(MaxSpeed * SpeedFactor);
	
	ControlledDrone->GetComponentByClass<UCapsuleComponent>()->AddForce(PIDForce);
	ControlledDrone->SetMoveInput(PIDForce.GetSafeNormal());
	PreviousError = Error;
}

void ADroneAIController::ApplySmoothMovement(float DeltaTime)
{
	TargetLocation = FMath::VInterpTo(TargetLocation, NewTargetLocation, DeltaTime, 5.0f);
}

 void ADroneAIController::DroneRotation(const TObjectPtr<APawn>& PlayerPawnPtr)
{
	const TObjectPtr<ADrone> ControlledDrone = Cast<ADrone>(GetPawn());
	if (!ControlledDrone) return;

	FRotator CurrentRotation = ControlledDrone->GetCameraSceneComponent()->GetRelativeRotation();
	FRotator TargetRotation;

	// Idle State
	if (BlackboardComp->GetValueAsEnum("CurrentState") == 0)
	{
		TargetRotation = PlayerPawnPtr->GetActorRotation();
	}
	// Follow State
	else if (BlackboardComp->GetValueAsEnum("CurrentState") == 1)
	{
		FVector DroneLocation = ControlledDrone->GetActorLocation();
		FVector DirectionToPlayer = (TargetLocation - DroneLocation).GetSafeNormal();
		TargetRotation = DirectionToPlayer.Rotation();

		//  DeadZone
		float DistanceToTarget = FVector::Dist(DroneLocation, TargetLocation);
		if (DistanceToTarget < 15.0f) 
		{
			return;
		}
	}

	FRotator NewRotation = FMath::RInterpTo(
		CurrentRotation,
		TargetRotation,
		GetWorld()->GetDeltaSeconds(),
		3.0f
	);

	ControlledDrone->GetCameraSceneComponent()->SetRelativeRotation(NewRotation);
}


void ADroneAIController::UpdateHappyMovement(float DeltaTime)
{
	const TObjectPtr<ADrone> ControlledDrone = Cast<ADrone>(GetPawn());
	if (!ControlledDrone) return;

	const TObjectPtr<APawn> Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!Player) return;

	const FVector PlayerLocation = Player->GetActorLocation() + BaseDroneOffset;

	const float Time = GetWorld()->GetTimeSeconds();

	float BounceHeight = FMath::Abs(FMath::Sin(Time * 1.0f)) * 50.0f;

	float EaseFactor = (FMath::Cos(Time * 2.0f) + 1.0f) / 2.0f;

	FVector RandomShake = FVector(
		FMath::Sin(Time * 2.0f + FMath::RandRange(-0.5f, 0.5f)) * 2.0f,
		FMath::Cos(Time * 2.0f + FMath::RandRange(-0.5f, 0.5f)) * 2.0f,
		0.0f
	);

	FVector TargetLocations = PlayerLocation + RandomShake + FVector(0.0f, 0.0f, BounceHeight * EaseFactor);

	// 좌우 틸트 회전 (Roll)
	float TiltAngle = FMath::Sin(Time * 3.0f) * 15.0f; // 좌우 기울기 각도
	FRotator NewTiltRotation = ControlledDrone->GetCameraSceneComponent()->GetRelativeRotation();
	NewTiltRotation.Roll = FMath::FInterpTo(NewTiltRotation.Roll, TiltAngle, DeltaTime, 8.0f);
	// 회전 적용
	ControlledDrone->GetCameraSceneComponent()->SetRelativeRotation(NewTiltRotation);
	
	SetNewTargetLocation(TargetLocations);
	ApplySmoothMovement(DeltaTime);
	ApplyPIDControl(DeltaTime, true);
}


void ADroneAIController::UpdateRollingCircleMovement(float DeltaTime)
{
	const TObjectPtr<ADrone> ControlledDrone = Cast<ADrone>(GetPawn());
	if (!ControlledDrone) return;

	const TObjectPtr<APawn> Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!Player) return;

	const FVector PlayerLocation = Player->GetActorLocation() + FVector(0.0f, 0.0f, 100.0f);

	const float Time = GetWorld()->GetTimeSeconds();

	// 원형 궤도 계산
	const float AngleSpeed = GetRotationSpeed();

	const FVector Offset = FVector(
		FMath::Cos(CurrentAngle) * CircleRadius,
		FMath::Sin(CurrentAngle) * CircleRadius,
		0.0f
	);

	FVector TargetLocations = PlayerLocation + Offset;

	// 🎲 랜덤 Roll 속도 조절
	const float RollStep = 60.0f; // 60도 단위로 속도 갱신
	if (FMath::Fmod(CurrentAngle, RollStep) < 5.0f && !bHasUpdatedRollSpeed)
	{
		TargetRollSpeed = FMath::RandRange(MinRollSpeed, MaxRollSpeed);
		bHasUpdatedRollSpeed = true;
	}
	else if (FMath::Fmod(CurrentAngle, RollStep) > 5.0f)
	{
		bHasUpdatedRollSpeed = false;
	}

	// Roll 회전 계산 (부드럽게 보간)
	CurrentRollAngle += FMath::FInterpTo(CurrentRollSpeed, TargetRollSpeed, DeltaTime, 1.5f) * DeltaTime;

	// Roll 한 바퀴 돌면 리셋
	if (CurrentRollAngle >= 360.0f)
	{
		CurrentRollAngle = 0.0f;
	}

	FRotator RollRotation = FRotator(
		0.0f,
		0.0f,
		CurrentRollAngle//FMath::Sin(CurrentRollAngle * PI / 180.0f) * 45.0f
	);

	// 회전 적용
	ControlledDrone->GetCameraSceneComponent()->SetRelativeRotation(RollRotation);

	// 위치 갱신
	SetNewTargetLocation(TargetLocations);
	ApplySmoothMovement(DeltaTime);
	ApplyPIDControl(DeltaTime);

	// 각도 업데이트
	CurrentAngle += AngleSpeed * DeltaTime;
}