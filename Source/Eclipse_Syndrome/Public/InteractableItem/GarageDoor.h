#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GarageDoor.generated.h"

UCLASS()
class ECLIPSE_SYNDROME_API AGarageDoor : public AActor
{
	GENERATED_BODY()
	
public:	
	AGarageDoor();

protected:
	virtual void BeginPlay() override;

public:	
	
	UPROPERTY(EditAnywhere)
	FString RequiredKeyName = "StorageKey";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Door")
	USkeletalMeshComponent* GDoorMesh;

	
	UFUNCTION()
	void OpenGarageDoor();
};
