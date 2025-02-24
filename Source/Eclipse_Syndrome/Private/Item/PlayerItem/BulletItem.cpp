#include "Item/PlayerItem/BulletItem.h"
#include "Components/BoxComponent.h"


ABulletItem::ABulletItem()
{	
	ItemType = "Bullet";

	static ConstructorHelpers::FObjectFinder<UStaticMesh>StaticMeshAsset(TEXT("/Game/Yujin/Assets/BulletItem/bullet.bullet"));
	if (StaticMeshAsset.Succeeded())
	{
		StaticMeshComp->SetStaticMesh(StaticMeshAsset.Object);
	}

	StaticMeshComp->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	StaticMeshComp->SetRelativeScale3D(FVector(10.f, 10.f, 10.f));
}

void ABulletItem::ActivateItem(AActor* Activator)
{
	DestroyItem();
}