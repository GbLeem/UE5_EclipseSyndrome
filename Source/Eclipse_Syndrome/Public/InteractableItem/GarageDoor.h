#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TimerManager.h"
#include "Components/TimelineComponent.h"
#include "Components/BoxComponent.h"
#include "GarageDoor.generated.h"

class UBoxComponent;
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	UBoxComponent* InteractionZone;

	FVector ClosedPosition;
	FVector TargetPosition;

	bool bIsOpening = false;
	bool bCanUseKey = false;


	UPROPERTY(EditAnywhere)
	float Speed = 2.0f;

	UFUNCTION()
	void OnOverlapBegin(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);


	UFUNCTION()
	void OnOverlapEnd(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

	UFUNCTION()
	bool UnlockDoor();

	UFUNCTION()
	void OpenGarageDoor();

	virtual void Tick(float DeltaTime) override;

	void MoveDoorUp(float DeltaTime);
};
