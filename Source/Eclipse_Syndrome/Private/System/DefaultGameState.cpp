#include "System/DefaultGameState.h"

#include "Character/PlayerCharacter.h"
#include "Character/PlayerCharacterController.h"
#include "System/DefaultGameInstance.h"


#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Button.h"

ADefaultGameState::ADefaultGameState()
	:CurrentLevelIndex(0)
	, MaxLevelIndex(3)
{
	LevelMapNames.Push(FName("Lv1"));
	LevelMapNames.Push(FName("Lv2"));
	LevelMapNames.Push(FName("Lv3"));

	/*LevelMapNames.Push(FName("MainLevelTest"));
	LevelMapNames.Push(FName("MainLevel_2"));*/
}

void ADefaultGameState::BeginPlay()
{
	Super::BeginPlay();
	
	UpdateHUD();
	StartLevel();

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
				if (UImage* WeaponUI4Image = Cast<UImage>(InventoryWidget->GetWidgetFromName(TEXT("Weapon4Image"))))
				{
					if (UGameInstance* GameInstance = GetGameInstance())
					{
						if (UDefaultGameInstance* DefaultGameInstance = Cast<UDefaultGameInstance>(GameInstance))
						{
							if (DefaultGameInstance->FindWeaponByIdx(4))
								WeaponUI4Image->SetOpacity(1.f);
						}
					}
				}
				if (UImage* WeaponUI5Image = Cast<UImage>(InventoryWidget->GetWidgetFromName(TEXT("SubWeaponImage"))))
				{
					if (UGameInstance* GameInstance = GetGameInstance())
					{
						if (UDefaultGameInstance* DefaultGameInstance = Cast<UDefaultGameInstance>(GameInstance))
						{
							if (DefaultGameInstance->FindWeaponByIdx(5))
								WeaponUI5Image->SetOpacity(1.f);
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

void ADefaultGameState::StartLevel()
{	
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (APlayerCharacterController* PlayerCharacterController = Cast<APlayerCharacterController>(PlayerController))
		{
			if (PlayerCharacterController)
			{
				PlayerCharacterController->ShowHUD();
			}
		}
	}
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		UDefaultGameInstance* DefaultGameInstance = Cast<UDefaultGameInstance>(GameInstance);

		if (DefaultGameInstance)
		{
			CurrentLevelIndex = DefaultGameInstance->CurrentLevel;				
		}
	}	
}

void ADefaultGameState::LevelChange()
{
	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString::Printf(TEXT("%d"), CurrentLevelIndex));

	if (GetGameInstance())
	{
		UDefaultGameInstance* DefaultGameInstance = Cast<UDefaultGameInstance>(GetGameInstance());
		if (DefaultGameInstance)
		{
			DefaultGameInstance->CurrentLevel++;
			CurrentLevelIndex = DefaultGameInstance->CurrentLevel;
		}
	}

	if (CurrentLevelIndex >= MaxLevelIndex)
	{
		GameClear();
		return;
	}

	if (LevelMapNames.IsValidIndex(CurrentLevelIndex))
	{
		UGameplayStatics::OpenLevel(GetWorld(), LevelMapNames[CurrentLevelIndex]);
	}
	else
	{
		GameClear();
	}
}

void ADefaultGameState::GameClear()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (APlayerCharacterController* PlayerCharacterController = Cast<APlayerCharacterController>(PlayerController))
		{
			PlayerCharacterController->SetInputMode(FInputModeUIOnly());
			PlayerCharacterController->ShowGameClearUI();

			GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Blue, FString::Printf(TEXT("Game Clear")));
		}
	}
}

//[TODO] when player dead, go to Main Menu
void ADefaultGameState::GameOver()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (APlayerCharacterController* PlayerCharacterController = Cast<APlayerCharacterController>(PlayerController))
		{
			PlayerCharacterController->ShowGameOverUI();
		}
	}
}

void ADefaultGameState::OpenCurrentLevel()
{
	if (LevelMapNames.IsValidIndex(CurrentLevelIndex))
	{
		UGameplayStatics::OpenLevel(GetWorld(), LevelMapNames[CurrentLevelIndex]);
	}
}
