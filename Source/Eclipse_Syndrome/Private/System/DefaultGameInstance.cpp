#include "System/DefaultGameInstance.h"

#include "Character/PlayerCharacter.h"
#include "System/DefaultGameState.h"

UDefaultGameInstance::UDefaultGameInstance()
	:InventoryAmmo(100)
	,PlayerMaxHealth(100)
	,PlayerCurrentHealth(100)
	,CurrentLevel(0)
	//[YJ fixing]
	,SpecialSlotItemID(-1)
{
	InventoryItem.Add(1, 0);
	InventoryItem.Add(2, InventoryAmmo);
	//[YJ fixing]
	InventoryItem.Add(3, 0);
}

void UDefaultGameInstance::AddAmmo(int32 Amount)
{
	InventoryAmmo += Amount;
}

void UDefaultGameInstance::UseAmmo(int32 Amount)
{
	InventoryAmmo -= Amount;
	InventoryItem[2] = InventoryAmmo;
}

//[YJfixing]
void UDefaultGameInstance::AddItem(int32 ItemIdx, int32 ItemAmount, EItemType ItemType, int32 BlockID)
{
	
	if(ItemType == EItemType::Key || ItemType == EItemType::PuzzleBlock)
	{
		if (SpecialSlotItemID == -1)
			//if inventory slot is empty,
		{
			if (ItemType == EItemType::PuzzleBlock)
			{//if puzzleblock,
				InventoryItem.Add(ItemIdx, ItemAmount);
				SpecialSlotItemID = BlockID;
				PuzzleBlockIDMap.Add(ItemIdx, BlockID);
				UE_LOG(LogTemp, Warning, TEXT("PuzzleBlock Added: %d with BlockID: %d"), ItemIdx, BlockID);
			}
			else
			{//if key,
				InventoryItem.Add(ItemIdx, ItemAmount);
				SpecialSlotItemID = 100;
				UE_LOG(LogTemp, Warning, TEXT("Special Item Added: %d (Type: %d)"), ItemIdx, static_cast<int32>(ItemType));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Special Slot is already occupied! Current Item: %d"), SpecialSlotItemID);
		}
	}
	else
	{
		// default item add
		if (InventoryItem.Contains(ItemIdx))
		{
			InventoryItem[ItemIdx] += ItemAmount;
			if (ItemIdx == 2)
				AddAmmo(ItemAmount);
		}
		else
		{
			InventoryItem.Add(ItemIdx, ItemAmount);
		}
		UE_LOG(LogTemp, Warning, TEXT("Added Normal Item: %d (Amount: %d)"), ItemIdx, ItemAmount);

	}
}

void UDefaultGameInstance::AddWeapon(int32 WeaponIdx)
{
	WeaponInventoryItem.Add(WeaponIdx);
}

bool UDefaultGameInstance::FindWeaponByIdx(int32 WeaponIdx)
{
	if (WeaponInventoryItem.Contains(WeaponIdx))
		return true;
	return false;
}

//[YJfixing]
bool UDefaultGameInstance::AddSpecialItem(int32 ItemID)
{
	if (SpecialSlotItemID != -1)
	{
		UE_LOG(LogTemp, Warning, TEXT("Item %d already in slot 3"), SpecialSlotItemID);
		return false;
	}
	SpecialSlotItemID = ItemID;
	UE_LOG(LogTemp, Warning, TEXT("Item %d added to slot 3"), ItemID);
	return true;
}


//[YJfixing] Called when slot needs to be empty
void UDefaultGameInstance::RemoveSpecialItem()
{
	if (SpecialSlotItemID != -1)//if exists
	{
		UE_LOG(LogTemp, Warning, TEXT("Item %d removed from slot 3"), SpecialSlotItemID);
		SpecialSlotItemID = -1;
	}
}

void UDefaultGameInstance::InitializeInstance()
{
	InventoryAmmo = 100;
	PlayerMaxHealth = 100;
	PlayerCurrentHealth = 100;
	CurrentLevel = 0;
	SpecialSlotItemID = -1;

	InventoryItem.Add(1, 0);
	InventoryItem.Add(2, InventoryAmmo);
	InventoryItem.Add(3, 0);
	
	WeaponInventoryItem.Empty();
}


void UDefaultGameInstance::PlusHealth(int32 Amount)
{
	if (PlayerCurrentHealth + Amount <= PlayerMaxHealth)
		PlayerCurrentHealth += Amount;
	else
		PlayerCurrentHealth = PlayerMaxHealth;
}

void UDefaultGameInstance::MinusHealth(int32 Amount)
{
	if (PlayerCurrentHealth - Amount <= 0)
	{
		PlayerCurrentHealth = 0;

		//death
		ADefaultGameState* DefaultGameState = Cast<ADefaultGameState>(GetWorld()->GetGameState());
		if (DefaultGameState)
		{
			DefaultGameState->GameOver();
		}

	}
	else
		PlayerCurrentHealth -= Amount;
}

void UDefaultGameInstance::SetCurrentWeapon(AWeapon* CurrentWeapon)
{
	if (CurrentWeapon)
	{
		CurrentWeaponInstance = CurrentWeapon;
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Black, CurrentWeaponInstance.GetName());
	}
}

void UDefaultGameInstance::SetCurrentWeaponAmmo(int32 Amount)
{

}
