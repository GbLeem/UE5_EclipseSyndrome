#include "Item/PlayerItem/BulletItem.h"

ABulletItem::ABulletItem()
{
	
	ItemType = "Bullet";

	SceneRootComp = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRootComp);

	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMeshComp->SetupAttachment(SceneRootComp);
}

void ABulletItem::ActivateItem(AActor* Activator)
{
	DestroyItem();
}