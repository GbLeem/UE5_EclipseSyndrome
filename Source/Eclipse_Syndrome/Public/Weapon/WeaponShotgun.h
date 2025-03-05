// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/Weapon.h"
#include "WeaponShotgun.generated.h"

/**
 * 
 */
UCLASS()
class ECLIPSE_SYNDROME_API AWeaponShotgun : public AWeapon
{
	GENERATED_BODY()
	
public:
	AWeaponShotgun();

	virtual void Fire() override;

};
