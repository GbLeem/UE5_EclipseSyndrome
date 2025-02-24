#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

class USphereComponent;
class UWidgetComponent;

UCLASS()
class ECLIPSE_SYNDROME_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	AWeapon();
	virtual void Tick(float DeltaTime) override;

public:
	UFUNCTION(BlueprintCallable)
	void Fire();
	UFUNCTION(BlueprintCallable)
	void Reload(int32 Amount);
	void DestroyItem();
	// TEST  Crosshair
	bool GetAimHitResult(FHitResult& OutHitResult);
	FVector CalculateDestination();
	//for ui
	void ShowUI();
	void StopUI();

	bool GetAutoFire() { return bAutoFire; }
	int32 GetCurrentAmmo() { return CurrentAmmo; }
	int32 GetMaxAmmo() { return MaxAmmo; }
	float GetFireRate() { return FireRate; }
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

protected:
    UPROPERTY(EditDefaultsOnly, Category = "Components")
    UStaticMeshComponent* GunMesh;

	//TEST MuzzleFlash
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	UParticleSystem* MuzzleFlash;
	//TEST
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	TObjectPtr<USphereComponent> CollisionComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TObjectPtr<UWidgetComponent> ItemHoverUI;

	//[FOR GUN's feature]
    UPROPERTY(EditAnywhere, Category = "Gun Settings")
    float FireRange;
    UPROPERTY(EditAnywhere, Category = "Gun Settings")
    float Damage;
    UPROPERTY(EditAnywhere, Category = "Gun Settings")
	float FireRate;
    UPROPERTY(EditAnywhere, Category = "Gun Settings")
	int32 MaxAmmo;
	UPROPERTY(EditAnywhere, Category = "Gun Settings")
	bool bAutoFire;


private:
    int32 CurrentAmmo;	

public:
	bool bIsPeeking;
};
