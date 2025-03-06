#include "Weapon/Weapon.h"

#include "Character/PlayerCamera.h"
#include "Character/PlayerCharacter.h"
#include "Character/PlayerCharacterController.h"
#include "Weapon/WeaponShell.h"

#include "Blueprint/UserWidget.h"
#include "Components/DecalComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/Image.h"
#include "DrawDebugHelpers.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Particles/ParticleSystem.h"

#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

AWeapon::AWeapon()
    :WeaponNumber(0)
    ,CurrentAmmo(0)
{
    PrimaryActorTick.bCanEverTick = true;

	GunMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GunMesh"));
	RootComponent = GunMesh;

    CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Collision"));
    CollisionComp->SetupAttachment(RootComponent);
    CollisionComp->SetCollisionProfileName(TEXT("OverlapAllDynamics"));
    CollisionComp->SetSphereRadius(200.f);   

    CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnItemOverlap);
    CollisionComp->OnComponentEndOverlap.AddDynamic(this, &AWeapon::OnItemEndOverlap);
    
    ItemHoverUI = CreateDefaultSubobject<UWidgetComponent>(TEXT("Hover UI"));
    ItemHoverUI->SetupAttachment(GunMesh);
    ItemHoverUI->SetWidgetSpace(EWidgetSpace::Screen);
    ItemHoverUI->SetVisibility(false);
   
    WeaponSpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
    WeaponSpringArmComp->SetupAttachment(RootComponent);
    WeaponSpringArmComp->TargetArmLength = 0.f;
    WeaponSpringArmComp->SetRelativeLocation(FVector(0.f, 0.f, 0.f));
    WeaponSpringArmComp->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));
    WeaponSpringArmComp->bUsePawnControlRotation = true;

    WeaponCameraComp = CreateDefaultSubobject<UChildActorComponent>(TEXT("Weapon Camera"));
    WeaponCameraComp->SetupAttachment(WeaponSpringArmComp);
    WeaponCameraComp->SetChildActorClass(APlayerCamera::StaticClass());
    WeaponCameraComp->SetRelativeLocation(FVector(0.f, 0.f, 0.f));
    WeaponCameraComp->SetRelativeRotation(FRotator(0.f, 0.f, 0.f));

    static ConstructorHelpers::FClassFinder<UUserWidget>ItemUIClass(TEXT("/Game/HJ/UI/WBP_Item.WBP_Item_C"));
    if (ItemUIClass.Succeeded())
    {
        ItemHoverUI->SetWidgetClass(ItemUIClass.Class);
    }    
    
    static ConstructorHelpers::FObjectFinder<UMaterialInterface>DecalAsset(TEXT("/Game/HJ/Material/M_Bullet.M_Bullet"));
    if (DecalAsset.Succeeded())
    {
        BulletDecal = DecalAsset.Object;
    }

    //Niagara Assets
    static ConstructorHelpers::FObjectFinder<UNiagaraSystem>BloodAsset(TEXT("/Game/SH/NS_Splash.NS_Splash")); 
    BloodNiagara = BloodAsset.Object;

    static ConstructorHelpers::FObjectFinder<UNiagaraSystem>MuzzleAsset(TEXT("/Game/SH/NS_MuzzleFlash.NS_MuzzleFlash"));
    MuzzleNiagara = MuzzleAsset.Object;

    static ConstructorHelpers::FObjectFinder<UNiagaraSystem>BulletAsset(TEXT("/Game/HJ/Material/NS_HJBullet2.NS_HJBullet2"));
    BulletNiagara = BulletAsset.Object;

    static ConstructorHelpers::FObjectFinder<UNiagaraSystem> LandNiagaraAsset(TEXT("/Game/HJ/Assets/NS_Steam1.NS_Steam1"));
    LandNiagara = LandNiagaraAsset.Object;

    //sound asset
    static ConstructorHelpers::FObjectFinder<USoundBase>NoAmmoAsset(TEXT("/Game/HJ/Assets/Sound/Smith_Wesson_Handling_1.Smith_Wesson_Handling_1"));
    if (NoAmmoAsset.Succeeded())
    {
        NoAmmoSound = NoAmmoAsset.Object;
    }
   

    Tags.Add("Weapon");
}

void AWeapon::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bIsPeeking)
        ShowUI();
    else
        StopUI();

    bIsPeeking = false;
}

void AWeapon::BeginPlay()
{
    Super::BeginPlay();
}

void AWeapon::Fire()
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

    UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), MuzzleNiagara, MuzzleLocation, GetActorRotation());
    //UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), BulletNiagara, MuzzleLocation, MuzzleRotation);

    FVector FireDirection = CalculateDestination()- MuzzleLocation;
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

    FColor DrawColor = bHit ? FColor::Green : FColor::Red;
    DrawDebugLine(GetWorld(), MuzzleLocation, EndLocation, DrawColor, false, 1.0f, 0, 2.0f);
}

// Reload
void AWeapon::Reload(int32 Amount)
{	
    CurrentAmmo += Amount;

    if (ReloadSound.Num() > 0)
    {
        int RandIdx = FMath::RandRange(0, ReloadSound.Num() - 1);
        UGameplayStatics::PlaySoundAtLocation(GetWorld(), ReloadSound[RandIdx], GetActorLocation());
    }
}

FVector AWeapon::CalculateDestination()
{
    APlayerCharacterController* PlayerCharacterController = Cast<APlayerCharacterController>(UGameplayStatics::GetPlayerController(this, 0));
    if (PlayerCharacterController)
    {                    
        int32 ScreenWidth;
        int32 ScreenHeight;

        PlayerCharacterController->GetViewportSize(ScreenWidth, ScreenHeight);
        const FVector2D ScreenCenter(ScreenWidth * 0.5f, ScreenHeight * 0.5f);

        FVector WorldLocation;
        FVector WorldDirection;
        if (PlayerCharacterController->DeprojectScreenPositionToWorld(ScreenCenter.X, ScreenCenter.Y, WorldLocation, WorldDirection))
        {
            FVector TraceStart = WorldLocation + WorldDirection;
            FVector TraceEnd = TraceStart + WorldDirection * FireRange;
                    
            FHitResult HitResult;
            FCollisionQueryParams TraceParams;
            TraceParams.AddIgnoredActor(this);

            GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, TraceParams);

            return TraceEnd;
        }                    
    }
    return FVector::ZeroVector;
}

void AWeapon::ShowUI()
{
    ItemHoverUI->SetVisibility(true);
}

void AWeapon::StopUI()
{
    ItemHoverUI->SetVisibility(false);
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