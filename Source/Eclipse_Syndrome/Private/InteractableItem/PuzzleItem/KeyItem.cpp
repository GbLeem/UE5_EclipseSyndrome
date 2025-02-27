#include "InteractableItem/PuzzleItem/KeyItem.h"
#include "EngineUtils.h"

AKeyItem::AKeyItem()
{
	ItemType = "Key";
}


void AKeyItem::BeginPlay()
{
	Super::BeginPlay();

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AKeyItem::TestActivateItem, 5.0f, false);
}


void AKeyItem::TestActivateItem()
{
	AActor* FakePlayer = GetWorld()->SpawnActor<AActor>(AActor::StaticClass(), GetActorLocation(), FRotator::ZeroRotator);
	if (FakePlayer)
	{
		ActivateItem(FakePlayer);
	}
}


void AKeyItem::ActivateItem(AActor* Activator)
{
	if (!Activator) return;

	//Max Distance for player to open the garage door with a key
	const float MaxDistance = 300.0f;
	
	AGarageDoor* FoundDoor = nullptr;
	for (TActorIterator<AGarageDoor> It(GetWorld()); It; ++It)
	{
		FoundDoor = *It;
		break;

	}

	if (FoundDoor)
	{

		float Distance = FVector::Dist(Activator->GetActorLocation(), FoundDoor->GetActorLocation());
		if (Distance <= MaxDistance)
		{
			FoundDoor->OpenGarageDoor();
			DestroyItem();
		}
		else 
		{
			UE_LOG(LogTemp, Error, TEXT("too far from the garage door"));
		}
		
			
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("No GarageDoor found in the Level!"));
	}
}