#include "Drone/BehaviorTree/BTTask_MoveToCommand.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Components/CapsuleComponent.h"
#include "Drone/Drone.h"
#include "Drone/DroneAIController.h"
#include "Volume/AOctreeVolume.h"

UBTTask_MoveToCommand::UBTTask_MoveToCommand()
	: PrevTargetLocation(FVector::ZeroVector)
	, DesiredTarget(FVector::ZeroVector)
	, bCanFindPath(true)
	, bEndFollowPath(true)
	, CurIndex(0)
	, NextNodeIgnoreRadius(80.0f)
{
	bNotifyTick = true;
	NodeName = "MoveToCommand";
}

EBTNodeResult::Type UBTTask_MoveToCommand::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	return EBTNodeResult::InProgress;
}

void UBTTask_MoveToCommand::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	if (OwnerComp.GetBlackboardComponent()->GetValueAsEnum("CurrentState") != 2)
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	if (const TObjectPtr<AAIController> AIController = OwnerComp.GetAIOwner())
	{
		if (const TObjectPtr<ADrone> Drone = Cast<ADrone>(AIController->GetPawn()))
		{
			TObjectPtr<ADroneAIController> DroneAIController = Cast<ADroneAIController>(AIController);
			UpdateDesiredTarget(OwnerComp, Drone);
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

			// 드론의 속도가 거의 0일 때 행동 완료
			constexpr float SpeedThreshold = 20.0f; // 적절한 임계값
			if (FVector::Dist(DesiredTarget, Drone->GetActorLocation()) < 50.0f
				&& Drone->GetVelocity().Size() <= SpeedThreshold)
			{
				OwnerComp.GetBlackboardComponent()->SetValueAsEnum("CurrentState", 3);
				FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
				return;
			}
			
			return;
		}
	}
	
	// 드론 없으면 실패 처리
	FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
}

void UBTTask_MoveToCommand::UpdateDesiredTarget(const UBehaviorTreeComponent& OwnerComp, const TObjectPtr<ADrone>& Drone)
{
	const FVector CommandLocation = OwnerComp.GetBlackboardComponent()->GetValueAsVector("MoveCommandLocation");
	if (PrevTargetLocation != CommandLocation)
	{
		CanFindPath();
		bEndFollowPath = true;
	}
	PrevTargetLocation = CommandLocation;
	if (!Drone) return;
	
	DesiredTarget = CommandLocation;
}

void UBTTask_MoveToCommand::UpdatePath(const TObjectPtr<ADroneAIController>& DroneAIController)
{
	FVector DroneLocation = DroneAIController->GetPawn()->GetActorLocation();
	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(DroneAIController->GetPawn());
	
	if (GetWorld()->LineTraceSingleByChannel(HitResult, DroneLocation, DesiredTarget, ECC_Visibility, CollisionParams))
	{
		if (bCanFindPath && bEndFollowPath)
		{
			FindPath(DroneAIController);
		}
	}
}

void UBTTask_MoveToCommand::FindPath(const TObjectPtr<ADroneAIController>& DroneAIController)
{
	TObjectPtr<AAOctreeVolume> CurOctreeVolume = DroneAIController->GetOctreeVolume();
	if (IsValid(CurOctreeVolume))
	{
		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Visibility));
		
		FVector DroneLocation = DroneAIController->GetPawn()->GetActorLocation();
		PathPoints.Empty();
		if (CurOctreeVolume->FindPath(DroneLocation, DesiredTarget, ObjectTypes, AActor::StaticClass(), PathPoints))
		{
			bEndFollowPath = false;
			bCanFindPath = false;
			CurIndex = 0;
		}
	}
}

void UBTTask_MoveToCommand::CanFindPath()
{
	bCanFindPath = true;
}

void UBTTask_MoveToCommand::FollowPath(const TObjectPtr<ADroneAIController>& DroneAIController, float DeltaTime)
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

void UBTTask_MoveToCommand::DrawDebugPath()
{
	for (const auto& Node : PathPoints)
	{
		DrawDebugSphere(GetWorld(), Node, 20, 20, FColor::Orange);
	}
}
