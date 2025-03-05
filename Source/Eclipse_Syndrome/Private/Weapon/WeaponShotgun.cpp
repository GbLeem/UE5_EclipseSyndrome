#include "Weapon/WeaponShotgun.h"

#include "Weapon/WeaponShell.h"

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
}

void AWeaponShotgun::Fire()
{
    if (CurrentAmmo <= 0)
    {
        return;
    }

    CurrentAmmo--;

    FVector MuzzleLocation = GunMesh->GetSocketLocation(TEXT("MuzzleSocket"));
    FRotator MuzzleRotation = GunMesh->GetSocketRotation(TEXT("MuzzleSocket"));

    UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), MuzzleNiagara, MuzzleLocation, GetActorRotation());
    //UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), BulletNiagara, MuzzleLocation, MuzzleRotation);

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
        FVector Offset = FVector::ZeroVector;
        Offset.X = FMath::RandRange(0., 10000.);
        Offset.Y = FMath::RandRange(0., 10000.);
        EndLocation += Offset;

        bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, MuzzleLocation, EndLocation, ECC_Visibility, Params);
        if (bHit)
        {
            if (HitResult.GetActor() && !HitResult.GetActor()->ActorHasTag("Player"))
            {
                if (HitResult.GetActor()->ActorHasTag("Enemy"))
                {
                    GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("blood")));
                }
                UGameplayStatics::ApplyDamage(HitResult.GetActor(), Damage, nullptr, this, UDamageType::StaticClass());

                //blood effect -> only enemy
                UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), BloodNiagara, HitResult.Location, UKismetMathLibrary::MakeRotFromX(HitResult.ImpactNormal), FVector(0.5f, 0.5f, 0.5f));
            }

            UGameplayStatics::SpawnDecalAtLocation(GetWorld(), BulletDecal, FVector(5.f, 5.f, 5.f),
                HitResult.Location, UKismetMathLibrary::MakeRotFromX(HitResult.ImpactNormal), 4.f)->SetFadeScreenSize(0.f);

        }
        FColor DrawColor = bHit ? FColor::Green : FColor::Red;
        DrawDebugLine(GetWorld(), MuzzleLocation, EndLocation, DrawColor, false, 1.0f, 0, 2.0f);
        GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Blue, FString::Printf(TEXT("%f %f %f"), EndLocation.X, EndLocation.Y, EndLocation.Z));
    }
}
