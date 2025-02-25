#include "Character/PlayerCharacterController.h"

#include "Character/PlayerCharacter.h"
#include "Drone/Drone.h"
#include "Drone/DroneAIController.h"
#include "System/DefaultGameState.h"

#include "AIController.h"
#include "Blueprint/UserWidget.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"

APlayerCharacterController::APlayerCharacterController()
	:DefaultInputMappingContext(nullptr)
	,MoveAction(nullptr)
	,LookAction(nullptr)
	,JumpAction(nullptr)
	,SprintAction(nullptr)
	,ShootAction(nullptr)
	,ReloadAction(nullptr)
	,PickUpAction(nullptr)
{
	//IMC & IA setting
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> IMC_Default(TEXT("/Game/HJ/Input/IMC_Default.IMC_Default"));
	if (IMC_Default.Succeeded())
	{
		DefaultInputMappingContext = IMC_Default.Object;
	}
	static ConstructorHelpers::FObjectFinder<UInputAction> IA_Move(TEXT("/Game/HJ/Input/IA_Move.IA_Move"));
	if (IA_Move.Succeeded())
	{
		MoveAction = IA_Move.Object;
	}
	static ConstructorHelpers::FObjectFinder<UInputAction> IA_Look(TEXT("/Game/HJ/Input/IA_Look.IA_Look"));
	if (IA_Look.Succeeded())
	{
		LookAction = IA_Look.Object;
	}
	static ConstructorHelpers::FObjectFinder<UInputAction> IA_Jump(TEXT("/Game/HJ/Input/IA_Jump.IA_Jump"));
	if (IA_Jump.Succeeded())
	{
		JumpAction = IA_Jump.Object;
	}
	static ConstructorHelpers::FObjectFinder<UInputAction> IA_Sprint(TEXT("/Game/HJ/Input/IA_Sprint.IA_Sprint"));
	if (IA_Sprint.Succeeded())
	{
		SprintAction = IA_Sprint.Object;
	}
	static ConstructorHelpers::FObjectFinder<UInputAction> IA_Shoot(TEXT("/Game/HJ/Input/IA_Shoot.IA_Shoot"));
	if (IA_Shoot.Succeeded())
	{
		ShootAction = IA_Shoot.Object;
	}
	static ConstructorHelpers::FObjectFinder<UInputAction> IA_Reload(TEXT("/Game/HJ/Input/IA_Reload.IA_Reload"));
	if (IA_Reload.Succeeded())
	{
		ReloadAction = IA_Reload.Object;
	}
	static ConstructorHelpers::FObjectFinder<UInputAction> IA_PickUp(TEXT("/Game/HJ/Input/IA_PickUp.IA_PickUp"));
	if (IA_PickUp.Succeeded())
	{
		PickUpAction = IA_PickUp.Object;
	}
	static ConstructorHelpers::FObjectFinder<UInputAction> IA_EquipWeapon1(TEXT("/Game/HJ/Input/IA_EquipWeapon1.IA_EquipWeapon1"));
	if (IA_EquipWeapon1.Succeeded())
	{
		EquipWeapon1Action = IA_EquipWeapon1.Object;
	}	
	static ConstructorHelpers::FObjectFinder<UInputAction> IA_Grapple(TEXT("/Game/HJ/Input/IA_Grapple.IA_Grapple"));
	if (IA_Grapple.Succeeded())
	{
		GrappleAction = IA_Grapple.Object;
	}
	static ConstructorHelpers::FObjectFinder<UInputAction> IA_PossessToDrone(TEXT("/Game/HJ/Input/IA_PossessToDrone.IA_PossessToDrone"));
	if (IA_PossessToDrone.Succeeded())
	{
		PossessAction = IA_PossessToDrone.Object;
	}


	static ConstructorHelpers::FClassFinder<UUserWidget>HUDWidget(TEXT("/Game/HJ/UI/WBP_HUD.WBP_HUD_C"));
	if (HUDWidget.Succeeded())
	{
		HUDWidgetClass = HUDWidget.Class;
	}

}

void APlayerCharacterController::BeginPlay()
{
	Super::BeginPlay();

	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* SubSystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (DefaultInputMappingContext)
			{
				SubSystem->AddMappingContext(DefaultInputMappingContext, 0);
			}
		}
	}

	if (HUDWidgetClass)
	{
		HUDWidgetInstance = CreateWidget<UUserWidget>(this, HUDWidgetClass);
		if (HUDWidgetInstance)
		{
			HUDWidgetInstance->AddToViewport();
		}
	}

	ADefaultGameState* DefaultGameState = GetWorld() ? GetWorld()->GetGameState<ADefaultGameState>() : nullptr;
	if (DefaultGameState)
	{
		DefaultGameState->UpdateHUD();
	}
}

void APlayerCharacterController::ChangePossess(const TObjectPtr<APawn>& NewPawn)
{
	if (!NewPawn)
	{
		return;
	}

	const TObjectPtr<APawn> PrevPawn = GetPawn();
	UnPossess();

	if (const TObjectPtr<ADrone> DronePawn = Cast<ADrone>(NewPawn))
	{
		HandleDronePossess(DronePawn, PrevPawn);
	}
	else if (const TObjectPtr<APlayerCharacter> NewPlayerPawn = Cast<APlayerCharacter>(NewPawn))
	{
		HandlePlayerPossess(NewPlayerPawn, PrevPawn);
	}
}

