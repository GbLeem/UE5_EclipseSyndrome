#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "DefaultGameInstance.generated.h"

class ABaseItem;

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
	void AddItem(ABaseItem* Item);

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PlayerData")
	int32 InventoryAmmo;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PlayerData")
	TArray<ABaseItem*> InventoryItem;
};
