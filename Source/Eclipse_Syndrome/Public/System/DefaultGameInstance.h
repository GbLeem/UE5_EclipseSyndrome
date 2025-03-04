#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "DefaultGameInstance.generated.h"

class ABaseItem;
class AWeapon;

UCLASS()
class ECLIPSE_SYNDROME_API UDefaultGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UDefaultGameInstance();

	UFUNCTION()
	void AddAmmo(int32 Amount);
	UFUNCTION()
	void UseAmmo(int32 Amount);
	UFUNCTION()
	void AddItem(int32 ItemIdx, int32 ItemAmount);
	UFUNCTION()
	void AddWeapon(int32 WeaponIdx);
	UFUNCTION()
	bool FindWeaponByIdx(int32 WeaponIdx);
	UFUNCTION()
	void PlusHealth(int32 Amount);
	UFUNCTION()
	void MinusHealth(int32 Amount);
	UFUNCTION()
	void SetCurrentWeapon(AWeapon* CurrentWeapon);
	UFUNCTION()
	void SetCurrentWeaponAmmo(int32 Amount);

public:
	//for HUD ammo text ui
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PlayerData")
	int32 InventoryAmmo;	

	//for Inventory UI
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PlayerData")
	TMap<int32, int32> InventoryItem;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PlayerData")
	TArray<int32> WeaponInventoryItem;

	//health
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PlayerData")
	float PlayerMaxHealth;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PlayerData")
	float PlayerCurrentHealth;

	//for level
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level");
	int32 CurrentLevel;	

	//for current weapon
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PlayerData")
	TObjectPtr<AWeapon> CurrentWeaponInstance;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PlayerData")
	int32 CurrentWeaponAmmo;

private:

};
