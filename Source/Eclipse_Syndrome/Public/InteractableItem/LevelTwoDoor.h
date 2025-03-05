#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelTwoDoor.generated.h"

UCLASS()
class ECLIPSE_SYNDROME_API ALevelTwoDoor : public AActor
{
	GENERATED_BODY()
	
public:	

	ALevelTwoDoor();


protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* StaticMeshComp;
	UPROPERTY(EditAnywhere, Category = "Sound")
	USoundBase* IDoorOpeningSound;



	virtual void BeginPlay() override;

public:	

	virtual void Tick(float DeltaTime) override;
	void OpenLevelTwoDoor();


private:
	FVector ClosedPosition;
	FVector OpenPosition;
	float MoveSpeed = 1.0f;
	bool bIsOpening = false;
	
};
