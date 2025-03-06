#include "Drone/BehaviorTree/BTTask_DroneAttack.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Drone/Drone.h"
#include "Drone/DroneAIController.h"
#include "Enemy/EnemyBase.h"
#include "Volume/AOctreeVolume.h"


UBTTask_DroneAttack::UBTTask_DroneAttack()
{
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_DroneAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return EBTNodeResult::Failed;

	ADrone* Drone = Cast<ADrone>(AIController->GetPawn());
	if (!Drone) return EBTNodeResult::Failed;

	AEnemyBase* Target = Cast<AEnemyBase>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TEXT("TargetEnemy")));
	if (!Target || !IsValid(Target)) return EBTNodeResult::Failed;
	
	if (PrevTarget != Target || bEndFollowPath)
	{
		CanFindPath();
		PathTimerHandle.Invalidate();
		bEndFollowPath = true;
		CurIndex = 0;
		PathPoints.Empty();
	}

	PrevTarget = Target;
	
	return EBTNodeResult::InProgress;
}

void UBTTask_DroneAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return;

	ADrone* Drone = Cast<ADrone>(AIController->GetPawn());
	AEnemyBase* Target = Cast<AEnemyBase>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TEXT("TargetEnemy")));

	if (!Drone || !Target || !IsValid(Target))
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsInt(TEXT("AttackType"), 0);
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	FVector DroneLocation = Drone->GetActorLocation();
	FVector TargetLocation = Target->GetActorLocation();
	float DistanceToTarget = FVector::Dist(DroneLocation, TargetLocation);
	
	if (DistanceToTarget <= Drone->GetAttackRange())
	{
		if (Target && Drone)
		{
			TObjectPtr<ADroneAIController> DroneAIController = Cast<ADroneAIController>(AIController);
			UpdateDesiredTarget(Target, DroneAIController);
			if (DroneAIController->GetShowDebug())
			{
				DrawDebugSphere(GetWorld(), DesiredTarget, 30, 30, FColor::Silver);
			}

			UpdatePath(DroneAIController);
			FollowPath(DroneAIController, DeltaSeconds);
			
			if (DroneAIController->GetShowDebug())
			{
				DrawDebugPath();
			}
		}
		if (!GetWorld()->GetTimerManager().IsTimerActive(AttackCooldownTimer))
		{
			Drone->Attack(Target);
			GetWorld()->GetTimerManager().SetTimer(AttackCooldownTimer, [](){}, Drone->GetAttackCooldown(), false);
			OwnerComp.GetBlackboardComponent()->SetValueAsInt(TEXT("AttackType"), 2);
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
			return;
		}
	}

	OwnerComp.GetBlackboardComponent()->SetValueAsInt(TEXT("AttackType"), 2);
	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
}

void UBTTask_DroneAttack::UpdateDesiredTarget(const TObjectPtr<AActor>& TargetPawn,
	const TObjectPtr<ADroneAIController>& DroneAIController)
{
	if (!TargetPawn) return;
	const FVector RotatedOffset = TargetPawn->GetActorRotation().RotateVector(DroneAIController->GetBaseDroneOffset());
	FHitResult Hit;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(TargetPawn);
	CollisionParams.AddIgnoredActor(DroneAIController->GetPawn());
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	//ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Visibility));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_GameTraceChannel1));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_GameTraceChannel2));
	if (!GetWorld()->LineTraceSingleByObjectType(Hit, TargetPawn->GetActorLocation() + FVector(0.0f, 0.0f, 200.0f), TargetPawn->GetActorLocation() + RotatedOffset * 1.5f, ObjectTypes, CollisionParams))
	{
		DesiredTarget = TargetPawn->GetActorLocation() + RotatedOffset + FVector(0.0f, 0.0f, 100.0f);
	}
}

void UBTTask_DroneAttack::UpdatePath(const TObjectPtr<ADroneAIController>& DroneAIController)
{
	FVector DroneLocation = DroneAIController->GetPawn()->GetActorLocation();
	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(DroneAIController->GetPawn());
	
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	//ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Visibility));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_GameTraceChannel1));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_GameTraceChannel2));
	
	if (GetWorld()->LineTraceSingleByObjectType(HitResult, DroneLocation, DesiredTarget, FCollisionObjectQueryParams(ObjectTypes), CollisionParams))
	{
		if (bCanFindPath && bEndFollowPath)
		{
			FindPath(DroneAIController);
		}
	}
}

void UBTTask_DroneAttack::FindPath(const TObjectPtr<ADroneAIController>& DroneAIController)
{
	TObjectPtr<AAOctreeVolume> CurOctreeVolume = DroneAIController->GetOctreeVolume();
	if (IsValid(CurOctreeVolume))
	{
		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
		//ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Visibility));
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_GameTraceChannel1));
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_GameTraceChannel2));

		FVector DroneLocation = DroneAIController->GetPawn()->GetActorLocation();
		//PathPoints.Empty();
		if (CurOctreeVolume->FindPath(DroneLocation, DesiredTarget, ObjectTypes, AActor::StaticClass(), PathPoints))
		{
			bEndFollowPath = false;
			CurIndex = 0;
		}
		else
		{
			bCanFindPath = false;
			GetWorld()->GetTimerManager().SetTimer(PathTimerHandle, this, &UBTTask_DroneAttack::CanFindPath, 1.0f, false);
		}
	}
}

void UBTTask_DroneAttack::FollowPath(const TObjectPtr<ADroneAIController>& DroneAIController, float DeltaTime)
{
	FVector TargetLocation = FVector::ZeroVector;
	
	if (!bEndFollowPath && !PathPoints.IsEmpty())
	{
		if (CurIndex < PathPoints.Num())
		{
			if (FVector::DistSquared(DroneAIController->GetPawn()->GetActorLocation(), PathPoints[CurIndex]) < FMath::Square(80))
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

void UBTTask_DroneAttack::DrawDebugPath()
{
	for (const auto& Node : PathPoints)
	{
		DrawDebugSphere(GetWorld(), Node, 20, 20, FColor::Orange);
	}
}