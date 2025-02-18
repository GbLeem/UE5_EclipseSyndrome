#include "Drone/DroneController.h"
#include "EnhancedInputSubsystems.h"

ADroneController::ADroneController()
	: InputMappingContext(nullptr)
	, MoveAction(nullptr)
	, LookAction(nullptr)
{
}

void ADroneController::BeginPlay()
{
	Super::BeginPlay();

	if (const TObjectPtr<ULocalPlayer> LocalPlayer = GetLocalPlayer())
	{
		if (const TObjectPtr<UEnhancedInputLocalPlayerSubsystem> SubSystem =
			LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (nullptr != InputMappingContext)
			{
				SubSystem->AddMappingContext(InputMappingContext, 0);
			}
		}
	}
}