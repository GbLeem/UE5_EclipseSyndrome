#include "System/DefaultGameInstance.h"

#include "Character/PlayerCharacter.h"

UDefaultGameInstance::UDefaultGameInstance()
	:InventoryAmmo(100)
	,PlayerMaxHealth(100)
	,PlayerCurrentHealth(100)
	,CurrentLevel(0)
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

void UDefaultGameInstance::AddItem(int32 ItemIdx, int32 ItemAmount)
{
	if (InventoryItem.Contains(ItemIdx))
	{
		InventoryItem[ItemIdx] += ItemAmount;
		if (ItemIdx == 2)
			AddAmmo(ItemAmount);
	}
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
		PlayerCurrentHealth = 0;
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
