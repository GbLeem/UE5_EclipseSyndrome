#include "Item/BaseItem.h"


ABaseItem::ABaseItem()
{
	PrimaryActorTick.bCanEverTick = false;
}


void ABaseItem::ActivateItem(AActor* Activator)
{
}
FName ABaseItem::GetItemType() const
{
	return ItemType;
}

void ABaseItem::DestroyItem()
{
	Destroy();
}