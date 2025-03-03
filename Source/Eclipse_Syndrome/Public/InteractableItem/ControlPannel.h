#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ControlPannel.generated.h"



class UBoxComponent;
class UStaticMeshComponent;
class UMaterialInstanceDynamic;


UCLASS()
class ECLIPSE_SYNDROME_API AControlPannel : public AActor
{
	GENERATED_BODY()
	
	
public:	
	AControlPannel();

	UFUNCTION()
	void ActivatePanel(bool IsActivated);



protected:
	virtual void BeginPlay() override;
	
	
	UPROPERTY(VisibleAnywhere)
	UBoxComponent* CollisionBox;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* ControlPanelMesh;

	//for the red and green button
	UMaterialInstanceDynamic* RedMID;
	UMaterialInstanceDynamic* GreenMID;

	bool bIsActivated = false;

	UFUNCTION()
	void OnOverlapBegin(
		UPrimitiveComponent* Overlappedcomponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);
};
