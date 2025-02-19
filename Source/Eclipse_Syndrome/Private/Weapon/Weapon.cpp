#include "Weapon/Weapon.h"
#include "Engine/Engine.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

AWeapon::AWeapon()
{
	GunMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GunMesh"));
	RootComponent = GunMesh;

	// 기본 값 설정
	FireRate = 0.2f;
	MaxAmmo = 30;
	CurrentAmmo = MaxAmmo;
	FireRange = 5000.f;
	Damage = 10.f;
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
}

void AWeapon::Fire()
{
    if (CurrentAmmo <= 0)
    {
        return;
    }

    // 총구 위치 가져오기
    FVector MuzzleLocation = GunMesh->GetComponentLocation();
    FRotator MuzzleRotation = GunMesh->GetComponentRotation();

    // 트레이스
    FHitResult HitResult;
    FVector EndLocation = MuzzleLocation + MuzzleRotation.Vector() * FireRange;  // 사거리 변수 사용

    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);  // 총기 자신은 무시

    if (GetWorld()->LineTraceSingleByChannel(HitResult, MuzzleLocation, EndLocation, ECC_Visibility, Params))
    {
        if (HitResult.GetActor())
        {
            UGameplayStatics::ApplyPointDamage(HitResult.GetActor(), Damage, HitResult.ImpactNormal, HitResult, nullptr, this, nullptr);
        }
    }

    // 탄약 감소
    CurrentAmmo--;
}

// 재장전 기능
void AWeapon::Reload()
{
    CurrentAmmo = MaxAmmo;
}

