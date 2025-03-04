// Fill out your copyright notice in the Description page of Project Settings.


#include "Volume/LevelChangeActor.h"

#include "Character/PlayerCharacter.h"
#include "Weapon/Weapon.h"
#include "Components/SphereComponent.h"
#include "System/DefaultGameState.h"
#include "System/DefaultGameInstance.h"

ALevelChangeActor::ALevelChangeActor()
	:CollisionRadius(100.f)
{
	PrimaryActorTick.bCanEverTick = true;

	SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("Scene Component"));
	RootComponent = SceneComp;

	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Collision"));
	CollisionComp->SetupAttachment(RootComponent);
	CollisionComp->SetCollisionProfileName(TEXT("OverlapAllDynamic"));

	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	StaticMeshComp->SetupAttachment(CollisionComp);
	StaticMeshComp->SetCollisionProfileName(TEXT("OverlapAllDynamic"));

	CollisionComp->InitSphereRadius(CollisionRadius);
	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &ALevelChangeActor::OnItemOverlap);
}

void ALevelChangeActor::OnItemOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->ActorHasTag("Player"))
	{
		if (UWorld* World = GetWorld())
		{
			/*if (UDefaultGameInstance* GameInstance = World->GetGameInstance<UDefaultGameInstance>())
			{
				if (Cast<APlayerCharacter>(OtherActor)->CurrentWeapon)
				{
					GameInstance->SetCurrentWeaponAmmo(Cast<APlayerCharacter>(OtherActor)->CurrentWeapon->GetCurrentAmmo());
				}
			}*/
			if (ADefaultGameState* GameState = World->GetGameState<ADefaultGameState>())
			{				
				GameState->LevelChange();
			}
		}
	}
}
