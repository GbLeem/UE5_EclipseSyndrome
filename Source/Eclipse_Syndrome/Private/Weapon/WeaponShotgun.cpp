#include "Weapon/WeaponShotgun.h"

#include "Weapon/WeaponShell.h"

#include "GameFramework/SpringArmComponent.h"
#include "Components/DecalComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

AWeaponShotgun::AWeaponShotgun()
{
    FireRate = 2.f;
    FireRange = 700.f;
    Damage = 20.f;
    MaxAmmo = 5;    
    bAutoFire = true;
    bIsPeeking = false;
    WeaponNumber = 4;

    MinPitchRecoil = -1.f;
    MaxPitchRecoil = 3.f;
    MinYawRecoil = -1.f;
    MaxYawRecoil = 2.f;

    static ConstructorHelpers::FObjectFinder<UStaticMesh>StaticMeshAsset(TEXT("/Game/HJ/Assets/package/static_mesh/StaticMesh.StaticMesh"));
    if (StaticMeshAsset.Succeeded())
    {
        GunMesh->SetStaticMesh(StaticMeshAsset.Object);
    }
    //sound asset
    static ConstructorHelpers::FObjectFinder<USoundBase>ShootAsset1(TEXT("/Game/HJ/Assets/Sound/ShotGun_Shot_Far_4.ShotGun_Shot_Far_4"));
    if (ShootAsset1.Succeeded())
    {
        ShootSound.Add(ShootAsset1.Object);
    }
    static ConstructorHelpers::FObjectFinder<USoundBase>ShootAsset2(TEXT("/Game/HJ/Assets/Sound/ShotGun_Shot_Punchy_2.ShotGun_Shot_Punchy_2"));
    if (ShootAsset2.Succeeded())
    {
        ShootSound.Add(ShootAsset2.Object);
    }

    static ConstructorHelpers::FObjectFinder<USoundBase>ReloadAsset(TEXT("/Game/HJ/Assets/Sound/ShotGun_Handling_Plastic_1.ShotGun_Handling_Plastic_1"));
    if (ReloadAsset.Succeeded())
    {
        ReloadSound.Add(ReloadAsset.Object);
    }    
    static ConstructorHelpers::FObjectFinder<USoundBase>NoAmmoAsset(TEXT("/Game/HJ/Assets/Sound/Smith_Wesson_Handling_1.Smith_Wesson_Handling_1"));
    if (NoAmmoAsset.Succeeded())
    {
        NoAmmoSound = NoAmmoAsset.Object;
    }

    static ConstructorHelpers::FObjectFinder<UNiagaraSystem>BulletAsset(TEXT("/Game/HJ/Material/NS_HJBullet3.NS_HJBullet3"));
    BulletNiagara = BulletAsset.Object;

    WeaponSpringArmComp->SocketOffset = FVector(10.f, 0.f, 15.f);
}

void AWeaponShotgun::Fire()
{
    FVector MuzzleLocation = GunMesh->GetSocketLocation(TEXT("MuzzleSocket"));
    FRotator MuzzleRotation = GunMesh->GetSocketRotation(TEXT("MuzzleSocket"));

    if (CurrentAmmo == 0)
    {
        UGameplayStatics::PlaySoundAtLocation(GetWorld(), NoAmmoSound, MuzzleLocation);
    }

    if (CurrentAmmo <= 0)
    {
        return;
    }

    CurrentAmmo--;

    //shoot sound
    if (ShootSound.Num() > 0)
    {
        int RandIdx = FMath::RandRange(0, ShootSound.Num() - 1);
        UGameplayStatics::PlaySoundAtLocation(GetWorld(), ShootSound[RandIdx], MuzzleLocation);
    }

    UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), MuzzleNiagara, MuzzleLocation, GetActorRotation(), FVector(0.5f, 0.5f, 0.5f));

    FVector FireDirection = CalculateDestination() - MuzzleLocation;
    FVector EndLocation = MuzzleLocation + FireDirection * FireRange;

    //shell effect    
    FVector ShellLocation = GunMesh->GetSocketLocation(TEXT("ShellSocket"));
    FRotator ShellRotation = GunMesh->GetSocketRotation(TEXT("ShellSocket"));

    FActorSpawnParameters SpawnParams;
    AWeaponShell* ShellActor = GetWorld()->SpawnActor<AWeaponShell>(AWeaponShell::StaticClass(), ShellLocation, ShellRotation, SpawnParams);
    if (ShellActor)
    {
        FVector Direction = ShellRotation.RotateVector(FVector(-30.f, 0.f, 0.f));
        ShellActor->EjectShell(Direction);
        ShellActor->SetLifeSpan(3.f);
    }

    FHitResult HitResult;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);

    for (int i = 0; i < 10; ++i)
    {        
        //bullet niagara

        FVector Offset = FVector::ZeroVector;
        Offset.X = FMath::RandRange(-4000., 4000.);
        Offset.Y = FMath::RandRange(-4000., 4000.);
        Offset.Z = FMath::RandRange(-4000., 4000.);

        EndLocation += Offset;
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), BulletNiagara, MuzzleLocation, UKismetMathLibrary::MakeRotFromY(EndLocation));

        bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, MuzzleLocation, EndLocation, ECC_Visibility, Params);
        if (bHit)
        {
            if (HitResult.GetActor() && !HitResult.GetActor()->ActorHasTag("Player"))
            {
                if (HitResult.GetActor()->ActorHasTag("Enemy"))
                {
                    //blood effect only enemy
                    UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), BloodNiagara, HitResult.Location, UKismetMathLibrary::MakeRotFromX(HitResult.ImpactNormal), FVector(0.5f, 0.5f, 0.5f));
                    UGameplayStatics::ApplyDamage(HitResult.GetActor(), Damage, nullptr, this, UDamageType::StaticClass());
                }
                else
                {
                    UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), LandNiagara, HitResult.Location, UKismetMathLibrary::MakeRotFromX(HitResult.ImpactNormal), FVector(0.2f, 0.2f, 0.2f));
                    UGameplayStatics::SpawnDecalAtLocation(GetWorld(), BulletDecal, FVector(5.f, 5.f, 5.f),
                        HitResult.Location, UKismetMathLibrary::MakeRotFromX(HitResult.ImpactNormal), 4.f)->SetFadeScreenSize(0.f);
                }
            }


        }
        /*FColor DrawColor = bHit ? FColor::Green : FColor::Red;
        DrawDebugLine(GetWorld(), MuzzleLocation, EndLocation, DrawColor, false, 1.0f, 0, 2.0f);*/
        //GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Blue, FString::Printf(TEXT("%f %f %f"), EndLocation.X, EndLocation.Y, EndLocation.Z));
    }
}

void AWeaponShotgun::Reload(int32 Amount)
{
    CurrentAmmo += Amount;

    if (ReloadSound.Num() > 0)
    {
        int RandIdx = FMath::RandRange(0, ReloadSound.Num() - 1);
        for (int i = 0; i < 4; ++i)
            UGameplayStatics::PlaySoundAtLocation(GetWorld(), ReloadSound[RandIdx], GetActorLocation());
    }
}
