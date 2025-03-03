#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractableItem/ControlPannel.h"
#include "PowerPlug.generated.h"

class UCableComponent;

UCLASS()
class ECLIPSE_SYNDROME_API APowerPlug : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PowerPlug|Components")
	TObjectPtr<UStaticMeshComponent> StaticMeshCompo;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PowerPlug|Components")
	TObjectPtr<UCableComponent> CableCompo;
	

	APowerPlug();

protected:
	virtual void BeginPlay() override;


	UFUNCTION()
	void OnOverlapBegin(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);
public:	
	void AttachToPanel(AControlPannel* ControlPannel);
};
