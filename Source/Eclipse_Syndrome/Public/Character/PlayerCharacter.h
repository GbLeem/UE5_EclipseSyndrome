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

	//crouch animation -> fail
	//virtual void Landed(const FHitResult& Hit) override;

	UFUNCTION()
	void Shoot();
	void ResetShoot() { bCanFire = true; }
	UFUNCTION()
	void Reloading();

	//recoil
	UFUNCTION()
	void Recoil();
	void CalculateRecoilValue();

	//call item's activate Item
	void BeginTraceForPickItem();
	void StartPeek();
	void StopPeek();
	void GrappleStart();
	UFUNCTION(BlueprintCallable)
	void GrappleEnd();

	UFUNCTION(BlueprintCallable)
	void EquipWeaponBack(int32 WeaponIdx);
	UFUNCTION(BlueprintCallable)
	void UseHealthItem();

	//getter
	int32 GetCurrentWeaponAmmo();

	//for possess drone
	void SetEnhancedInput();

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, 
		AController* EventInstigator, AActor* DamageCauser) override;

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
	UFUNCTION()
	void ShowInventory(const FInputActionValue& value);
	UFUNCTION()
	void PossessToDrone(const FInputActionValue& value);
	UFUNCTION()
	void DroneMoveCommand(const FInputActionValue& value);	
	UFUNCTION()
	void ChangeView(const FInputActionValue& value);
	UFUNCTION()
	void ZoomInOut(const FInputActionValue& value);
	UFUNCTION()
	void CrouchCharacter(const FInputActionValue& value);


//variables
public:
	//components
	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
	TObjectPtr<USpringArmComponent> SpringArmComp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
	TObjectPtr<UCameraComponent> CameraComp;*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
	TObjectPtr<UCableComponent> CableComp;
	


	//change view component
	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
	TObjectPtr<USpringArmComponent> FPSSpringArmComp;*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
	TObjectPtr<USpringArmComponent> TPSSpringArmComp;
	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
	TObjectPtr<UChildActorComponent> FPSCamera;*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
	TObjectPtr<UChildActorComponent> TPSCamera;

	//for character speed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float SprintSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float NormalSpeed;

	//for character item (inventory ammo)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 CurrentInventoryAmmos;

	//for animation
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	int32 BlendPoseVariable;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	bool bIsWeaponEquipped; //for change character animation(idle<->weapon)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	bool bCanGrapple; //can move character(attach to grapple actor)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	TObjectPtr<UAnimMontage> ReloadAnimMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	TObjectPtr<UAnimMontage> DamageAnimMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	TObjectPtr<UAnimMontage> RollingAnimMontage;

	//for equip weapon
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<AWeapon> CurrentWeapon;	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	TMap<int32, AWeapon*> PlayerWeaponInventory;

	//for reloading notify
	bool bIsReloading;

	//for rolling notify
	bool bIsRolling;

private:
	bool bCanFire;  //character flag
	bool bCanReload; //for reloading animation
	bool bCanTraceForItemPeeking; //if true, trace start
	bool bIsWeaponEquippedBack; //[TEMP]

	FTimerHandle FireRateTimerHandle;
	FTimerHandle GrappleTimerHandle;
	TObjectPtr<AActor> PeekingItem;
	FHitResult GrappleHitPoint; //grapple target actor point
	float GrappleEndTime;

	//recoil value
	float MinPitchRecoil;
	float MaxPitchRecoil;
	float MinYawRecoil;
	float MaxYawRecoil;

public:
	//camera change value
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	bool bIsTPSMode;

	//Crouch System
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	bool bIsCrouch;

	//sprint only forward 
	bool bMoveForward;

	FVector Velocity;
};
