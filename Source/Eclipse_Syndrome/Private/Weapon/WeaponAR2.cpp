#include "Weapon/WeaponAR2.h"
#include "GameFramework/SpringArmComponent.h"

AWeaponAR2::AWeaponAR2()
{
    FireRate = 0.1f;
    FireRange = 4000.f;
    Damage = 15.f;
    MaxAmmo = 25;
    bAutoFire = true;
    bIsPeeking = false;
    WeaponNumber = 2;

    MinPitchRecoil = -0.2f;
    MaxPitchRecoil = 0.4f;
    MinYawRecoil = -0.2f;
    MaxYawRecoil = 0.4f;

    WeaponSpringArmComp->SocketOffset = FVector(10.f, 0.f, 15.f);    

    static ConstructorHelpers::FObjectFinder<UStaticMesh>StaticMeshAsset(TEXT("/Game/HJ/Assets/FPS_Weapon_Bundle/Weapons/Meshes/Ka47/SM_KA47.SM_KA47"));
    if (StaticMeshAsset.Succeeded())
    {
        GunMesh->SetStaticMesh(StaticMeshAsset.Object);
    }

    //sound asset
    static ConstructorHelpers::FObjectFinder<USoundBase>ShootAsset1(TEXT("/Game/HJ/Assets/Sound/Smith_Wesson_OneShot_Close_2.Smith_Wesson_OneShot_Close_2"));
    if (ShootAsset1.Succeeded())
    {
        ShootSound.Add(ShootAsset1.Object);
    }
    static ConstructorHelpers::FObjectFinder<USoundBase>ShootAsset2(TEXT("/Game/HJ/Assets/Sound/Smith_Wesson_OneShot_Close_1.Smith_Wesson_OneShot_Close_1"));
    if (ShootAsset2.Succeeded())
    {
        ShootSound.Add(ShootAsset2.Object);
    }

    static ConstructorHelpers::FObjectFinder<USoundBase>ReloadAsset(TEXT("/Game/HJ/Assets/Sound/reload.reload"));
    if (ReloadAsset.Succeeded())
    {
        ReloadSound.Add(ReloadAsset.Object);
    }
}