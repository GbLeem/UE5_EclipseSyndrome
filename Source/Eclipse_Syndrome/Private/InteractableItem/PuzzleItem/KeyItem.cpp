#include "InteractableItem/PuzzleItem/KeyItem.h"
#include "Character/PlayerCharacter.h"
#include "EngineUtils.h"

AKeyItem::AKeyItem()
{
	ItemType = "Key";
	//[fixing]Connecting with Character
	ItemNumber = 3;
	ItemAmount = 1;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> StaticMeshAsset(TEXT("/Game/Yujin/Assets/key/source/Key.Key"));
	if (StaticMeshAsset.Succeeded())
	{
		StaticMeshComp->SetStaticMesh(StaticMeshAsset.Object);
	}

	InteractionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	InteractionBox->SetupAttachment(StaticMeshComp);
	InteractionBox->SetBoxExtent(FVector(1000.f, 1000.f, 1000.f));
	InteractionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractionBox->SetCollisionObjectType(ECC_WorldDynamic);
	InteractionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	InteractionBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	InteractionBox->OnComponentBeginOverlap.AddDynamic(this, &AKeyItem::OnPlayerOverlapBegin);
	InteractionBox->OnComponentEndOverlap.AddDynamic(this, &AKeyItem::OnPlayerOverlapEnd);

	//Only for test
	InteractionBox->SetHiddenInGame(false);
	InteractionBox->SetVisibility(true);
}


void AKeyItem::BeginPlay()
{
	Super::BeginPlay();
	//FString TagString;
	//for (const FName& tag : Tags)
	//{
	//	TagString += tag.ToString() + TEXT(" ");
	//}
	//UE_LOG(LogTemp, Warning, TEXT("AKeyItem Tags : %s"), *TagString);
	//UE_LOG(LogTemp, Error, TEXT("(TEST) BeginPlay called"));
	/*FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AKeyItem::TestActivateItem, 5.0f, false);*/
}

void AKeyItem::OnPlayerOverlapBegin(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->IsA(APlayerCharacter::StaticClass()))
	{
		bPlayerInRange = true;
		UE_LOG(LogTemp, Warning, TEXT("Press F"));

		//Player code
		APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(OtherActor);
		if (PlayerCharacter)
		{
			PlayerCharacter->StartPeek();
		}

	}
}

void AKeyItem::OnPlayerOverlapEnd(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	if (OtherActor && OtherActor->IsA(APlayerCharacter::StaticClass()))
	{
		UE_LOG(LogTemp, Warning, TEXT("Player out of Range"));
		bPlayerInRange = false;

		APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(OtherActor);
		if (PlayerCharacter)
		{
			PlayerCharacter->StopPeek();
		}
	}
}

//void AKeyItem::TestActivateItem()
//{
//	UE_LOG(LogTemp, Error, TEXT("(TEST) TestActivateItem called"));
//
//	AActor* PlayerActor = GetWorld()->GetFirstPlayerController()->GetPawn();
//
//	if (!PlayerActor)
//	{
//		UE_LOG(LogTemp, Error, TEXT("(TEST) no player found"));
//		return;
//	}
//
//
//
//	AGarageDoor* FoundDoor = nullptr;
//	
//	for (TActorIterator<AGarageDoor> It(GetWorld()); It; ++It)
//	{
//		FoundDoor = *It;
//		break;
//	}
//
//	if (FoundDoor)
//	{
//		UE_LOG(LogTemp, Error, TEXT("(TEST) door found"));
//		ActivateItem(PlayerActor);
//	}
//	else
//	{
//		UE_LOG(LogTemp, Error, TEXT("(TEST) door found"));
//	}
//}


void AKeyItem::ActivateItem(AActor* Activator)
{
	if (!Activator) return;

	//Max Distance for player to open the garage door with a key
	const float MaxDistance = 500.0f;
	
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