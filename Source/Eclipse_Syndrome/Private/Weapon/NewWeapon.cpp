// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/NewWeapon.h"
#include "Components/SceneCaptureComponent2D.h"

ANewWeapon::ANewWeapon()
{
	PrimaryActorTick.bCanEverTick = true;
	static ConstructorHelpers::FObjectFinder<UStaticMesh>StaticMeshAsset(TEXT("/Game/HJ/Assets/Combined_DDA78210.Combined_DDA78210"));
	if (StaticMeshAsset.Succeeded())
	{
		GunMesh->SetStaticMesh(StaticMeshAsset.Object);
	}

	static ConstructorHelpers::FObjectFinder<UStaticMesh>LensAsset(TEXT("/Game/HJ/Material/sniper/Sniper_Lens.Sniper_Lens"));
	if (LensAsset.Succeeded())
	{
		Lens->SetStaticMesh(LensAsset.Object);
		Scope->SetStaticMesh(LensAsset.Object);
	}

	static ConstructorHelpers::FObjectFinder<UMaterial>LensMat(TEXT("/Game/HJ/Material/sniper/M_Sniper.M_Sniper"));
	if (LensMat.Succeeded())
	{
		Lens->SetMaterial(0,LensMat.Object);
	}
	static ConstructorHelpers::FObjectFinder<UMaterial>CrossMat(TEXT("/Game/HJ/Material/sniper/M_Crosshair.M_Crosshair"));
	if (CrossMat.Succeeded())
	{
		Scope->SetMaterial(0, CrossMat.Object);
	}

	Lens->SetRelativeLocation(FVector(0.f, -3.6f, 9.8f));
	Lens->SetRelativeRotation(FRotator(-90.f, 0.f, 0.f));
	Scope->SetRelativeLocation(FVector(0.f, -4.f, 9.8f));
	Scope->SetRelativeRotation(FRotator(-90.f, 0.f, 0.f));

	CaptureComp = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("capture"));
	CaptureComp->SetupAttachment(GunMesh);
}


// Called when the game starts or when spawned
void ANewWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ANewWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

