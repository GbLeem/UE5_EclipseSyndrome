#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"

#include "ControlPannel.generated.h"



class UBoxComponent;
class UStaticMeshComponent;
class UMaterialInstanceDynamic;
class ALevelTwoDoor;

UCLASS()
class ECLIPSE_SYNDROME_API AControlPannel : public AActor
{
	GENERATED_BODY()
	
	
public:	
	AControlPannel();

	UPROPERTY(EditAnywhere, Category = "Effects")
	UNiagaraSystem* NiagaraEffect;
	UPROPERTY()
	UNiagaraComponent* NiagaraComp;

	//UPROPERTY(EditAnywhere)
	//UMaterialInterface* OutlineMaterial;

	bool bIsPlugConnected = false;




	UFUNCTION()
	void ActivatePanel(bool IsActivated);

	UFUNCTION(BlueprintCallable)
	FVector GetPlugPosition() const;

	UFUNCTION(BlueprintCallable)
	FQuat GetPlugRotation() const;


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

	ALevelTwoDoor* LevelTwoDoor;

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
