#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "DefaultGameInstance.generated.h"

UCLASS()
class ECLIPSE_SYNDROME_API UDefaultGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UDefaultGameInstance();

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PlayerData")
	int32 CurrentInventoryAmmo;
};
