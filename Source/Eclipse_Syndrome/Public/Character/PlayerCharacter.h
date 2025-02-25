#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
struct FInputActionValue;
class ABaseItem;
class AWeapon;
class UCableComponent;

UCLASS()
class ECLIPSE_SYNDROME_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	APlayerCharacter();
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;

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
	void GrappleStart();
	UFUNCTION(BlueprintCallable)
	void GrappleEnd();
	void EquipWeaponBack();

	//getter
	int32 GetCurrentWeaponAmmo();
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
	void StopShoot(const FInputActionValue& value);
	UFUNCTION()
	void PickUp(const FInputActionValue& value);
	UFUNCTION()
	void EquipWeapon1(const FInputActionValue& value);
	UFUNCTION()
	void Grapple(const FInputActionValue& value);


//variables
public:
	//components
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
	TObjectPtr<USpringArmComponent> SpringArmComp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
	TObjectPtr<UCameraComponent> CameraComp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
	TObjectPtr<UCableComponent> CableComp;

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
	int32 CurrentInventoryAmmos;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	TArray<ABaseItem*> Inventory;

	//for animation
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	int32 BlendPoseVariable;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	bool bIsWeaponEquipped; //for change character animation(idle<->weapon)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	bool bCanGrapple; //can move character(attach to grapple actor)

	void SetEnhancedInput();
	
private:
	bool bCanFire;  //character flag
	bool bCanReload; //for reloading animation
	bool bCanTraceForItemPeeking; //if true, trace start

	FTimerHandle FireRateTimerHandle;
	FTimerHandle GrappleTimerHandle;
	TObjectPtr<AActor> PeekingItem;
	AWeapon* CurrentWeapon;
	FHitResult GrappleHitPoint; //grapple target actor point
	float GrappleEndTime;
	FRotator OriginRootRotator;
};
