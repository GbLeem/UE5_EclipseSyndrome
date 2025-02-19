#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Chest.generated.h"

UCLASS()
class ECLIPSE_SYNDROME_API AChest : public AActor
{
	GENERATED_BODY()
	
public:	
	AChest();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chest")
	USceneComponent* SceneRootComp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chest")
	USkeletalMeshComponent* SkeletalMeshComp;

};
