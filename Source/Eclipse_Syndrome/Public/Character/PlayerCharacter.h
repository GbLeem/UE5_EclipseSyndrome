#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
struct FInputActionValue;
class ABaseItem;
class AWeapon;

UCLASS()
class ECLIPSE_SYNDROME_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	APlayerCharacter();
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void Shoot();
	void ResetShoot() { bCanFire = true; }
	UFUNCTION()
	void Reloading();
	UFUNCTION()
	void PickUpItem();

	//call item's activate Item
	void BeginTraceForPickItem();
	void StartPeek();
	void StopPeek();

	virtual void BeginPlay() override;
protected:
	//for input action
	UFUNCTION()
	void Move(const FInputActionValue& value);
	UFUNCTION()
	void Look(const FInputActionValue& value);
	UFUNCTION()
	void StartJump(const FInputActionValue& value);
	UFUNCTION()
	void StopJump(const FInputActionValue& value);
	UFUNCTION()
	void StartSprint(const FInputActionValue& value);
	UFUNCTION()
	void StopSprint(const FInputActionValue& value);
	UFUNCTION()
	void Reload(const FInputActionValue& value);
	UFUNCTION()
	void StartShoot(const FInputActionValue& value);
	UFUNCTION()
	void StartShootAuto(const FInputActionValue& value);
	UFUNCTION()
	void StopShoot(const FInputActionValue& value);
	UFUNCTION()
	void PickUp(const FInputActionValue& value);
	UFUNCTION()
	void EquipWeapon1(const FInputActionValue& value);

//variables
public:
	//components
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
	TObjectPtr<USpringArmComponent> SpringArmComp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
	TObjectPtr<UCameraComponent> CameraComp;

	//for character speed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float SprintSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float NormalSpeed;

	//for character health
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float MaxHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float CurrentHealth;

	//for character item (inventory ammo)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 CurrentAmmos;

	//for test
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	float FireRate; //from gun class
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	bool bAutoFire; //from gun class
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	int32 BlendPoseVariable;

private:
	bool bCanFire;  //character flag
	bool bCanReload; //for reloading animation
	bool bCanTraceForItemPeeking;
	bool bIsWeaponEquipped; //for change character animation(idle<->weapon)

	//for test
	int32 GunCurrentAmmo;//from gun class
	int32 GunMaxAmmo;//from gun class

	FTimerHandle FireRateTimerHandle;
	TArray<ABaseItem*> Inventory;
	ABaseItem* PeekingItem;
	AWeapon* CurrentWeapon;
};
