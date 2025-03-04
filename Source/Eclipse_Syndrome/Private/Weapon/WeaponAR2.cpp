#include "Weapon/WeaponAR2.h"
#include "GameFramework/SpringArmComponent.h"

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

    MinPitchRecoil = -0.2f;
    MaxPitchRecoil = 0.4f;
    MinYawRecoil = -0.2f;
    MaxYawRecoil = 0.4f;

    static ConstructorHelpers::FObjectFinder<UStaticMesh>StaticMeshAsset(TEXT("/Game/HJ/Assets/FPS_Weapon_Bundle/Weapons/Meshes/Ka47/SM_KA47.SM_KA47"));
    if (StaticMeshAsset.Succeeded())
    {
        GunMesh->SetStaticMesh(StaticMeshAsset.Object);
    }

    WeaponSpringArmComp->SetRelativeLocation(FVector((0.f, 9.5f, 10.3f)));
    WeaponSpringArmComp->bUsePawnControlRotation = true;
}

void AWeaponAR2::BeginPlay()
{
    Super::BeginPlay();

    WeaponSpringArmComp->SetRelativeLocation(FVector((0.f, 9.5f, 10.3f)));
    WeaponSpringArmComp->bUsePawnControlRotation = true;
}