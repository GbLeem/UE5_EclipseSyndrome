#include "Drone/BehaviorTree/BTTask_HappyMovement.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Drone/Drone.h"
#include "Drone/DroneAIController.h"
#include "Kismet/GameplayStatics.h"

UBTTask_HappyMovement::UBTTask_HappyMovement()
	: CurrentTime(0.0f)
	, PrevDroneAIDesiredDistance(0.0f)
{
	NodeName = "HappyMovement";
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_HappyMovement::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	CurrentTime = 0.0f;
	const TObjectPtr<ADroneAIController> AIDroneController = Cast<ADroneAIController>(OwnerComp.GetAIOwner());
	PrevDroneAIDesiredDistance = AIDroneController->GetDesiredDistance();
	AIDroneController->SetDesiredDistance(0.0f);
	AIDroneController->SetPIDMaxSpeed(150000.0f);
	return EBTNodeResult::InProgress;
}

void UBTTask_HappyMovement::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	if (OwnerComp.GetBlackboardComponent()->GetValueAsEnum("CurrentState") != 0
		|| CurrentTime > 5.0f)
		EndTask(OwnerComp);
		
	
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	const TObjectPtr<APawn> ControlledPawn = OwnerComp.GetAIOwner()->GetPawn();
	const TObjectPtr<APawn> PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

	if (ControlledPawn && PlayerPawn)
	{
		UpdateHappyMovement(OwnerComp.GetAIOwner(), ControlledPawn, PlayerPawn, DeltaSeconds);
	}

	CurrentTime += DeltaSeconds;
}

void UBTTask_HappyMovement::UpdateHappyMovement(const TObjectPtr<AAIController>& AIController, const TObjectPtr<APawn>& ControlledPawn, const TObjectPtr<APawn>& PlayerPawn, float DeltaTime)
{
	const TObjectPtr<ADrone> ControlledDrone = Cast<ADrone>(ControlledPawn);
	if (!ControlledDrone) return;

	const TObjectPtr<APawn> Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!Player) return;

	const TObjectPtr<ADroneAIController> AIDroneController = Cast<ADroneAIController>(AIController);
	if (!AIDroneController) return;

	const FVector RotatedOffset = PlayerPawn->GetActorRotation().RotateVector(AIDroneController->GetBaseDroneOffset());
	const FVector PlayerLocation = Player->GetActorLocation() + RotatedOffset;

	const float Time = GetWorld()->GetTimeSeconds();

	float BounceHeight = FMath::Abs(FMath::Sin(Time * 1.0f)) * 40.0f;

	float EaseFactor = (FMath::Cos(Time * 2.0f) + 1.0f) / 2.0f;

	// FVector RandomShake = FVector(
	// 	FMath::Sin(Time * 2.0f + FMath::RandRange(-0.5f, 0.5f)) * 2.0f,
	// 	FMath::Cos(Time * 2.0f + FMath::RandRange(-0.5f, 0.5f)) * 2.0f,
	// 	0.0f
	// );
	//RandomShake
	
	FVector TargetLocations = PlayerLocation + FVector(0.0f, 0.0f, BounceHeight * EaseFactor);
	
	float TiltAngle = FMath::Sin(Time * 3.0f) * 15.0f;
	FRotator NewTiltRotation = ControlledDrone->GetCameraSceneComponent()->GetRelativeRotation();
	NewTiltRotation.Roll = FMath::FInterpTo(NewTiltRotation.Roll, TiltAngle, DeltaTime, 8.0f);
	ControlledDrone->GetCameraSceneComponent()->SetRelativeRotation(NewTiltRotation);
	
	AIDroneController->SetNewTargetLocation(TargetLocations);
	AIDroneController->ApplySmoothMovement(DeltaTime);
	AIDroneController->ApplyPIDControl(DeltaTime, true);
}

void UBTTask_HappyMovement::EndTask(UBehaviorTreeComponent& OwnerComp)
{
	const TObjectPtr<ADroneAIController> AIDroneController = Cast<ADroneAIController>(OwnerComp.GetAIOwner());
	AIDroneController->SetDesiredDistance(PrevDroneAIDesiredDistance);
	
	FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
}
