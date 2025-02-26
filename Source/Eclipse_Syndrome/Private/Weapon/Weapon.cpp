#include "Weapon/Weapon.h"

#include "Character/PlayerCharacter.h"
#include "Character/PlayerCharacterController.h"

#include "Blueprint/UserWidget.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/Image.h"
#include "DrawDebugHelpers.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"


AWeapon::AWeapon()
    :WeaponNumber(0)
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

    static ConstructorHelpers::FClassFinder<UUserWidget>ItemUIClass(TEXT("/Game/HJ/UI/WBP_Item.WBP_Item_C"));
    if (ItemUIClass.Succeeded())
    {
        ItemHoverUI->SetWidgetClass(ItemUIClass.Class);
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

//[TODO] effect, sound, damage system
void AWeapon::Fire()
{
    if (CurrentAmmo <= 0)
    {
        GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Black, FString::Printf(TEXT("no current ammo")));
        return;
    }

    FVector MuzzleLocation = GunMesh->GetSocketLocation(TEXT("MuzzleSocket"));
    FVector FireDirection = CalculateDestination() - MuzzleLocation;

    FVector EndLocation = MuzzleLocation + FireDirection * FireRange;  // 사거리 변수 사용

    // 트레이스
    FHitResult HitResult;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);  // 총기 자신은 무시

    bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, MuzzleLocation, EndLocation, ECC_Visibility, Params);
    FColor DrawColor = bHit ? FColor::Green : FColor::Red;
    if (bHit)
    {
        if (HitResult.GetActor())
        {
            float FinalDamage = Damage; // Damage

            if (HitResult.BoneName == "head")  // Headshot
            {
                FinalDamage *= 2.0f; // Damage *2
                //GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Yellow, TEXT("Headshot!")); // 추후 삭제
            }
            else // Bodyshot
            {
                //GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("Bodyshot!")); // 추후 삭제
            }   

            GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, HitResult.GetActor()->GetName());
            APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetWorld()->GetFirstLocalPlayerFromController());
            if (PlayerCharacter)
            {
               UGameplayStatics::ApplyDamage(HitResult.GetActor(), Damage, nullptr, PlayerCharacter, UDamageType::StaticClass());
            }
            /*if (GetWorld()->GetFirstPlayerController())
            {
                APlayerCharacterController* PlayerCharacterController = Cast<APlayerCharacterController>(GetWorld()->GetFirstPlayerController());
                if (PlayerCharacterController)
                {
                }
            }*/
            //UGameplayStatics::ApplyPointDamage(HitResult.GetActor(), Damage, HitResult.ImpactNormal, HitResult, nullptr, this, nullptr);
            
        }
    }
    DrawDebugLine(GetWorld(), MuzzleLocation, EndLocation, DrawColor, false, 1.0f, 0, 2.0f); //트레이스 빨간선 
    
    // 탄약 감소
    CurrentAmmo--;
}

// Reload
void AWeapon::Reload(int32 Amount)
{	
    //GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("reloading!!")));
    CurrentAmmo += Amount;
}


//TEST  Crosshair 이거는 잘모르겠네요 삭제하셔도 상관없는 코드 헤드샷 판정??
bool AWeapon::GetAimHitResult(FHitResult& OutHitResult)
{
    APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
    if (!PlayerController) return false;

    FVector CameraLocation;
    FRotator CameraRotation;
    PlayerController->GetPlayerViewPoint(CameraLocation, CameraRotation);

    FVector TraceStart = CameraLocation;
    FVector TraceEnd = TraceStart + (CameraRotation.Vector() * FireRange);

    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(this); // 총기 자신은 무시

    return GetWorld()->LineTraceSingleByChannel(OutHitResult, TraceStart, TraceEnd, ECC_Visibility, QueryParams);
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
            //DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Blue, false, 2.f, 0, 2.f);

            return TraceEnd;
        }                    
    }
    return FVector::ZeroVector;
}

void AWeapon::ShowUI()
{
    ItemHoverUI->SetVisibility(true);
    //CanPickUp = true;
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

