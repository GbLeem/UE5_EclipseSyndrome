#include "InteractableItem/Chest.h"
#include "Components/BoxComponent.h"
#include "Character/PlayerCharacter.h"

AChest::AChest()
{
	PrimaryActorTick.bCanEverTick = false;


	InteractionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionBox"));
	InteractionBox->SetupAttachment(RootComponent);
	InteractionBox->SetBoxExtent(FVector(200.f, 200.f, 200.f)); //Collision range
	InteractionBox->SetCollisionProfileName(TEXT("Trigger"));

	InteractionBox->OnComponentBeginOverlap.AddDynamic(this, &AChest::OnPlayerOverlapBegin);
	InteractionBox->OnComponentEndOverlap.AddDynamic(this, &AChest::OnPlayerOverlapEnd);

	SkeletalMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMeshComp->SetupAttachment(InteractionBox);


}


//Player Overlap
void AChest::OnPlayerOverlapBegin(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	UE_LOG(LogTemp, Warning, TEXT("OnPlayerOverlapBegin executed!"));
	if (OtherActor && OtherActor->IsA(APlayerCharacter::StaticClass()))
	{
		UE_LOG(LogTemp, Warning, TEXT("Player in range!"));
		bPlayerInRange = true;
		//need to connect with F key
		OpenChest();

	}
}




void AChest::OnPlayerOverlapEnd(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	if (OtherActor && OtherActor->IsA(APlayerCharacter::StaticClass()))
	{
		UE_LOG(LogTemp, Warning, TEXT("Player out of Range"));
		bPlayerInRange = false; 
		CloseChest();
	}
}


void AChest::InteractWithChest()
{
	if (bPlayerInRange)
	{
		UE_LOG(LogTemp, Warning, TEXT("Chest Interacted!"));
		OpenChest();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No chest in range!"));
	}
}



void AChest::OpenChest()
{
	
	//about to add chest open animation here
	if (OpenAnimation)
	{
		SkeletalMeshComp->PlayAnimation(OpenAnimation, false);
		UE_LOG(LogTemp, Warning, TEXT("Chest Opened!"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("OpenAnimation is not set!"));
	}
	SpawnRandomItem();
}



void AChest::SpawnRandomItem()
{
	if (PossibleItems.Num() > 0)
	{
		//random temporary(about to fix)
		int32 RandomIndex = FMath::RandRange(0, PossibleItems.Num() - 1);
		TSubclassOf<AActor> SelectedItemClass = PossibleItems[RandomIndex];

		if (SelectedItemClass)
		{
			FVector SpawnLocation = GetActorLocation() + FVector(0, 0, 110);
			FRotator SpawnRotation = FRotator::ZeroRotator;

			//Spawn PlayerItem on Chest
			GetWorld()->SpawnActor<AActor>(
				SelectedItemClass,
				SpawnLocation,
				SpawnRotation
			);

			UE_LOG(LogTemp, Warning, TEXT("Spawned Item: %s"), *SelectedItemClass->GetName());

		}
	}
}


void AChest::CloseChest()
{
	if (OpenAnimation)
	{
		SkeletalMeshComp->PlayAnimation(OpenAnimation, false);
		SkeletalMeshComp->SetPlayRate(-1.0f);
		UE_LOG(LogTemp, Warning, TEXT("Chest Closed!"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("OpenAnimation is not set!"));
	}
}