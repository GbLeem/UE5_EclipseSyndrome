#include "System/DefaultGameInstance.h"

#include "Character/PlayerCharacter.h"

UDefaultGameInstance::UDefaultGameInstance()
	:InventoryAmmo(100)
{
	InventoryItem.Add(1, 0);
	InventoryItem.Add(2, InventoryAmmo);
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

void UDefaultGameInstance::AddItem(int32 ItemIdx, int32 ItemAmount)
{
	if(InventoryItem.Contains(ItemIdx))
		InventoryItem[ItemIdx] += ItemAmount;
	else
		InventoryItem.Add(ItemIdx, ItemAmount);
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
