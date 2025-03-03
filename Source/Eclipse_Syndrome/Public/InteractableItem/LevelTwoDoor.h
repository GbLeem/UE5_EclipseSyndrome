#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelTwoDoor.generated.h"

UCLASS()
class ECLIPSE_SYNDROME_API ALevelTwoDoor : public AActor
{
	GENERATED_BODY()
	
public:	

	ALevelTwoDoor();


protected:	
	virtual void BeginPlay() override;

public:	

	virtual void Tick(float DeltaTime) override;

};
