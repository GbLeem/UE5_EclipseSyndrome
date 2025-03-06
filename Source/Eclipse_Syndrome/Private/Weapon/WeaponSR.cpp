#include "Weapon/WeaponSR.h"
#include "GameFramework/SpringArmComponent.h"

AWeaponSR::AWeaponSR()
{
    FireRate = 1.f;
    FireRange = 6000.f;
    Damage = 35.f;
    MaxAmmo = 15;
    bAutoFire = false;
    bIsPeeking = false;
    WeaponNumber = 3;

    MinPitchRecoil = 2.5f;
    MaxPitchRecoil = 4.5f;
    MinYawRecoil = 1.2f;
    MaxYawRecoil = 3.2f;

    static ConstructorHelpers::FObjectFinder<UStaticMesh>StaticMeshAsset(TEXT("/Game/HJ/Assets/FPS_Weapon_Bundle/Weapons/Meshes/KA_Val/SM_KA_Val_Y.SM_KA_Val_Y"));
    if (StaticMeshAsset.Succeeded())
    {
        GunMesh->SetStaticMesh(StaticMeshAsset.Object);
    }   

    //sound asset
    static ConstructorHelpers::FObjectFinder<USoundBase>ShootAsset1(TEXT("/Game/HJ/Assets/Sound/fire.fire"));
    if (ShootAsset1.Succeeded())
    {
        ShootSound.Add(ShootAsset1.Object);
    }

    static ConstructorHelpers::FObjectFinder<USoundBase>ReloadAsset(TEXT("/Game/HJ/Assets/Sound/reload.reload"));
    if (ReloadAsset.Succeeded())
    {
        ReloadSound.Add(ReloadAsset.Object);
    }
 
    WeaponSpringArmComp->SocketOffset = FVector(10.f, 0.f, 15.f);
}
