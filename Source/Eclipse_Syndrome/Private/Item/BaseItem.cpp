#include "Item/BaseItem.h"
#include "Components/BoxComponent.h"
#include "Character/PlayerCharacter.h"

ABaseItem::ABaseItem()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRootComp = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRootComp);

	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMeshComp->SetupAttachment(SceneRootComp);


	GlowEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("GlowEffect"));
	GlowEffect->SetupAttachment(RootComponent);

}



//Overlap + UMG(coming soon..)
void ABaseItem::OnPlayerOverlapBegin(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	if (OtherActor && OtherActor->IsA(APlayerCharacter::StaticClass()))
	{
		bCanPickUp = true;
		UE_LOG(LogTemp, Warning, TEXT("Press F to collect Item"));
	}
}



void ABaseItem::OnPlayerOverlapEnd(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	if (OtherActor && OtherActor->IsA(APlayerCharacter::StaticClass()))
	{
		bCanPickUp = false;
		UE_LOG(LogTemp, Warning, TEXT("Item Player out of range!"));
	}
}

//Called when player grabbed the Item
void ABaseItem::CollectItem(AActor* Collector)
{

	APlayerCharacter* Player = Cast<APlayerCharacter>(Collector);
	if(Player)
	{
		UE_LOG(LogTemp, Warning, TEXT("Gained item: %s"), *ItemType.ToString());
		DestroyItem();
		//Inventory add code here?
	}

}

//Called when player used the Item
void ABaseItem::ActivateItem(AActor* Activator)
{
		UE_LOG(LogTemp, Warning, TEXT("Used Item: %s"), *ItemType.ToString());
}

FName ABaseItem::GetItemType() const
{
	return ItemType;
}

void ABaseItem::DestroyItem()
{
	Destroy();
}