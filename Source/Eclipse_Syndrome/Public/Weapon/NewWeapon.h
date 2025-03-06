#pragma once

#include "CoreMinimal.h"
#include "Weapon/Weapon.h"
#include "NewWeapon.generated.h"

class USceneCaptureComponent2D;

UCLASS()
class ECLIPSE_SYNDROME_API ANewWeapon : public AWeapon
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANewWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	TObjectPtr<UStaticMeshComponent> Lens;
	TObjectPtr<UStaticMeshComponent> Scope;

	TObjectPtr<USceneCaptureComponent2D> CaptureComp;
};
