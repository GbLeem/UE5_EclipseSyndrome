#include "Character/PlayerCharacterController.h"

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
}
