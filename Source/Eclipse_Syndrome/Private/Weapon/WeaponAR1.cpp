// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/WeaponAR1.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

AWeaponAR1::AWeaponAR1()
{
    FireRate = 0.05f;
    FireRange = 5000.f;
    Damage = 10.f;
    MaxAmmo = 30;
    bAutoFire = true;
    bIsPeeking = false;
    WeaponNumber = 1;

    MinPitchRecoil = -0.1f; 
    MaxPitchRecoil = 0.3f;
    MinYawRecoil = -0.1f;
    MaxYawRecoil = 0.2f;
    
    static ConstructorHelpers::FObjectFinder<UStaticMesh>StaticMeshAsset(TEXT("/Game/HJ/Assets/QuantumCharacter/Mesh/Rifle/SM_Rifle_Olive.SM_Rifle_Olive"));
    if (StaticMeshAsset.Succeeded())
    {
        GunMesh->SetStaticMesh(StaticMeshAsset.Object);
    }    

    //sound
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

    WeaponSpringArmComp->SocketOffset = FVector(-8.f, 0.f, 20.f);
}
