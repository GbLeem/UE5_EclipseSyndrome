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


//variables
public:
	int32 CurrentInventoryAmmos;


private:
	FTimerHandle HUDUpdateTimerHandle;
};
