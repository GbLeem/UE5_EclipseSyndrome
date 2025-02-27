#include "Weapon/WeaponSR.h"

AWeaponSR::AWeaponSR()
{
    FireRate = 10.f;
    FireRange = 6000.f;
    Damage = 35.f;
    MaxAmmo = 15;
    CurrentAmmo = 15;
    bAutoFire = false;
    bIsPeeking = false;
    WeaponNumber = 3;

    MinPitchRecoil = 3.5f;
    MaxPitchRecoil = 5.5f;
    MinYawRecoil = 2.2f;
    MaxYawRecoil = 4.2f;

    static ConstructorHelpers::FObjectFinder<UStaticMesh>StaticMeshAsset(TEXT("/Game/HJ/Assets/FPS_Weapon_Bundle/Weapons/Meshes/KA_Val/SM_KA_Val_Y.SM_KA_Val_Y"));
    if (StaticMeshAsset.Succeeded())
    {
        GunMesh->SetStaticMesh(StaticMeshAsset.Object);
    }   
}
