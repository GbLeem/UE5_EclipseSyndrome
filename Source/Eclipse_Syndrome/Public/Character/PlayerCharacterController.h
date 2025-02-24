#pragma once

#include "CoreMinimal.h"
#include "Drone/DroneAIController.h"
#include "GameFramework/PlayerController.h"
#include "PlayerCharacterController.generated.h"

class APlayerCharacter;
class UInputMappingContext;
class UInputAction;

UCLASS()
class ECLIPSE_SYNDROME_API APlayerCharacterController : public APlayerController
{
	GENERATED_BODY()

private:
	TObjectPtr<APawn> PlayerPawn;

	void HandleDronePossess(const TObjectPtr<ADrone>& DronePawn, const TObjectPtr<APawn>& PrevPawn);
	void HandlePlayerPossess(const TObjectPtr<APlayerCharacter>& NewPlayerPawn, const TObjectPtr<APawn>& PrevPawn);
	void SpawnDummyAIForPawn(const TObjectPtr<APlayerCharacter>& PrevPlayerPawn);
	
public:
	APlayerCharacterController();
	UUserWidget* GetHUDWidget() { return HUDWidgetInstance; }

	void SetPlayerPawn(const TObjectPtr<APawn>& PlayerCharacterPawn) { PlayerPawn = PlayerCharacterPawn; }
	APawn* GetPlayerPawn() const { return PlayerPawn; }
	void ChangePossess(const TObjectPtr<APawn>& NewPawn);
	void ChangeMappingContext(int Type);

protected:
	virtual void BeginPlay() override;

//variables
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> DefaultInputMappingContext;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> LookAction;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> JumpAction;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> SprintAction;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> ShootAction;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> ReloadAction;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> PickUpAction;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> EquipWeapon1Action;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> GrappleAction;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> PossessAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DroneInput|Property")
	TSubclassOf<ADroneAIController> DroneAIClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DroneInput|Property")
	TObjectPtr<UInputMappingContext> DroneInputMappingContext;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DroneInput|Property")
	TObjectPtr<UInputAction> DroneMoveAction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DroneInput|Property")
	TObjectPtr<UInputAction> DroneLookAction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DroneInput|Property")
	TObjectPtr<UInputAction> DronePossessAction;
	
	//UI
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "HUD")
	TSubclassOf<UUserWidget> HUDWidgetClass;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "HUD")
	TObjectPtr<UUserWidget> HUDWidgetInstance;
};
