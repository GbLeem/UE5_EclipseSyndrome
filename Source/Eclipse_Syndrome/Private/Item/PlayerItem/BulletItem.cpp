#include "Item/PlayerItem/BulletItem.h"

ABulletItem::ABulletItem()
{
	ItemType = "Bullet";
}

void ABulletItem::ActivateItem(AActor* Activator)
{
	DestroyItem();
}