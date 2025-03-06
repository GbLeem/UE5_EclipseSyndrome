#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

class USphereComponent;
class UWidgetComponent;
class UMaterialInterface;
class USpringArmComponent;
class UNiagaraSystem;

UCLASS()
class ECLIPSE_SYNDROME_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	AWeapon();
	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable)
	virtual void Fire();
	UFUNCTION(BlueprintCallable)
	virtual void Reload(int32 Amount);

	//for shooting
	FVector CalculateDestination();

	//for ui
	void ShowUI();
	void StopUI();

	//getter
	bool GetAutoFire() { return bAutoFire; }
	int32 GetCurrentAmmo() { return CurrentAmmo; }
	int32 GetMaxAmmo() { return MaxAmmo; }
	float GetFireRate() { return FireRate; }
	int32 GetWeaponNumber() { return WeaponNumber; }
	FTransform GetHandSocket();

	//setter
	//void SetCurrentAmmo(int32 Amount) { CurrentAmmo = Amount; }

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
	
	//overlap check
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	TObjectPtr<USphereComponent> CollisionComp;

	//ui
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TObjectPtr<UWidgetComponent> ItemHoverUI;
	
	//Decal
	TObjectPtr<UMaterialInterface> BulletDecal;

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
	UPROPERTY(EditAnywhere, Category = "Gun Settings")
    int32 CurrentAmmo;	
	UPROPERTY(EditAnywhere, Category = "Gun Settings")
	int32 WeaponNumber;	


public:
	bool bIsPeeking;
	FVector RecoilValue;

	float MinPitchRecoil;
	float MaxPitchRecoil;
	float MinYawRecoil;
	float MaxYawRecoil;

	//for zoom in
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component")
	TObjectPtr<USpringArmComponent> WeaponSpringArmComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component")
	TObjectPtr<UChildActorComponent> WeaponCameraComp;

	//for effect
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Components")
	TObjectPtr<UNiagaraSystem> BloodNiagara;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Components")
	TObjectPtr<UNiagaraSystem> MuzzleNiagara;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Components")
	TObjectPtr<UNiagaraSystem> BulletNiagara;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Components")
	TObjectPtr<UNiagaraSystem> LandNiagara;


	//for weapon sound
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	TArray<TObjectPtr<USoundBase>> ShootSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	TArray<TObjectPtr<USoundBase>> ReloadSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	TObjectPtr<USoundBase> NoAmmoSound;
};