void APlayerCharacterController::ChangeMappingContext(int Type)
{
	if (const ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* SubSystem =
			LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			SubSystem->ClearAllMappings();
			// Character
			if (Type == 0)
			{
				if (nullptr != DefaultInputMappingContext)
				{
					SubSystem->AddMappingContext(DefaultInputMappingContext, 0);
				}
			}
			// Drone
			else if (Type == 1)
			{
				if (nullptr != DroneInputMappingContext)
				{
					SubSystem->AddMappingContext(DroneInputMappingContext, 1);
				}
			}
		}
	}
}

void APlayerCharacterController::HandleDronePossess(const TObjectPtr<ADrone>& DronePawn, const TObjectPtr<APawn>& PrevPawn)
{
	if (const TObjectPtr<AController>& PrevController = DronePawn->GetController())
	{
		if (const TObjectPtr<ADroneAIController>& PrevDroneAIController = Cast<ADroneAIController>(PrevController))
		{
			PrevDroneAIController->UnPossess();
			PrevDroneAIController->Destroy();
		}
	}

	Possess(DronePawn);
	DronePawn->SetEnhancedInput();

	if (APlayerCharacter* PrevPlayer = Cast<APlayerCharacter>(PrevPawn))
	{
		SpawnDummyAIForPawn(PrevPlayer);
	}
}

void APlayerCharacterController::HandlePlayerPossess(const TObjectPtr<APlayerCharacter>& NewPlayerPawn, const TObjectPtr<APawn>& PrevPawn)
{
	if (const TObjectPtr<AController>& PrevController = NewPlayerPawn->GetController())
	{
		PrevController->UnPossess();
		PrevController->Destroy();
	}

	if (ADroneAIController* NewAIController = GetWorld()->SpawnActor<ADroneAIController>(DroneAIClass))
	{
		NewAIController->Possess(PrevPawn);
	}

	Possess(NewPlayerPawn);
	NewPlayerPawn->SetEnhancedInput();
}

void APlayerCharacterController::SpawnDummyAIForPawn(const TObjectPtr<APlayerCharacter>& PrevPlayerPawn)
{
	if (TObjectPtr<AAIController> DummyAIController = GetWorld()->SpawnActor<AAIController>(AAIController::StaticClass()))
	{
		DummyAIController->Possess(PrevPlayerPawn);
	}
}


// void APlayerCharacterController::ChangePossess(const TObjectPtr<APawn>& NewPawn)
// {
// 	if (nullptr != NewPawn)
// 	{
// 		TObjectPtr<APawn> PrevPawn = GetPawn();
// 		UnPossess(); // 기존 Pawn에서 빙의 해제
//
// 		if (TObjectPtr<ADrone> DronePawn = Cast<ADrone>(NewPawn))
// 		{
// 			// 이전 Drone의 AI 컨트롤러를 UnPossess하고 삭제
// 			if (const TObjectPtr<AController>& PrevController = NewPawn->GetController())
// 			{
// 				if (const TObjectPtr<ADroneAIController>& PrevDroneAIController = Cast<ADroneAIController>(PrevController))
// 				{
// 					PrevDroneAIController->UnPossess();
// 					PrevDroneAIController->Destroy();
// 				}
// 			}
//
// 			// DronePawn에 대해 새로운 Possess 호출
// 			Possess(NewPawn);
// 			DronePawn->SetEnhancedInput();
//
// 			if (APlayerCharacter* PrevPlayer = Cast<APlayerCharacter>(PrevPawn))
// 			{
// 				if (TObjectPtr<AAIController> DummyAIController = GetWorld()->SpawnActor<AAIController>(AAIController::StaticClass()))
// 				{
// 					DummyAIController->Possess(PrevPlayer); // 플레이어 캐릭터를 AI에 맡김
// 				}
// 			}
// 		}
// 		else if (TObjectPtr<APlayerCharacter> NewPlayerPawn = Cast<APlayerCharacter>(NewPawn))
// 		{
// 			// 이전 Player의 AI 컨트롤러를 UnPossess하고 삭제
// 			if (const TObjectPtr<AController>& PrevController = NewPawn->GetController())
// 			{
// 				PrevController->UnPossess();
// 				PrevController->Destroy();
// 			}
// 			
// 			// AI 컨트롤러를 새로 생성하여 PlayerPawn에 대한 Possess 호출
// 			ADroneAIController* NewAIController = GetWorld()->SpawnActor<ADroneAIController>(DroneAIClass);
// 			if (NewAIController)
// 			{
// 				NewAIController->Possess(PrevPawn); // 이전 PlayerPawn을 AI 컨트롤러로 Possess
// 			}
//
// 			// PlayerPawn에 대해서 Possess 호출
// 			Possess(NewPawn);
// 			NewPlayerPawn->SetEnhancedInput();
// 		}
// 	}
// }