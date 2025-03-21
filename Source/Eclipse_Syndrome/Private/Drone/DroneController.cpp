#include "Drone/DroneController.h"
#include "EnhancedInputSubsystems.h"

ADroneController::ADroneController()
	: InputMappingContext(nullptr)
	, MoveAction(nullptr)
	, LookAction(nullptr)
{
	
}

void ADroneController::AddMappingContextToPawn(APawn* InPawn)
{
    if (const TObjectPtr<ULocalPlayer> LocalPlayer = GetLocalPlayer())
    {
        GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Black, LocalPlayer->GetName());

        if (const TObjectPtr<UEnhancedInputLocalPlayerSubsystem> SubSystem =
            LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
        {
            if (nullptr != InputMappingContext)
            {
                GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Black, FString::Printf(TEXT("drone controller mapping")));

                SubSystem->AddMappingContext(InputMappingContext, 0);
            }
        }
    }
}

void ADroneController::BeginPlay()
{
	Super::BeginPlay();
    
}