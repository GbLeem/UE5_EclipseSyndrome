#include "InteractableItem/Chest.h"


AChest::AChest()
{
	PrimaryActorTick.bCanEverTick = false;
	SceneRootComp = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRootComp);

	SkeletalMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMeshComp->SetupAttachment(SceneRootComp);
}
