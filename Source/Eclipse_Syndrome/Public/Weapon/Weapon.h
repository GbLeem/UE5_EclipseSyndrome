#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

UCLASS()
class ECLIPSE_SYNDROME_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	AWeapon();

protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable)
	void Fire();
	UFUNCTION(BlueprintCallable)
	void Reload();

private:
    UPROPERTY(EditDefaultsOnly, Category = "Components")
    UStaticMeshComponent* GunMesh;

    UPROPERTY(EditAnywhere, Category = "Gun Settings")
    float FireRange = 5000.f;

    UPROPERTY(EditAnywhere, Category = "Gun Settings")
    float Damage = 10.f;

    UPROPERTY(EditAnywhere, Category = "Gun Settings")
    float FireRate = 0.2f;

    UPROPERTY(EditAnywhere, Category = "Gun Settings")
    int32 MaxAmmo = 30;

    int32 CurrentAmmo;
};
