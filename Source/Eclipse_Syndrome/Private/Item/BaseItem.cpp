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
	,bIsPuzzleBlock(false)
{
	PrimaryActorTick.bCanEverTick = true;

	if (!bIsPuzzleBlock)
	{
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


//Use this code(for player) 
//Called when player grabbed the Item
void ABaseItem::CollectItem(AActor* Collector)
{

	APlayerCharacter* Player = Cast<APlayerCharacter>(Collector);
	if(Player)
	{
		UE_LOG(LogTemp, Warning, TEXT("Gained item: %s"), *ItemType.ToString());
		DestroyItem();
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
