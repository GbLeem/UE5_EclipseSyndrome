#include "Weapon/Weapon.h"

#include "Character/PlayerCharacter.h"

#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"

AWeapon::AWeapon()
{
    PrimaryActorTick.bCanEverTick = false;

	GunMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GunMesh"));
	RootComponent = GunMesh;

    //Test Muzzlepoint
    Muzzlepoint = CreateDefaultSubobject<USceneComponent>(TEXT("Muzzlepoint"));
    Muzzlepoint->SetupAttachment(GunMesh);

    CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Collision"));
    CollisionComp->SetupAttachment(RootComponent);
    CollisionComp->SetCollisionProfileName(TEXT("OverlapAllDynamics"));
    CollisionComp->SetSphereRadius(150.f);
    
    static ConstructorHelpers::FObjectFinder<UStaticMesh>StaticMeshAsset(TEXT("/Game/HJ/Assets/QuantumCharacter/Mesh/Rifle/SM_Rifle_Olive.SM_Rifle_Olive"));
    if (StaticMeshAsset.Succeeded())
    {
        GunMesh->SetStaticMesh(StaticMeshAsset.Object);
    }
	// 기본 값 설정
	FireRate = 0.2f;
	MaxAmmo = 30;
	CurrentAmmo = MaxAmmo;
	FireRange = 5000.f;
	Damage = 10.f;

    CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnItemOverlap);
    CollisionComp->OnComponentEndOverlap.AddDynamic(this, &AWeapon::OnItemEndOverlap);
    
    Tags.Add("Weapon");
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

    // 총구 위치 가져오기 (GunMegh -> Muzzlepoint)
    FVector MuzzleLocation = Muzzlepoint->GetComponentLocation();
    FRotator MuzzleRotation = Muzzlepoint->GetComponentRotation();

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

void AWeapon::DestroyItem()
{
    Destroy();
}

void AWeapon::OnItemOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor && OtherActor->ActorHasTag("Player"))
    {
        APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(OtherActor);

        if (PlayerCharacter)
        {
            PlayerCharacter->StartPeek();
        }
    }
}

void AWeapon::OnItemEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (OtherActor && OtherActor->ActorHasTag("Player"))
    {
        APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(OtherActor);

        if (PlayerCharacter)
        {
            PlayerCharacter->StopPeek();
        }
    }
}

