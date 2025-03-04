#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DefaultBullet.generated.h"

class UProjectileMovementComponent;
class UNiagaraComponent;
class UBoxComponent;

UCLASS()
class ECLIPSE_SYNDROME_API ADefaultBullet : public AActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bullet|Components")
	TObjectPtr<UBoxComponent> BoxComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bullet|Components")
	TObjectPtr<UNiagaraComponent> NiagaraComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bullet|Components")
	TObjectPtr<UProjectileMovementComponent> ProjectileMovementComp;
	
public:	
	ADefaultBullet();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	void FireInDirection(const FVector& ShootDirection);
};
