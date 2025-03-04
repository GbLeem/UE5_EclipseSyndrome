#pragma once

#include "CoreMinimal.h"
#include "Weapon/Weapon.h"
#include "WeaponAR2.generated.h"


UCLASS()
class ECLIPSE_SYNDROME_API AWeaponAR2 : public AWeapon
{
	GENERATED_BODY()
	
public:
	AWeaponAR2();

	virtual void BeginPlay() override;

};
