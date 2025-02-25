#include "System/DefaultGameInstance.h"

UDefaultGameInstance::UDefaultGameInstance()
	:InventoryAmmo(100)
{
}

void UDefaultGameInstance::AddAmmo(int32 Amount)
{
	InventoryAmmo += Amount;
}

void UDefaultGameInstance::UseAmmo(int32 Amount)
{
	InventoryAmmo -= Amount;
}

void UDefaultGameInstance::AddItem(ABaseItem* Item)
{
	InventoryItem.Add(Item);
}
