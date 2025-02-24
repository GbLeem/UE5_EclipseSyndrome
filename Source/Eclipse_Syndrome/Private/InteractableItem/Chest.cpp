#include "InteractableItem/Chest.h"
#include "Components/BoxComponent.h"
#include "Character/PlayerCharacter.h"
#include "Components/TimelineComponent.h"


AChest::AChest()
{
	PrimaryActorTick.bCanEverTick = false;


	InteractionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionBox"));
	InteractionBox->SetupAttachment(RootComponent);
	InteractionBox->SetBoxExtent(FVector(200.f, 200.f, 200.f)); //Collision range
	InteractionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly); //overlap only
	InteractionBox->SetCollisionObjectType(ECC_WorldDynamic);
	InteractionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	InteractionBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);



	InteractionBox->OnComponentBeginOverlap.AddDynamic(this, &AChest::OnPlayerOverlapBegin);
	InteractionBox->OnComponentEndOverlap.AddDynamic(this, &AChest::OnPlayerOverlapEnd);

	//Only for test
	InteractionBox->SetHiddenInGame(false);
	InteractionBox->SetVisibility(true);


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
	const FHitResult& SweepResult)
{
	
	if (OtherActor && OtherActor->IsA(APlayerCharacter::StaticClass()))
	{
		
		bPlayerInRange = true;
		bCanOpenChest = true;
		UE_LOG(LogTemp, Warning, TEXT("Press F"));

		//Only for test
		//For Player Test
		//( When player enters Box Collision of the Chest, 
		//  OpenChest function will activate and random items will be spawned )
		OpenChest();

		//[HJ add]
		APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(OtherActor);

		if (PlayerCharacter)
		{
			PlayerCharacter->StartPeek();
		}
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
		UE_LOG(LogTemp, Warning, TEXT("Player out of Range. Delete Message"));
		bPlayerInRange = false; 
		bCanOpenChest = false;

		//[HJ add]
		APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(OtherActor);

		if (PlayerCharacter)
		{
			PlayerCharacter->StopPeek();
		}
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
	if (OpenAnimation && !bIsOpened)
	{
		SkeletalMeshComp->PlayAnimation(OpenAnimation, false);
		UE_LOG(LogTemp, Warning, TEXT("Chest Opened!"));

		float AnimationDuration = OpenAnimation->GetPlayLength();

		CheckChestOpened();

		FTimerHandle SpawnItemTimerHandle;
		float SpawnTime = FMath::Max(0.1f, AnimationDuration - 1.3f);
		GetWorld()->GetTimerManager().SetTimer(SpawnItemTimerHandle, this, &AChest::SpawnRandomItem, SpawnTime, false);
		bIsOpened = true;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Chest was already Opened! or OpenAnimation is not set!"));
	}
	
}



void AChest::SpawnRandomItem()
{
	if (PossibleItems.Num() > 0)
	{
		int32 RandomIndex = FMath::RandRange(0, PossibleItems.Num() - 1);
		TSubclassOf<AActor> SelectedItemClass = PossibleItems[RandomIndex];

		if (SelectedItemClass)
		{
			FVector SpawnLocation = GetActorLocation() + FVector(-10, 0, 110);
			FRotator SpawnRotation = FRotator::ZeroRotator;
			
			
			//If Bullet Item Spawned,
			if (SelectedItemClass->GetName().Contains("BulletItem"))
			{
				int32 BulletCount = FMath::RandRange(1, 5);
				//for space between bullet
				float Spacing = 23.0f;

				for (int32 i = 0; i < BulletCount; i++)
				{

					FVector BulletSpawnLocation = SpawnLocation + FVector(i*Spacing, 0, 0);
					AActor* SpawnedBullet = GetWorld()->SpawnActor<AActor>(SelectedItemClass, BulletSpawnLocation, SpawnRotation);
				
				}
				UE_LOG(LogTemp, Warning, TEXT("Spawned %d BulletItems"), BulletCount);
			}
			else
			//If Healitem Item Spawned,
			{
				AActor* SpawnedHealItem = GetWorld()->SpawnActor<AActor>(SelectedItemClass, SpawnLocation, SpawnRotation);
				
				UE_LOG(LogTemp, Warning, TEXT("Spawned 1 HealItem"));
			}

			UE_LOG(LogTemp, Warning, TEXT("Spawned Item: %s"), *SelectedItemClass->GetName());

		}
	}
}
