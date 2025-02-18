#include "Item/PlayerItem/HealItem.h"

AHealItem::AHealItem()
{
	HealAmount = 20.0f;
	ItemType = "Heal";
}


void AHealItem::ActivateItem(AActor* Activator)
{
	DestroyItem();
}