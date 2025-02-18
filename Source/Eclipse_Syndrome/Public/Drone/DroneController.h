#pragma once
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "DroneController.generated.h"

class UInputMappingContext;
class UInputAction;

UCLASS()
class ECLIPSE_SYNDROME_API ADroneController : public APlayerController
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DroneInput|Property")
	TObjectPtr<UInputMappingContext> InputMappingContext;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DroneInput|Property")
	TObjectPtr<UInputAction> MoveAction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DroneInput|Property")
	TObjectPtr<UInputAction> LookAction;

public:
	ADroneController();

protected:
	virtual void BeginPlay() override;

	friend class ADrone;
};
