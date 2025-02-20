#include "Item/PlayerItem/HealItem.h"
#include "Components/BoxComponent.h"

AHealItem::AHealItem()
{
	HealAmount = 20.0f;
	ItemType = "Heal";
}


void AHealItem::ActivateItem(AActor* Activator)
{
	DestroyItem();
}