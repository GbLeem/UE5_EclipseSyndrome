#include "System/DefaultGameState.h"

#include "Character/PlayerCharacter.h"
#include "Character/PlayerCharacterController.h"

#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"

ADefaultGameState::ADefaultGameState()
	:CurrentInventoryAmmos(100)
{
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
