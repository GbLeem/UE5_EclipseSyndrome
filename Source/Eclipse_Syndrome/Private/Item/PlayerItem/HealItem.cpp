#include "Item/PlayerItem/HealItem.h"
#include "Components/BoxComponent.h"

AHealItem::AHealItem()
{
	HealAmount = 20.0f;
	ItemType = "Heal";

	static ConstructorHelpers::FObjectFinder<UStaticMesh>StaticMeshAsset(TEXT("/Game/Yujin/Assets/HealItem/HealKit.HealKit"));
	if (StaticMeshAsset.Succeeded())
	{
		StaticMeshComp->SetStaticMesh(StaticMeshAsset.Object);
	}
	StaticMeshComp->SetRelativeScale3D(FVector(0.4f, 0.4f, 0.4f));
}


void AHealItem::ActivateItem(AActor* Activator)
{
	DestroyItem();
}