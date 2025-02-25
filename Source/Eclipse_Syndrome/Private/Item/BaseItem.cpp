#include "Item/BaseItem.h"

#include "Components/WidgetComponent.h"
#include "Components/BoxComponent.h"
#include "Components/TextBlock.h"
#include "Character/PlayerCharacter.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/InputSettings.h"

ABaseItem::ABaseItem()
	:bIsPeeking(false)
	,ItemNumber(0)
	,ItemAmount(0)
{
	PrimaryActorTick.bCanEverTick = true;

	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMeshComp->SetupAttachment(RootComponent);

	GlowEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("GlowEffect"));
	GlowEffect->SetupAttachment(RootComponent);

	ItemHoverUI = CreateDefaultSubobject<UWidgetComponent>(TEXT("Hover UI"));
	ItemHoverUI->SetupAttachment(StaticMeshComp);
	ItemHoverUI->SetWidgetSpace(EWidgetSpace::Screen);
	ItemHoverUI->SetVisibility(false);

	static ConstructorHelpers::FClassFinder<UUserWidget>ItemUIClass(TEXT("/Game/HJ/UI/WBP_Item.WBP_Item_C"));
	if (ItemUIClass.Succeeded())
	{
		ItemHoverUI->SetWidgetClass(ItemUIClass.Class);
	}

	Tags.Add("Item");
}

void ABaseItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsPeeking)
		ShowUI();
	else
		StopUI();

	bIsPeeking = false;
}


////currently not working 
////-> will be deleted when this code is completely useless
////Overlap + UMG(coming soon..)
//void ABaseItem::OnPlayerOverlapBegin(
//	UPrimitiveComponent* OverlappedComponent,
//	AActor* OtherActor,
//	UPrimitiveComponent* OtherComp,
//	int32 OtherBodyIndex,
//	bool bFromSweep,
//	const FHitResult& SweepResult
//)
//{
//	if (OtherActor && OtherActor->IsA(APlayerCharacter::StaticClass()))
//	{
//		bCanPickUp = true;
//		UE_LOG(LogTemp, Warning, TEXT("Press F to collect Item"));
//	}
//}
//
//
////currently not working
////-> will be deleted when this code is completely useless
//void ABaseItem::OnPlayerOverlapEnd(
//	UPrimitiveComponent* OverlappedComponent,
//	AActor* OtherActor,
//	UPrimitiveComponent* OtherComp,
//	int32 OtherBodyIndex)
//{
//	if (OtherActor && OtherActor->IsA(APlayerCharacter::StaticClass()))
//	{
//		bCanPickUp = false;
//		UE_LOG(LogTemp, Warning, TEXT("Item Player out of range!"));
//	}
//}

//Use this code(for player) 
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

/*//For test(press T when spawned)
void ABaseItem::TestCollectItem()
{
	UE_LOG(LogTemp, Warning, TEXT("Gained item: %s"), *ItemType.ToString());
	DestroyItem();

}
*/



//Called when player used the Item
void ABaseItem::ActivateItem(AActor* Activator)
{
	UE_LOG(LogTemp, Warning, TEXT("Used Item: %s"), *ItemType.ToString());
}

FName ABaseItem::GetItemType() const
{
	return ItemType;
}

void ABaseItem::ShowUI()
{
	ItemHoverUI->SetVisibility(true);	
}

void ABaseItem::StopUI()
{
	ItemHoverUI->SetVisibility(false);
}

void ABaseItem::DestroyItem()
{
	Destroy();
}



/*//for test(press F to grab Item)

void ABaseItem::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController)
	{
		EnableInput(PlayerController);
		if (InputComponent)
		{
			InputComponent->BindAction("TestDeleteItem", IE_Pressed, this, &ABaseItem::TestCollectItem);
		}
	}

}*/