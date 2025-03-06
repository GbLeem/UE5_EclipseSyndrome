#include "Character/PlayerCharacterController.h"

#include "Character/PlayerCharacter.h"
#include "Drone/Drone.h"
#include "Drone/DroneAIController.h"
#include "System/DefaultGameState.h"
#include "System/DefaultGameInstance.h"

#include "AIController.h"
#include "Blueprint/UserWidget.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Kismet/GameplayStatics.h"

APlayerCharacterController::APlayerCharacterController()
	:bIsInventoryUIOpen(false)
{
	//player IMC & IA setting
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
	static ConstructorHelpers::FObjectFinder<UInputAction> IA_ShowInventory(TEXT("/Game/HJ/Input/IA_ShowInventoryUI.IA_ShowInventoryUI"));
	if (IA_ShowInventory.Succeeded())
	{
		ShowInventoryAction = IA_ShowInventory.Object;
	}
	static ConstructorHelpers::FObjectFinder<UInputAction> IA_ChangeView(TEXT("/Game/HJ/Input/IA_ViewChange.IA_ViewChange"));
	if (IA_ChangeView.Succeeded())
	{
		ViewChangeAction = IA_ChangeView.Object;
	}
	static ConstructorHelpers::FObjectFinder<UInputAction> IA_Zoom(TEXT("/Game/HJ/Input/IA_Zoom.IA_Zoom"));
	if (IA_Zoom.Succeeded())
	{
		ZoomAction = IA_Zoom.Object;
	}
	static ConstructorHelpers::FObjectFinder<UInputAction> IA_Crouch(TEXT("/Game/HJ/Input/IA_Crouch.IA_Crouch"));
	if (IA_Crouch.Succeeded())
	{
		CrouchAction = IA_Crouch.Object;
	}

	//for drone possess
	static ConstructorHelpers::FObjectFinder<UInputAction> IA_DroneMoveCommand(TEXT("/Game/HJ/Input/IA_DroneMoveCommand.IA_DroneMoveCommand"));
	if (IA_DroneMoveCommand.Succeeded())
	{
		DroneMoveCommandAction = IA_DroneMoveCommand.Object;
	}
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> IMC_Drone(TEXT("/Game/SH/Input/IMC_Drone.IMC_Drone"));
	if (IMC_Drone.Succeeded())
	{
		DroneInputMappingContext = IMC_Drone.Object;
	}
	static ConstructorHelpers::FObjectFinder<UInputAction> IA_DroneMove(TEXT("/Game/SH/Input/IA_DroneMove.IA_DroneMove"));
	if (IA_DroneMove.Succeeded())
	{
		DroneMoveAction = IA_DroneMove.Object;
	}
	static ConstructorHelpers::FObjectFinder<UInputAction> IA_DroneLook(TEXT("/Game/SH/Input/IA_DroneLook.IA_DroneLook"));
	if (IA_DroneLook.Succeeded())
	{
		DroneLookAction = IA_DroneLook.Object;
	}
	static ConstructorHelpers::FObjectFinder<UInputAction> IA_DronePossess(TEXT("/Game/SH/Input/IA_DroneToPlayer.IA_DroneToPlayer"));
	if (IA_DronePossess.Succeeded())
	{
		DronePossessAction = IA_DronePossess.Object;
	}
	static ConstructorHelpers::FClassFinder<ADroneAIController> DroneAIControllerClass(TEXT("/Game/SH/Blueprint/BP_DroneAIController.BP_DroneAIController_C"));
	if (DroneAIControllerClass.Succeeded())
	{
		DroneAIClass = DroneAIControllerClass.Class;
	}

	//ui BP setting
	static ConstructorHelpers::FClassFinder<UUserWidget>HUDWidgetBP(TEXT("/Game/HJ/UI/WBP_HUD.WBP_HUD_C"));
	if (HUDWidgetBP.Succeeded())
	{	
		HUDWidgetClass = HUDWidgetBP.Class;
	}
	static ConstructorHelpers::FClassFinder<UUserWidget>InventoryWidgetBP(TEXT("/Game/HJ/UI/WBP_Inventory.WBP_Inventory_C"));
	if (InventoryWidgetBP.Succeeded())
	{
		InventoryUIClass = InventoryWidgetBP.Class;
	}
	static ConstructorHelpers::FClassFinder<UUserWidget>MainMenuWidgetBP(TEXT("/Game/HJ/UI/WBP_MainMenu.WBP_MainMenu_C"));
	if (MainMenuWidgetBP.Succeeded())
	{
		MainMenuClass = MainMenuWidgetBP.Class;
	}
	static ConstructorHelpers::FClassFinder<UUserWidget>GameOverWidgetBP(TEXT("/Game/HJ/UI/WBP_GameOver.WBP_GameOver_C"));
	if (GameOverWidgetBP.Succeeded())
	{
		GameOverUIClass = GameOverWidgetBP.Class;
	}
}

