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
	ANewWeapon();
	virtual void BeginPlay() override;

public:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> Lens;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> Scope;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneCaptureComponent2D> CaptureComp;
};
