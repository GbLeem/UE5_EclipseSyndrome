#include "Weapon/NewWeapon.h"
#include "Components/SceneCaptureComponent2D.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetRenderingLibrary.h"

ANewWeapon::ANewWeapon()
{
	FireRate = 10.f;
	FireRange = 8000.f;
	Damage = 100.f;
	MaxAmmo = 15;
	bAutoFire = false;
	bIsPeeking = false;
	WeaponNumber = 5;

	MinPitchRecoil = 3.5f;
	MaxPitchRecoil = 5.5f;
	MinYawRecoil = 2.2f;
	MaxYawRecoil = 4.2f;

	Lens = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Lens mesh"));
	Lens->SetupAttachment(RootComponent);
	Scope = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Scope mesh"));
	Scope->SetupAttachment(RootComponent);

	static ConstructorHelpers::FObjectFinder<UStaticMesh>StaticMeshAsset(TEXT("/Game/HJ/Assets/Combined_DDA78210.Combined_DDA78210"));
	if (StaticMeshAsset.Succeeded())
	{
		GunMesh->SetStaticMesh(StaticMeshAsset.Object);
	}

	static ConstructorHelpers::FObjectFinder<UStaticMesh>LensAsset(TEXT("/Game/HJ/Material/sniper/Sniper_Lens.Sniper_Lens"));
	if (LensAsset.Succeeded())
	{
		Lens->SetStaticMesh(LensAsset.Object);
	}
	static ConstructorHelpers::FObjectFinder<UStaticMesh>ScopeAsset(TEXT("/Game/HJ/Material/sniper/Sniper_Lens.Sniper_Lens"));
	if (ScopeAsset.Succeeded())
	{
		Scope->SetStaticMesh(ScopeAsset.Object);
	}
	static ConstructorHelpers::FObjectFinder<UMaterial>LensMat(TEXT("/Game/HJ/Material/sniper/M_Sniper.M_Sniper"));
	if (LensMat.Succeeded())
	{
		Lens->SetMaterial(0, LensMat.Object);
	}
	static ConstructorHelpers::FObjectFinder<UMaterial>CrossMat(TEXT("/Game/HJ/Material/sniper/M_Crosshair.M_Crosshair"));
	if (CrossMat.Succeeded())
	{
		Scope->SetMaterial(0, CrossMat.Object);
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

	Lens->SetRelativeLocation(FVector(0.f, -3.6f, 9.8f));
	Lens->SetRelativeRotation(FRotator(0.f, -180.f, 90.f));
	Scope->SetRelativeLocation(FVector(0.f, -4.f, 9.8f));
	Scope->SetRelativeRotation(FRotator(0.f, -180.f, 90.f));
	
	CaptureComp = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("capture"));
	CaptureComp->SetupAttachment(GunMesh);
	CaptureComp->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));
	CaptureComp->SetupAttachment(GunMesh, FName(TEXT("Scope")));

	CaptureComp->FOVAngle = 5.f;
	WeaponSpringArmComp->SocketOffset = FVector(-14.f, 0.f, 10.f);
}

void ANewWeapon::BeginPlay()
{
	Super::BeginPlay();

	//scene capture
	CaptureComp->TextureTarget = UKismetRenderingLibrary::CreateRenderTarget2D(GetWorld(), 1000.f, 1000.f);
	Lens->CreateDynamicMaterialInstance(0, Lens->GetMaterial(0))->SetTextureParameterValue(FName(TEXT("Target")), CaptureComp->TextureTarget);
	Lens->SetMaterial(0, Lens->CreateDynamicMaterialInstance(0, Lens->GetMaterial(0)));
}


