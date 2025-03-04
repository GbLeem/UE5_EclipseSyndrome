#include "Drone/BehaviorTree/BTTask_CircleAroundPlayer.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Camera/CameraComponent.h"
#include "Character/PlayerCharacter.h"
#include "Drone/Drone.h"
#include "Drone/DroneAIController.h"
#include "Kismet/GameplayStatics.h"

UBTTask_CircleAroundPlayer::UBTTask_CircleAroundPlayer()
	: AccumulateAngle(0.0f)
{
	NodeName = "CircleAroundPlayer";
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_CircleAroundPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	TObjectPtr<AAIController> AIController = OwnerComp.GetAIOwner();
	if (AIController)
	{
		const TObjectPtr<AActor> Player = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject("PlayerActor"));
		const TObjectPtr<ADrone> Drone = Cast<ADrone>(AIController->GetPawn());

		if (Player && Drone)
		{
			AccumulateAngle = 0.0f;
			CalculateStartAngle(Player, AIController);
			return EBTNodeResult::InProgress;
		}
	}

	return EBTNodeResult::Failed;
}

void UBTTask_CircleAroundPlayer::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	if (OwnerComp.GetBlackboardComponent()->GetValueAsEnum("CurrentState") != 0)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	}

	IsIdleCircleMovementDone(OwnerComp);
	
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	TObjectPtr<AAIController> AIController = OwnerComp.GetAIOwner();
	if (AIController)
	{
		const TObjectPtr<AActor> Player = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject("PlayerActor"));
		const TObjectPtr<ADrone> Drone = Cast<ADrone>(AIController->GetPawn());

		if (Player && Drone)
		{
			UpdateIdleMovement(Player, AIController, OwnerComp.GetWorld()->GetDeltaSeconds());
			return;
		}
	}
	
	FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
}

void UBTTask_CircleAroundPlayer::UpdateIdleMovement(const TObjectPtr<AActor>& PlayerPawn, AAIController* AIController, float DeltaTime)
{
	const FVector PlayerLocation = PlayerPawn->GetActorLocation() + FVector(0.0f, 0.0f, 100.0f);
	const FVector CameraForward = Cast<APlayerCharacter>(PlayerPawn)->TPSCamera->GetForwardVector();

	ADroneAIController* DroneAIController = Cast<ADroneAIController>(AIController);
	
	float AngleSpeed = DroneAIController->GetRotationSpeed();
	float CurrentAngle = DroneAIController->GetCurrentAngle();
	const float CircleRadius = DroneAIController->GetCircleRadius();
	const float DistanceScaleFactor = DroneAIController->GetDistanceScaleFactor();
	const float HeightNoiseSpeed = DroneAIController->GetHeightNoiseSpeed();
	const float MaxHeightVariation = DroneAIController->GetMaxHeightVariation();

	const FVector Offset = FVector(
		FMath::Cos(CurrentAngle) * CircleRadius,
		FMath::Sin(CurrentAngle) * CircleRadius,
		0.0f
	);

	const FVector PlayerToDrone = Offset.GetSafeNormal();

	const float DotProduct = FVector::DotProduct(CameraForward, PlayerToDrone);
	float AdjustedRadius = CircleRadius;

	if (DotProduct > 0)
	{
		float RadiusFactor = (1.0f - DotProduct * DistanceScaleFactor);
		AdjustedRadius *= RadiusFactor;
		
		AngleSpeed /= FMath::Max(RadiusFactor, 0.1f);
	}
	
	FVector TargetLocation = PlayerLocation + Offset.GetSafeNormal() * AdjustedRadius;

	const float TimeScale = GetWorld()->GetTimeSeconds() * HeightNoiseSpeed;
	const float NoiseValue = FMath::PerlinNoise1D(TimeScale) * MaxHeightVariation;
	
	TargetLocation.Z += NoiseValue;

	float DeltaAngle = AngleSpeed * DeltaTime;
	CurrentAngle += DeltaAngle;
	AccumulateAngle += FMath::Abs(DeltaAngle);
	DroneAIController->SetCurrentAngle(CurrentAngle);
	DroneAIController->SetNewTargetLocation(TargetLocation);
	DroneAIController->ApplySmoothMovement(DeltaTime);
	DroneAIController->ApplyPIDControl(DeltaTime);
}

void UBTTask_CircleAroundPlayer::CalculateStartAngle(const TObjectPtr<AActor>& PlayerPawn, AAIController* AIController)
{
	ADroneAIController* DroneAIController = Cast<ADroneAIController>(AIController);
	const FVector DroneLocation = DroneAIController->GetPawn()->GetActorLocation();
	const FVector PlayerLocation = PlayerPawn->GetActorLocation() + FVector(0.0f, 0.0f, 100.0f);
	
	const FVector DirectionToPlayer = (DroneLocation - PlayerLocation).GetSafeNormal();
	float CurrentAngle = FMath::Atan2(DirectionToPlayer.Y, DirectionToPlayer.X);
	
	DroneAIController->SetCurrentAngle(CurrentAngle);
}

void UBTTask_CircleAroundPlayer::IsIdleCircleMovementDone(UBehaviorTreeComponent& OwnerComp)
{
	const TObjectPtr<APawn> Drone = OwnerComp.GetAIOwner()->GetPawn();
	const TObjectPtr<APawn> Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	const TObjectPtr<ADroneAIController> DroneAIController = Cast<ADroneAIController>(OwnerComp.GetAIOwner());
	if (AccumulateAngle >= 2 * PI
		&& FVector::Dist(Drone->GetActorLocation(),
			Player->GetActorLocation() + Player->GetActorRotation().RotateVector(DroneAIController->GetBaseDroneOffset())) <= 15.f)
	{
		AccumulateAngle = 0.0f;
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	}
}