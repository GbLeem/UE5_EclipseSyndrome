#include "Drone/BehaviorTree/BTTask_FollowWithAvoidance.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Drone/Drone.h"
#include "Drone/DroneAIController.h"
#include "Volume/AOctreeVolume.h"

UBTTask_FollowWithAvoidance::UBTTask_FollowWithAvoidance()
	: DesiredTarget(FVector::ZeroVector)
	, bEndFollowPath(true)
	, bCanFindPath(true)
	, NextNodeIgnoreRadius(60.0f)
{
	NodeName = "FollowWithAvoidance";
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_FollowWithAvoidance::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	TObjectPtr<AAIController> AIController = OwnerComp.GetAIOwner();
	if (AIController)
	{
		const TObjectPtr<AActor> Player = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject("PlayerActor"));
		const TObjectPtr<ADrone> Drone = Cast<ADrone>(AIController->GetPawn());

		if (Player && Drone)
		{
			PathPoints.Empty();
			CanFindPath();
			bEndFollowPath = true;
			TObjectPtr<ADroneAIController> DroneAIController = Cast<ADroneAIController>(AIController);
			UpdateDesiredTarget(Player, DroneAIController);
			DroneAIController->SetPIDDefaultSpeed();
			if (DroneAIController->GetShowDebug())
			{
				DrawDebugSphere(GetWorld(), DesiredTarget, 30, 30, FColor::Green);
			}

			UpdatePath(DroneAIController);
			FollowPath(DroneAIController, OwnerComp.GetWorld()->GetDeltaSeconds());
			
			if (DroneAIController->GetShowDebug())
			{
				DrawDebugPath();
			}

			return EBTNodeResult::InProgress;
		}
	}

	return EBTNodeResult::Failed;
}

void UBTTask_FollowWithAvoidance::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	if (OwnerComp.GetBlackboardComponent()->GetValueAsEnum("CurrentState") != 1)
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	
	TObjectPtr<AAIController> AIController = OwnerComp.GetAIOwner();
	if (AIController)
	{
		const TObjectPtr<AActor> Player = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject("PlayerActor"));
		const TObjectPtr<ADrone> Drone = Cast<ADrone>(AIController->GetPawn());

		if (Player && Drone)
		{
			TObjectPtr<ADroneAIController> DroneAIController = Cast<ADroneAIController>(AIController);
			UpdateDesiredTarget(Player, DroneAIController);
			if (DroneAIController->GetShowDebug())
			{
				DrawDebugSphere(GetWorld(), DesiredTarget, 30, 30, FColor::Green);
			}

			UpdatePath(DroneAIController);
			FollowPath(DroneAIController, DeltaSeconds);
			
			if (DroneAIController->GetShowDebug())
			{
				DrawDebugPath();
			}

			return;
		}
	}
	
	// 플레이어나 드론 없으면 실패 처리
	FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
}


void UBTTask_FollowWithAvoidance::UpdateDesiredTarget(const TObjectPtr<AActor>& PlayerPawn, const TObjectPtr<ADroneAIController>& DroneAIController)
{
	if (!PlayerPawn) return;
	const FVector RotatedOffset = PlayerPawn->GetActorRotation().RotateVector(DroneAIController->GetBaseDroneOffset());
	FHitResult Hit;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(PlayerPawn);
	CollisionParams.AddIgnoredActor(DroneAIController->GetPawn());
	
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Visibility));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));
	if (!GetWorld()->LineTraceSingleByObjectType(Hit, PlayerPawn->GetActorLocation() + FVector(0.0f, 0.0f, 100.0f), PlayerPawn->GetActorLocation() + RotatedOffset * 1.5f, ObjectTypes, CollisionParams))
	{
		DesiredTarget = PlayerPawn->GetActorLocation() + RotatedOffset;
	}
}

void UBTTask_FollowWithAvoidance::UpdatePath(const TObjectPtr<ADroneAIController>& DroneAIController)
{
	FVector DroneLocation = DroneAIController->GetPawn()->GetActorLocation();
	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(DroneAIController->GetPawn());
	
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Visibility));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));
	
	if (GetWorld()->LineTraceSingleByObjectType(HitResult, DroneLocation, DesiredTarget, FCollisionObjectQueryParams(ObjectTypes), CollisionParams))
	{
		if (bCanFindPath && bEndFollowPath)
		{
			FindPath(DroneAIController);
		}
	}
}

void UBTTask_FollowWithAvoidance::FindPath(const TObjectPtr<ADroneAIController>& DroneAIController)
{
	TObjectPtr<AAOctreeVolume> CurOctreeVolume = DroneAIController->GetOctreeVolume();
	if (IsValid(CurOctreeVolume))
	{
		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Visibility));
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));

		FVector DroneLocation = DroneAIController->GetPawn()->GetActorLocation();
		PathPoints.Empty();
		if (CurOctreeVolume->FindPath(DroneLocation, DesiredTarget, ObjectTypes, AActor::StaticClass(), PathPoints))
		{
			bEndFollowPath = false;
			CurIndex = 0;
		}
	}
}

void UBTTask_FollowWithAvoidance::CanFindPath()
{
	bCanFindPath = true;
}

void UBTTask_FollowWithAvoidance::FollowPath(const TObjectPtr<ADroneAIController>& DroneAIController, float DeltaTime)
{
	FVector TargetLocation = FVector::ZeroVector;
	
	if (!bEndFollowPath && !PathPoints.IsEmpty())
	{
		if (CurIndex < PathPoints.Num())
		{
			if (FVector::DistSquared(DroneAIController->GetPawn()->GetActorLocation(), PathPoints[CurIndex]) < FMath::Square(NextNodeIgnoreRadius))
			{
				CurIndex++;
			}
			if (CurIndex < PathPoints.Num())
			{
				TargetLocation = PathPoints[CurIndex];
			}
			else
			{
				CanFindPath();
				bEndFollowPath = true;
			}
		}
	}
	else
	{
		TargetLocation = DesiredTarget;
	}

	DroneAIController->SetNewTargetLocation(TargetLocation);
	DroneAIController->ApplySmoothMovement(DeltaTime);
	DroneAIController->ApplyPIDControl(DeltaTime, !bEndFollowPath);
}

void UBTTask_FollowWithAvoidance::DrawDebugPath()
{
	for (const auto& Node : PathPoints)
	{
		DrawDebugSphere(GetWorld(), Node, 20, 20, FColor::Orange);
	}
}