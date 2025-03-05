#include "Item/PlayerItem/BulletItem.h"
#include "Components/BoxComponent.h"


ABulletItem::ABulletItem()
{
	ItemType = "Bullet";
	ItemNumber = 2;
	ItemAmount = 30;

	static ConstructorHelpers::FObjectFinder<UStaticMesh>StaticMeshAsset(TEXT("/Game/Yujin/Assets/BulletItem/bullet.bullet"));
	if (StaticMeshAsset.Succeeded())
	{
		StaticMeshComp->SetStaticMesh(StaticMeshAsset.Object);
	}

	StaticMeshComp->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	StaticMeshComp->SetRelativeScale3D(FVector(10.f, 10.f, 10.f));

	GlowEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("GlowEffect"));
	GlowEffect->SetupAttachment(StaticMeshComp);
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem>NiagaraEffect(TEXT("/Game/Yujin/FX/DrapEffet/VFX/NE_drop_effects03"));
	if (NiagaraEffect.Succeeded())
	{
		GlowEffect->SetAsset(NiagaraEffect.Object);
	}
	/*GlowEffect->SetRelativeLocation(FVector(0.0f, 0.0f, 10.0f));*/
	GlowEffect->SetRelativeLocation(FVector(0.f, 0.f, 0.f));
	GlowEffect->SetRelativeRotation(FRotator(0.f, 0.f, 0.f));
	GlowEffect->SetRelativeScale3D(FVector(1.f, 1.f, 1.f));
}

void  ABulletItem::BeginPlay()
{
	Super::BeginPlay();
	if (GlowEffect)
	{
		GlowEffect->Activate();  // BeginPlay에서 GlowEffect 활성화
	}
	
}

void ABulletItem::ActivateItem(AActor* Activator)
{
	if (GlowEffect)
	{
		GlowEffect->Deactivate();  // 효과 비활성화
	}
	DestroyItem();
}