#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Math/UnrealMathUtility.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/BoxComponent.h"
#include "Door.generated.h"


UCLASS()
class ECLIPSE_SYNDROME_API ADoor : public AActor
{
	GENERATED_BODY()
	
public:	

	ADoor();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Door")
	UStaticMeshComponent* DoorMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	UBoxComponent* CollisionBox;

	bool bIsOpen = false;



	UPROPERTY(EditAnywhere, Category="Door Rotation")
	float MoveSpeed;
	UPROPERTY(EditAnywhere, Category = "Door Rotation")
	float RotationSpeed; 
	UPROPERTY(EditAnywhere, Category = "Sound")
	USoundBase* IDoorOpeningSound;

	bool bIsOpening = false;
	
	
	FVector OpenLocation;
	FRotator OpenRotation;
	FVector ClosedLocation;
	FRotator ClosedRotation;

	float MoveDistance;


	UFUNCTION()
	void OpenDoor();

	void MoveAndRotateDoor(float DeltaTime);

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
};
