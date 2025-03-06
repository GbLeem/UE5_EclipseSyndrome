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

	GlowEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("GlowEffect"));
	GlowEffect->SetupAttachment(StaticMeshComp);
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem>NiagaraEffect(TEXT("/Game/Yujin/FX/DrapEffet/VFX/NE_drop_effects03"));
	if (NiagaraEffect.Succeeded())
	{
		GlowEffect->SetAsset(NiagaraEffect.Object);
	}
	GlowEffect->SetRelativeLocation(FVector(0.0f, 0.0f, -10.0f));


	InteractionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	InteractionBox->SetupAttachment(StaticMeshComp);
	InteractionBox->SetBoxExtent(FVector(3000.f, 3000.f, 3000.f));
	InteractionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractionBox->SetCollisionObjectType(ECC_WorldDynamic);
	InteractionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	InteractionBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	InteractionBox->OnComponentBeginOverlap.AddDynamic(this, &AKeyItem::OnPlayerOverlapBegin);
	InteractionBox->OnComponentEndOverlap.AddDynamic(this, &AKeyItem::OnPlayerOverlapEnd);

	//Only for test
	InteractionBox->SetHiddenInGame(true);
	InteractionBox->SetVisibility(false);
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

bool AKeyItem::CanUseKeyItem(AActor* Activator)
{
	if (!Activator) return false;

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
			return true;  // Can Open Door
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Too far from the garage door"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No GarageDoor found in the Level!"));
	}

	return false;  // Cannot Open Door
}