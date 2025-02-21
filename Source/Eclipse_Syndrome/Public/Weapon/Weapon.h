#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

class USphereComponent;

UCLASS()
class ECLIPSE_SYNDROME_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	AWeapon();

public:
	UFUNCTION(BlueprintCallable)
	void Fire();
	UFUNCTION(BlueprintCallable)
	void Reload();
	void DestroyItem();
	// TEST  Crosshair
	bool GetAimHitResult(FHitResult& OutHitResult);

	//TEST
	UFUNCTION()
	virtual void OnItemOverlap
	(
		UPrimitiveComponent* OverlappedComp
		, AActor* OtherActor
		, UPrimitiveComponent* OtherComp
		, int32 OtherBodyIndex
		, bool bFromSweep
		, const FHitResult& SweepResult
	);
	UFUNCTION()
	virtual void OnItemEndOverlap
	(
		UPrimitiveComponent* OverlappedComp
		, AActor* OtherActor
		, UPrimitiveComponent* OtherComp
		, int32 OtherBodyIndex
	);

private:
    UPROPERTY(EditDefaultsOnly, Category = "Components")
    UStaticMeshComponent* GunMesh;

	//TEST Muzzlepoint
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	USceneComponent* Muzzlepoint;
	//TEST MuzzleFlash
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	UParticleSystem* MuzzleFlash;
	//TEST
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	TObjectPtr<USphereComponent> CollisionComp;

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
