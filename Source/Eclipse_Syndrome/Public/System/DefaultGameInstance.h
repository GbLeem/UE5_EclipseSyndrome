#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "DefaultGameInstance.generated.h"

//[YJfixing]
UENUM(BlueprintType)
enum class EItemType : uint8
{
	None UMETA(DisplayName = "None"),
	Key UMETA(DisplayName = "Key"),
	PuzzleBlock UMETA(DisplayName = "PuzzleBlock")
};


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
	//[YJfixing]
	UFUNCTION()
	void AddItem(int32 ItemIdx, int32 ItemAmount, EItemType ItemType = EItemType::None, int32 BlockID = -1);
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
	//[YJfixing]
	UFUNCTION()
	bool AddSpecialItem(int32 ItemID);
	UFUNCTION()
	void RemoveSpecialItem();

public:
	//for HUD ammo text ui
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PlayerData")
	int32 InventoryAmmo;	

	//for Inventory UI
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PlayerData")
	TMap<int32, int32> InventoryItem;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PlayerData")
	TArray<int32> WeaponInventoryItem;

	//for block ID
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PlayerData")
	TMap<int32, int32> PuzzleBlockIDMap;

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

	//[YJfixing]for 3rd inventory slot(key = 100, gear = 1~5, empty = -1)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category ="PlayerData")
	int32 SpecialSlotItemID;
private:

};
