#include "Weapon/DefaultBullet.h"
#include "NiagaraComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

ADefaultBullet::ADefaultBullet()
{
	PrimaryActorTick.bCanEverTick = false;

	BoxComp = CreateDefaultSubobject<UBoxComponent>("BoxComponent");
	SetRootComponent(BoxComp);
	BoxComp->SetBoxExtent(FVector(1.0f, 1.0f, 1.0f));
	BoxComp->SetSimulatePhysics(true);
	BoxComp->SetEnableGravity(false);
	BoxComp->SetCollisionProfileName(TEXT("BlockAll"));
	BoxComp->OnComponentHit.AddDynamic(this, &ADefaultBullet::OnHit);

	NiagaraComp = CreateDefaultSubobject<UNiagaraComponent>("NiagaraComponent");
	NiagaraComp->SetupAttachment(BoxComp);

	ProjectileMovementComp = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovementComponent");
	ProjectileMovementComp->InitialSpeed = 4000.0f;
	ProjectileMovementComp->MaxSpeed = 6000.0f;
	ProjectileMovementComp->bInitialVelocityInLocalSpace = true;
}

void ADefaultBullet::BeginPlay()
{
	Super::BeginPlay();
	
}

void ADefaultBullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADefaultBullet::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor && OtherActor != this)
	{
		NiagaraComp->DestroyComponent();
		Destroy();
	}
}

void ADefaultBullet::FireInDirection(const FVector& ShootDirection)
{
	if (ProjectileMovementComp)
	{
		ProjectileMovementComp->Velocity = ShootDirection * ProjectileMovementComp->InitialSpeed;
	}
}
