#pragma once

#include "CoreMinimal.h"
#include "Item/BaseItem.h"
#include "BulletItem.generated.h"


UCLASS()
class ECLIPSE_SYNDROME_API ABulletItem : public ABaseItem
{
	GENERATED_BODY()

public:
	ABulletItem();

	//num of bullet
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	float NumOfBullet;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	UNiagaraComponent* GlowEffect;
	//UPROPERTY(EditAnywhere, Category = "Effects")
	//UNiagaraSystem* NiagaraEffect;


	UFUNCTION()
	virtual void ActivateItem(AActor* Activator) override;
	UFUNCTION()
	virtual void BeginPlay() override;
};
