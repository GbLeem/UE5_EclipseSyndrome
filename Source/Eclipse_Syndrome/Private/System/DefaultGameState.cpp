#include "System/DefaultGameState.h"

#include "Character/PlayerCharacter.h"
#include "Character/PlayerCharacterController.h"
#include "Drone/Drone.h"
#include "Drone/DroneAIController.h"
#include "Drone/DroneController.h"

#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

ADefaultGameState::ADefaultGameState()
	:CurrentInventoryAmmos(100)
	, bIsControllingDrone(false)
{
	static ConstructorHelpers::FClassFinder<ADroneController>DroneControllerClass(TEXT("/Game/SH/Blueprint/BP_DroneController.BP_DroneController_C"));
	if (DroneControllerClass.Succeeded())
	{
		DroneClass = DroneControllerClass.Class;
	}
}

int32 ADefaultGameState::GetAmmo() const
{
	return CurrentInventoryAmmos;
}

void ADefaultGameState::BeginPlay()
{
	Super::BeginPlay();

	UpdateHUD();

	GetWorldTimerManager().SetTimer
	(
		HUDUpdateTimerHandle
		, this
		, &ADefaultGameState::UpdateHUD
		, 0.1f
		, true
	);

	//for possess
	PlayerCharacterP = Cast<APawn>(UGameplayStatics::GetActorOfClass(GetWorld(), APlayerCharacter::StaticClass()));	
	Drone = Cast<APawn>(UGameplayStatics::GetActorOfClass(GetWorld(), ADrone::StaticClass()));
	PlayerCharacterControllerP = Cast<APlayerCharacterController>(GetWorld()->GetFirstPlayerController());

	if (Drone)
	{		
		DroneAIController = Cast<ADroneAIController>(Drone->GetController());	
		DroneController = GetWorld()->SpawnActor<ADroneController>(DroneClass);
		//DroneController = Cast<ADroneController>(UGameplayStatics::GetActorOfClass(GetWorld(), ADroneController::StaticClass()));		
		//DroneController->AutoReceiveInput = EAutoReceiveInput::Player0;
	}
}

void ADefaultGameState::UpdateHUD()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (APlayerCharacterController* PlayerCharacterController = Cast<APlayerCharacterController>(PlayerController))
		{
			if (UUserWidget* HUDWidget = PlayerCharacterController->GetHUDWidget())
			{
				if (UTextBlock* CurrentInventoryAmmoText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("TotalAmmoTxt"))))
				{
					CurrentInventoryAmmoText->SetText(FText::FromString(FString::Printf(TEXT(": %d"), CurrentInventoryAmmos)));
				}

				if (UTextBlock* CurrentAmmoText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("CurrentAmmoTxt"))))
				{
					if (APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(PlayerController->GetPawn()))
					{
						CurrentAmmoText->SetText(FText::FromString(FString::Printf(TEXT("%d"), PlayerCharacter->GetCurrentWeaponAmmo())));
					}
				}
			}
		}
	}
}

void ADefaultGameState::ToggleControl()
{
	if (bIsControllingDrone)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("Possess to player")));

		PlayerCharacterControllerP->Possess(PlayerCharacterP);
		ControlPlayer();
	}
	else
	{
		ControlDrone();		
	}
}

void ADefaultGameState::ControlPlayer()
{
	bIsControllingDrone = false;
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("Possess to player")));

	if (PlayerCharacterP)
	{
		PlayerCharacterControllerP->Possess(PlayerCharacterP);
	}
}

void ADefaultGameState::ControlDrone()
{
	bIsControllingDrone = true;
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("Possess to drone")));

	if (Drone)
	{		
		//DroneAIController->UnPossess();
		DroneAIController->Destroy();
		PlayerCharacterControllerP->UnPossess();		
		//DroneController = GetWorld()->SpawnActor<ADroneController>();

		DroneController->Possess(Drone);
		DroneController->AddMappingContextToPawn(Drone);
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, Drone->GetController()->GetName());		
	}
}
