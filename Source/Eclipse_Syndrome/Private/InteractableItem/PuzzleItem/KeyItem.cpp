#include "InteractableItem/PuzzleItem/KeyItem.h"
#include "EngineUtils.h"

AKeyItem::AKeyItem()
{
	ItemType = "Key";
}


void AKeyItem::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Error, TEXT("(TEST) BeginPlay called"));
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AKeyItem::TestActivateItem, 5.0f, false);
}


void AKeyItem::TestActivateItem()
{
	UE_LOG(LogTemp, Error, TEXT("(TEST) TestActivateItem called"));

	AActor* PlayerActor = GetWorld()->GetFirstPlayerController()->GetPawn();

	if (!PlayerActor)
	{
		UE_LOG(LogTemp, Error, TEXT("(TEST) no player found"));
		return;
	}



	AGarageDoor* FoundDoor = nullptr;
	
	for (TActorIterator<AGarageDoor> It(GetWorld()); It; ++It)
	{
		FoundDoor = *It;
		break;
	}

	if (FoundDoor)
	{
		UE_LOG(LogTemp, Error, TEXT("(TEST) door found"));
		ActivateItem(PlayerActor);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("(TEST) door found"));
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