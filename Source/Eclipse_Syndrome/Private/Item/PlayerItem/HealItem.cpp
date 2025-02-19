#include "Item/PlayerItem/HealItem.h"

AHealItem::AHealItem()
{
	HealAmount = 20.0f;
	ItemType = "Heal";

	SceneRootComp = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRootComp);

	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMeshComp->SetupAttachment(SceneRootComp);
}


void AHealItem::ActivateItem(AActor* Activator)
{
	DestroyItem();
}