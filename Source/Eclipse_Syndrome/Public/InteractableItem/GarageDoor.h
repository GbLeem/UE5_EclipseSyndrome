#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TimerManager.h"
#include "Components/TimelineComponent.h"
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

	FVector ClosedPosition;
	FVector TargetPosition;

	bool bIsOpening = false;
	
	UPROPERTY(EditAnywhere)
	float Speed = 2.0f;


	UFUNCTION()
	void OpenGarageDoor();

	virtual void Tick(float DeltaTime) override;

	void MoveDoorUp(float DeltaTime);
};
