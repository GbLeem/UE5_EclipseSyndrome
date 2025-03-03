#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Time_TriggerVolimeActor.generated.h"

class UBoxComponent;

UCLASS()
class ECLIPSE_SYNDROME_API ATime_TriggerVolimeActor : public AActor
{
	GENERATED_BODY()

public:
	ATime_TriggerVolimeActor();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, Category = "Trigger")
	UBoxComponent* TriggerBox;
	UPROPERTY(EditAnywhere, Category = "Target")
	UStaticMeshComponent* TargetMesh;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
						UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
						bool bFromSweep, const FHitResult& SweepResult);

	void RemoveMesh();

};