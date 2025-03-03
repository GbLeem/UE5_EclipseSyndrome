#include "Drone/DroneAIController.h"

#include "EngineUtils.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Drone/Drone.h"
#include "Enemy/EnemyBase.h"
#include "System/DefaultGameState.h"
#include "Volume/AOctreeVolume.h"
#include "Volume/NavNode.h"

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
	
	EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("BlackboardEnum"));
}

void ADroneAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (const TObjectPtr<APawn> Player = Cast<ADefaultGameState>(GetWorld()->GetGameState())->GetPlayerCharacter())
	{
		BlackboardComp->SetValueAsObject("PlayerActor", Player);
	}
	
	CurOctreeVolume = Cast<ADrone>(Cast<ADefaultGameState>(GetWorld()->GetGameState())->GetDrone())->GetOctreeVolume();
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
		const bool bPlayerFar = DistanceToPlayer > 500.0f;
	
		float DistanceChange = FMath::Abs(DistanceToPlayer - LastDistanceToPlayer);

		if (!bExecuteCommand)
		{
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

	float MinSpeed = 15000.0f;
	if (PIDForce.Size() < MinSpeed)
	{
		PIDForce = PIDForce.GetSafeNormal() * MinSpeed;
	}
	
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

	float InterpSpeed = 3.0f;
	// Idle State
	if (BlackboardComp->GetValueAsEnum("CurrentState") == 0)
	{
		TargetRotation = PlayerPawnPtr->GetActorRotation();
	}
	// Follow State
	else if (BlackboardComp->GetValueAsEnum("CurrentState") == 1
		|| BlackboardComp->GetValueAsEnum("CurrentState") == 2)
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

	// Attack State
	else if (BlackboardComp->GetValueAsEnum("CurrentState") == 4)
	{
		if (UObject* Target = BlackboardComp->GetValueAsObject("TargetEnemy"))
		{
			FVector EnemyLocation = Cast<AActor>(Target)->GetActorLocation();
			FVector DroneLocation = ControlledDrone->GetActorLocation();
			FVector DirectionToTarget = (EnemyLocation - DroneLocation).GetSafeNormal();
			TargetRotation = DirectionToTarget.Rotation();
			InterpSpeed = 10.0f;
		}
	}

	FRotator NewRotation = FMath::RInterpTo(
		CurrentRotation,
		TargetRotation,
		GetWorld()->GetDeltaSeconds(),
		InterpSpeed
	);

	ControlledDrone->GetCameraSceneComponent()->SetRelativeRotation(NewRotation);
}

void ADroneAIController::DroneMoveCommand(const FVector& NewCommandLocation)
{
	MoveCommandLocation = NewCommandLocation;
	
	// if (TObjectPtr<AAOctreeVolume> OctreeVolume = Cast<ADrone>(GetPawn())->GetOctreeVolume())
	// {
	// 	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	// 	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Visibility));
	// 	if (!OctreeVolume->IsValidDestLocation(NewCommandLocation, ObjectTypes, AActor::StaticClass()))
	// 	{
	// 		;
	// 		MoveCommandLocation = OctreeVolume->ConvertCoordinatesToLocation(OctreeVolume->FindNearestValidNode(NewCommandLocation, ObjectTypes, AActor::StaticClass())->Coordinates);
	// 	}
	// }
	
	bExecuteCommand = true;
	BlackboardComp->SetValueAsVector("MoveCommandLocation", MoveCommandLocation);
	BlackboardComp->SetValueAsEnum("CurrentState", 2);
}

void ADroneAIController::DroneAttack()
{
	bExecuteCommand = true;
	BlackboardComp->SetValueAsEnum(TEXT("CurrentState"), 4);
	BlackboardComp->SetValueAsInt(TEXT("AttackType"), 0);
}