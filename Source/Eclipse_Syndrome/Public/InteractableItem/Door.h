#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Door.generated.h"


UCLASS()
class ECLIPSE_SYNDROME_API ADoor : public AActor
{
	GENERATED_BODY()
	
public:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Door")
	UStaticMeshComponent* DoorMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	UBoxComponent* CollisionBox;

	bool bIsOpen = false;

	FTimerHandle DoorOpenTimer;
	float TotalRotationTime = 1.5f;
	float RotationSpeed = 90.0f;
	float TargetYaw = 90.0f;


	ADoor();

	void OpenDoor();

	void RotateDoor();
};
