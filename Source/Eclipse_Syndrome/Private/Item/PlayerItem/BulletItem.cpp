#include "Item/PlayerItem/BulletItem.h"
#include "Components/BoxComponent.h"


ABulletItem::ABulletItem()
{
	
	ItemType = "Bullet";
}

void ABulletItem::ActivateItem(AActor* Activator)
{
	DestroyItem();
}