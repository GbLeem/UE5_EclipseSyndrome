// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/WeaponAR1.h"

AWeaponAR1::AWeaponAR1()
{
    FireRate = 0.05f;
    FireRange = 5000.f;
    Damage = 10.f;
    MaxAmmo = 30;
    CurrentAmmo = 30;
    bAutoFire = true;
    bIsPeeking = false;
    WeaponNumber = 1;

    static ConstructorHelpers::FObjectFinder<UStaticMesh>StaticMeshAsset(TEXT("/Game/HJ/Assets/QuantumCharacter/Mesh/Rifle/SM_Rifle_Olive.SM_Rifle_Olive"));
    if (StaticMeshAsset.Succeeded())
    {
        GunMesh->SetStaticMesh(StaticMeshAsset.Object);
    }
}
