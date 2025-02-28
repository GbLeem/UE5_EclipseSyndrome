#include "Weapon/WeaponShell.h"

AWeaponShell::AWeaponShell()
{
	PrimaryActorTick.bCanEverTick = false;

	ShellMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	RootComponent = ShellMesh;

	ShellMesh->SetSimulatePhysics(true);
	ShellMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> ShellMeshAsset(TEXT("/Game/HJ/Assets/FPS_Weapon_Bundle/Weapons/Meshes/Ammunition/SM_Shell_545x39_Empty.SM_Shell_545x39_Empty"));
	if (ShellMeshAsset.Succeeded())
	{
		ShellMesh->SetStaticMesh(ShellMeshAsset.Object);
	}
	ShellMesh->SetWorldScale3D(FVector(2.f, 2.f, 2.f));
}

void AWeaponShell::EjectShell(FVector Direction)
{
	ShellMesh->AddImpulse(Direction * 3.f, NAME_None, true);
}