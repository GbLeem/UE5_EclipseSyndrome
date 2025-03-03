#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PowerPlug.generated.h"

class UCableComponent;

UCLASS()
class ECLIPSE_SYNDROME_API APowerPlug : public AActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PowerPlug|Components")
	TObjectPtr<UStaticMeshComponent> StaticMeshCompo;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PowerPlug|Components")
	TObjectPtr<UCableComponent> CableCompo;
	
public:	
	APowerPlug();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

};
