#include "Weapon/WeaponAR2.h"

AWeaponAR2::AWeaponAR2()
{
    FireRate = 0.1f;
    FireRange = 4000.f;
    Damage = 15.f;
    MaxAmmo = 25;
    CurrentAmmo = 25;
    bAutoFire = true;
    bIsPeeking = false;
    WeaponNumber = 2;

    static ConstructorHelpers::FObjectFinder<UStaticMesh>StaticMeshAsset(TEXT("/Game/HJ/Assets/FPS_Weapon_Bundle/Weapons/Meshes/Ka47/SM_KA47_X.SM_KA47_X"));
    if (StaticMeshAsset.Succeeded())
    {
        GunMesh->SetStaticMesh(StaticMeshAsset.Object);
    }
    GunMesh->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));
}
