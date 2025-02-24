#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "DefaultGameState.generated.h"

class ADroneAIController;
class ADroneController;
class APlayerCharacterController;

UCLASS()
class ECLIPSE_SYNDROME_API ADefaultGameState : public AGameState
{
	GENERATED_BODY()
	
public:
	ADefaultGameState();

	UFUNCTION(BlueprintPure, Category = "Score")
	int32 GetAmmo() const;

	virtual void BeginPlay() override;
	void UpdateHUD();

	//for possesss
	void ToggleControl();
	void ControlPlayer();
	void ControlDrone();

//variables
public:
	int32 CurrentInventoryAmmos;

	//possess
	UPROPERTY(EditAnywhere, Category = "DronePossess")
	APawn* PlayerCharacterP;

	UPROPERTY(EditAnywhere, Category = "DronePossess")
	APawn* Drone;

	UPROPERTY(EditAnywhere, Category = "DronePossess")
	TObjectPtr<APlayerCharacterController> PlayerCharacterControllerP;

	UPROPERTY(EditAnywhere, Category = "DronePossess")
	TObjectPtr<ADroneAIController> DroneAIController;

	UPROPERTY(EditAnywhere, Category = "DronePossess")
	TObjectPtr<ADroneController> DroneController;

	UPROPERTY(EditAnywhere, Category = "AI")
	TSubclassOf<ADroneController> DroneClass;

	bool bIsControllingDrone;

private:
	FTimerHandle HUDUpdateTimerHandle;
};
