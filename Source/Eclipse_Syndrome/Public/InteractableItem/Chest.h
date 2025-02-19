#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Chest.generated.h"

UCLASS()
class ECLIPSE_SYNDROME_API AChest : public AActor
{
	GENERATED_BODY()
	
public:	
	AChest();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

};
