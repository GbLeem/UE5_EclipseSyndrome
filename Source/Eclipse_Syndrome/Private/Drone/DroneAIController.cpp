#include "Drone/DroneAIController.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Drone/Drone.h"
#include "Volume/AOctreeVolume.h"

ADroneAIController::ADroneAIController()
	: BaseDroneOffset(FVector(-50, 80, 100))
	, Kp(140000.0f)
	, Ki(2000.0f)
	, Kd(8000.0f)
	, MaxSpeed(150000.f)
	, DesiredDistance(15.0f)
	, bShowDebug(true)
	, CurIndex(0)
	, PathFindModeAcceleration(20.0f)
	, NextNodeIgnoreRadius(80.0f)
	, bEndFollowPath(true)
	, bCanFindPath(true)
{
}
void ADroneAIController::BeginPlay()
{
	Super::BeginPlay();
}

void ADroneAIController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
	const TObjectPtr<APawn> PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	UpdateDesiredTarget(PlayerPawn);
	
	if (bShowDebug)
	{
		DrawDebugSphere(GetWorld(), DesiredTarget, 30, 30, FColor::Green);
	}
	
	UpdatePath();
	FollowPath(DeltaTime);
	
	if (bShowDebug)
	{
		DrawDebugPath();
	}
	
    TargetLocation = FMath::VInterpTo(TargetLocation, PathTargetLocation, DeltaTime, 5.0f);
    DroneRotation(PlayerPawn);
    ApplyPIDControl(DeltaTime);
}

void ADroneAIController::FindPath()
{
    if (IsValid(CurOctreeVolume))
    {
	    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
    	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Visibility));
    	if (CurOctreeVolume->IsValidDestLocation(DesiredTarget, ObjectTypes, AActor::StaticClass()))
    	{
    		FVector DroneLocation = GetPawn()->GetActorLocation();
    		PathPoints.Empty();
    		if (CurOctreeVolume->FindPath(DroneLocation, DesiredTarget, ObjectTypes, AActor::StaticClass(), PathPoints))
    		{
    			bEndFollowPath = false;
    			CurIndex = 0;
    		}
    	}
	    else
	    {
	    	bCanFindPath = false;
		    GetWorld()->GetTimerManager().SetTimer(PathTimerHandle, this, &ADroneAIController::CanFindPath, 1.0f, false);
	    }
    }
}

void ADroneAIController::CanFindPath()
{
	bCanFindPath = true;
}

void ADroneAIController::DrawDebugPath()
{
	for (const auto& Node : PathPoints)
	{
		DrawDebugSphere(GetWorld(), Node, 20, 20, FColor::Orange);
	}
}

void ADroneAIController::UpdatePath()
{
	FVector DroneLocation = GetPawn()->GetActorLocation();
	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(GetPawn());
	
	if (GetWorld()->LineTraceSingleByChannel(HitResult, DroneLocation, DesiredTarget, ECC_Visibility, CollisionParams))
	{
		if (bCanFindPath && bEndFollowPath)
		{
			FindPath();
		}
	}
}

void ADroneAIController::UpdateDesiredTarget(const TObjectPtr<APawn>& PlayerPawn)
{
	if (!PlayerPawn) return;
	const FVector RotatedOffset = PlayerPawn->GetActorRotation().RotateVector(BaseDroneOffset);
	FHitResult Hit;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(PlayerPawn);
	CollisionParams.AddIgnoredActor(GetPawn());
	if (!GetWorld()->LineTraceSingleByChannel(Hit, PlayerPawn->GetActorLocation() + FVector(0.0f, 0.0f, 100.0f), PlayerPawn->GetActorLocation() + RotatedOffset * 1.5f, ECC_Visibility, CollisionParams))
	{
		DesiredTarget = PlayerPawn->GetActorLocation() + RotatedOffset;
	}
}

void ADroneAIController::FollowPath(float DeltaTime)
{
	if (!bEndFollowPath && !PathPoints.IsEmpty())
	{
		if (CurIndex < PathPoints.Num())
		{
			if (FVector::DistSquared(GetPawn()->GetActorLocation(), PathPoints[CurIndex]) < FMath::Square(NextNodeIgnoreRadius))
			{
				CurIndex++;
			}
			if (CurIndex < PathPoints.Num())
			{
				PathTargetLocation = PathPoints[CurIndex];
			}
			else
			{
				bEndFollowPath = true;
			}
		}
	}
	else
	{
		PathTargetLocation = DesiredTarget;
	}
}

void ADroneAIController::ApplyPIDControl(float DeltaTime)
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
	float PathSpeedBoost = bEndFollowPath ? 1.0f : PathFindModeAcceleration;
	float SpeedFactor = FMath::Clamp(BaseSpeedFactor * PathSpeedBoost, 0.1f, 1.0f);
	
	PIDForce = PIDForce.GetClampedToMaxSize(MaxSpeed * SpeedFactor);
	
	ControlledDrone->GetComponentByClass<UCapsuleComponent>()->AddForce(PIDForce);
	ControlledDrone->SetMoveInput(PIDForce.GetSafeNormal());
	PreviousError = Error;
}
void ADroneAIController::DroneRotation(const TObjectPtr<APawn>& PlayerPawnPtr)
{
	const TObjectPtr<ADrone> ControlledDrone = Cast<ADrone>(GetPawn());
	if (!ControlledDrone) return;
	
	ControlledDrone->GetCameraSceneComponent()->SetRelativeRotation(PlayerPawnPtr->GetActorRotation());
}