void APlayerCharacterController::ShowHUD()
{
	if (MainMenuInstance)
	{
		MainMenuInstance->RemoveFromParent();
		MainMenuInstance = nullptr;
	}

	if (HUDWidgetInstance)
	{
		HUDWidgetInstance->RemoveFromParent();
		HUDWidgetInstance = nullptr;	
	}

	if (InventoryUIInstance)
	{
		InventoryUIInstance->RemoveFromParent();
		InventoryUIInstance = nullptr;
	}

	if (HUDWidgetClass)
	{
		HUDWidgetInstance = CreateWidget<UUserWidget>(this, HUDWidgetClass);
		if (HUDWidgetInstance)
		{
			HUDWidgetInstance->AddToViewport();

			bShowMouseCursor = false;
			SetInputMode(FInputModeGameOnly());

			ADefaultGameState* DefaultGameState = GetWorld() ? GetWorld()->GetGameState<ADefaultGameState>() : nullptr;
			if (DefaultGameState)
			{
				DefaultGameState->UpdateHUD();
			}
		}
	}
}

void APlayerCharacterController::ShowMainMenu()
{
	if (HUDWidgetInstance)
	{
		HUDWidgetInstance->RemoveFromParent();
		HUDWidgetInstance = nullptr;
	}
	if (MainMenuInstance)
	{
		MainMenuInstance->RemoveFromParent();
		MainMenuInstance = nullptr;
	}

	if (MainMenuClass)
	{
		MainMenuInstance = CreateWidget<UUserWidget>(this, MainMenuClass);

		if (MainMenuInstance)
		{
			MainMenuInstance->AddToViewport();
			
			bShowMouseCursor = true;
			SetInputMode(FInputModeUIOnly());
		}
	}
}

void APlayerCharacterController::ShowGameOverUI()
{
	if (HUDWidgetInstance)
	{
		HUDWidgetInstance->RemoveFromParent();
		HUDWidgetInstance = nullptr;
	}
	if (MainMenuInstance)
	{
		MainMenuInstance->RemoveFromParent();
		MainMenuInstance = nullptr;
	}
	//SetPause(true);
	if (GameOverUIClass)
	{
		GameOverUIInstance = CreateWidget<UUserWidget>(this, GameOverUIClass);

		GameOverUIInstance->AddToViewport();
		bShowMouseCursor = true;
		SetInputMode(FInputModeUIOnly());

		UFunction* GameOverUIAnim = GameOverUIInstance->FindFunction(FName("GameOverAnim"));
		if (GameOverUIAnim)
		{
			GameOverUIInstance->ProcessEvent(GameOverUIAnim, nullptr);
		}
		SetPause(true);
	}
}

void APlayerCharacterController::ShowGameClearUI()
{
	SetPause(true);
}

void APlayerCharacterController::StartGame()
{
	if (UDefaultGameInstance* DefaultGameInstance = Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(this)))
	{
		DefaultGameInstance->InitializeInstance();		
	}
	
	ADefaultGameState* DefaultGameState = Cast<ADefaultGameState>(GetWorld()->GetGameState());
	if (DefaultGameState)
	{
		DefaultGameState->UpdateHUD();
	}

	UGameplayStatics::OpenLevel(GetWorld(), FName("Lv1"));
	SetPause(false);
}

void APlayerCharacterController::ShowInventoryUI()
{
	if (InventoryUIClass && !InventoryUIInstance)
	{
		InventoryUIInstance = CreateWidget<UUserWidget>(this, InventoryUIClass);
	}

	if (InventoryUIInstance && !InventoryUIInstance->IsInViewport())
	{
		InventoryUIInstance->AddToViewport();
		bShowMouseCursor = true;		
		bIsInventoryUIOpen = true;
		SetInputMode(FInputModeGameAndUI());		
	}	
}

void APlayerCharacterController::StopShowInventoryUI()
{
	//if (!bIsInventoryUIOpen)
	//{
		//if (InventoryUIInstance && InventoryUIInstance->IsInViewport())
		//{
			InventoryUIInstance->RemoveFromParent();
			InventoryUIInstance = nullptr;

			bShowMouseCursor = false;
			SetInputMode(FInputModeGameOnly());

			bIsInventoryUIOpen = false;
		//}
	//}	
}

//void APlayerCharacterController::PlayDamageAnimation()
//{
//	UFunction* DamageAnim = 
//}

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

	FString CurrentMapName = GetWorld()->GetMapName();
	if (CurrentMapName.Contains("MainMenuLevel"))
	{
		//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString::Printf(TEXT("menu map")));
		ShowMainMenu();
	}

	//ShowHUD();	
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