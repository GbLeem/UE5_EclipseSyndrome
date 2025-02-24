#include "Character/PlayerCharacterController.h"

#include "Character/PlayerCharacter.h"
#include "Drone/Drone.h"
#include "Drone/DroneAIController.h"
#include "Drone/DroneController.h"
#include "System/DefaultGameState.h"

#include "AIController.h"
#include "Blueprint/UserWidget.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Kismet/GameplayStatics.h"

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
