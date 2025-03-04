#pragma once

#include "CoreMinimal.h"
#include "Character/PlayerCharacter.h"
#include "Drone/Drone.h"
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

	virtual void BeginPlay() override;
	void UpdateHUD();

	void SetDrone(const TObjectPtr<ADrone>& TargetDrone) { Drone = TargetDrone; }
	const TObjectPtr<ADrone>& GetDrone() const { return Drone; }
	void SetPlayerCharacter(const TObjectPtr<APlayerCharacter>& TargetCharacter) { PlayerPawn = TargetCharacter;}
	const TObjectPtr<APlayerCharacter>& GetPlayerCharacter() const { return PlayerPawn; }
	
	//level
	void LevelChange();
	void GameClear();
	void GameOver();


//variables
public:
	UPROPERTY(VisibleAnywhere, Category = "Possess")
	TObjectPtr<ADrone> Drone;
	UPROPERTY(VisibleAnywhere, Category = "Possess")
	TObjectPtr<APlayerCharacter> PlayerPawn;

	//for level
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level");
	int32 CurrentLevelIndex;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level");
	int32 MaxLevelIndex;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level");
	TArray<FName> LevelMapNames;

private:
	FTimerHandle HUDUpdateTimerHandle;
};
