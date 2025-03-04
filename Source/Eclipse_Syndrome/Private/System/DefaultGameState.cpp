#include "System/DefaultGameState.h"

#include "Character/PlayerCharacter.h"
#include "Character/PlayerCharacterController.h"
#include "System/DefaultGameInstance.h"

#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/Button.h"

ADefaultGameState::ADefaultGameState()
{
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
					if (UGameInstance* GameInstance = GetGameInstance())
					{
						if (UDefaultGameInstance* DefaultGameInstance = Cast<UDefaultGameInstance>(GameInstance))
						{
							CurrentInventoryAmmoText->SetText(FText::FromString(FString::Printf(TEXT("| %d"), DefaultGameInstance->InventoryAmmo)));						
						}
					}
				}

				if (UTextBlock* CurrentAmmoText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("CurrentAmmoTxt"))))
				{
					if (APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(PlayerController->GetPawn()))
					{
						CurrentAmmoText->SetText(FText::FromString(FString::Printf(TEXT("%d"), PlayerCharacter->GetCurrentWeaponAmmo())));
					}
				}

				if (UProgressBar* HealthBar = Cast<UProgressBar>(HUDWidget->GetWidgetFromName(TEXT("HealthBar"))))
				{
					if (UGameInstance* GameInstance = GetGameInstance())
					{
						if (UDefaultGameInstance* DefaultGameInstance = Cast<UDefaultGameInstance>(GameInstance))
						{
							float MaxHealth = DefaultGameInstance->PlayerMaxHealth;
							float CurrentHealth = DefaultGameInstance->PlayerCurrentHealth;
							//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString::Printf(TEXT("%f / %f"), CurrentHealth, MaxHealth));
							HealthBar->SetPercent(CurrentHealth / MaxHealth);
						}
					}

				}
			}

			//Inventory
			if (UUserWidget* InventoryWidget = PlayerCharacterController->GetInventoryWidget())
			{
				//Weapon UI
				if (UImage* WeaponUI1Image = Cast<UImage>(InventoryWidget->GetWidgetFromName(TEXT("Weapon1Image"))))
				{
					if (UGameInstance* GameInstance = GetGameInstance())
					{
						if (UDefaultGameInstance* DefaultGameInstance = Cast<UDefaultGameInstance>(GameInstance))
						{
							if (DefaultGameInstance->FindWeaponByIdx(1))
								WeaponUI1Image->SetOpacity(1.f);
						}
					}
				}
				if (UImage* WeaponUI2Image = Cast<UImage>(InventoryWidget->GetWidgetFromName(TEXT("Weapon2Image"))))
				{
					if (UGameInstance* GameInstance = GetGameInstance())
					{
						if (UDefaultGameInstance* DefaultGameInstance = Cast<UDefaultGameInstance>(GameInstance))
						{
							if (DefaultGameInstance->FindWeaponByIdx(2))
								WeaponUI2Image->SetOpacity(1.f);
						}
					}
				}
				if (UImage* WeaponUI3Image = Cast<UImage>(InventoryWidget->GetWidgetFromName(TEXT("Weapon3Image"))))
				{
					if (UGameInstance* GameInstance = GetGameInstance())
					{
						if (UDefaultGameInstance* DefaultGameInstance = Cast<UDefaultGameInstance>(GameInstance))
						{
							if (DefaultGameInstance->FindWeaponByIdx(3))
								WeaponUI3Image->SetOpacity(1.f);
						}
					}
				}


				//Item UI
				if (UTextBlock* CurrentInventoryHealthText = Cast<UTextBlock>(InventoryWidget->GetWidgetFromName(TEXT("Item1Text"))))
				{
					if (UGameInstance* GameInstance = GetGameInstance())
					{
						if (UDefaultGameInstance* DefaultGameInstance = Cast<UDefaultGameInstance>(GameInstance))
						{
							CurrentInventoryHealthText->SetText(FText::FromString(FString::Printf(TEXT("%d"), DefaultGameInstance->InventoryItem[1])));
						}
					}
				}
				if (UTextBlock* CurrentInventoryAmmoText = Cast<UTextBlock>(InventoryWidget->GetWidgetFromName(TEXT("Item2Text"))))
				{
					if (UGameInstance* GameInstance = GetGameInstance())
					{
						if (UDefaultGameInstance* DefaultGameInstance = Cast<UDefaultGameInstance>(GameInstance))
						{
							CurrentInventoryAmmoText->SetText(FText::FromString(FString::Printf(TEXT("%d"), DefaultGameInstance->InventoryItem[2])));
						}
					}
				}

				//[YJ fixing]
				if (UTextBlock* CurrentInventoryAmmoText = Cast<UTextBlock>(InventoryWidget->GetWidgetFromName(TEXT("Item3Text"))))
				{
					if (UGameInstance* GameInstance = GetGameInstance())
					{
						if (UDefaultGameInstance* DefaultGameInstance = Cast<UDefaultGameInstance>(GameInstance))
						{
							CurrentInventoryAmmoText->SetText(FText::FromString(FString::Printf(TEXT("%d"), DefaultGameInstance->InventoryItem[3])));
						}
					}
				}



				if (UImage* KeyImageWidget = Cast<UImage>(InventoryWidget->GetWidgetFromName(TEXT("KeyImage"))))
				{
					if (UImage* GearImageWidget = Cast<UImage>(InventoryWidget->GetWidgetFromName(TEXT("GearImage"))))
					{
						if (UGameInstance* GameInstance = GetGameInstance())
						{
							if (UDefaultGameInstance* DefaultGameInstance = Cast<UDefaultGameInstance>(GameInstance))
							{
								int ItemID = DefaultGameInstance->SpecialSlotItemID;
								UE_LOG(LogTemp, Warning, TEXT("itemid : %d"), ItemID);
								//default state
								KeyImageWidget->SetVisibility(ESlateVisibility::Hidden);
								GearImageWidget->SetVisibility(ESlateVisibility::Hidden);

								if (ItemID == 100) // if KeyItem, show KeyImage
								{
									KeyImageWidget->SetVisibility(ESlateVisibility::Visible);
								}
								else if (ItemID >= 1 && ItemID <= 5) // if GearItem, show GearImage
								{
									GearImageWidget->SetVisibility(ESlateVisibility::Visible);
								}
							}
						}
					}
				}
			
				
			}
		}
	}
}