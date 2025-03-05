// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/GangsterEnemy.h"
#include "Enemy/GangsterAIController.h"
#include "Character/PlayerCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "System/DefaultGameState.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "NiagaraComponent.h"
#include "Weapon/DefaultBullet.h"

AGangsterEnemy::AGangsterEnemy()
{
	Name = "GangsterEnemy";
	MaxHealth = 100.0f;
	Health = MaxHealth;
	Damage = 20.0f;
	AttackRange = 5000.0f;
	ChasingRange = 1500.0f;
	AdvancingRange = 1000.0f;
	ShootingRange = 450.0f;
	AimSpeed = 200.0f;
	ChaseSeed = 600.0f;
	PatrolSpeed = 150.0f;

	// AI
	AIControllerClass = AGangsterAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	// Components
	EnemyMesh = GetMesh();
	GunMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunMesh"));
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("Callable Area"));
	SphereComp->SetupAttachment(RootComponent);
	MuzzleFlashComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Muzzle Splash Niagara"));
	MuzzleFlashComp->SetupAttachment(GunMesh, "MuzzleSocket");
}

void AGangsterEnemy::ChangeSpeedAim()
{
	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
	if (MovementComponent)
	{
		MovementComponent->MaxWalkSpeed = AimSpeed;
	}
}

void AGangsterEnemy::CallNearbyGangster()
{
	if (!SphereComp) return;

	TArray<AActor*> OverlappingActors;

	SphereComp->GetOverlappingActors(OverlappingActors, AGangsterEnemy::StaticClass());
	
	ADefaultGameState* DefaultGameState = Cast<ADefaultGameState>(GetWorld()->GetGameState());
	
	for (AActor* Enemy : OverlappingActors)
	{
		if (AGangsterEnemy* Gangster = Cast<AGangsterEnemy>(Enemy))
		{
			if (AGangsterAIController* GangsterAIController = Cast<AGangsterAIController>(Gangster->GetController()))
			{
				GangsterAIController->GetBlackboardComponent()->SetValueAsBool(TEXT("PlayerDetected"), true);
				if (DefaultGameState)
				{
					APlayerCharacter* Player = DefaultGameState->GetPlayerCharacter();
					GangsterAIController->GetBlackboardComponent()->SetValueAsObject(TEXT("TargetActor"), Player);
					GangsterAIController->GetBlackboardComponent()->SetValueAsObject(TEXT("PatrolPath"), nullptr);
				}
			}
		}
	}
}

void AGangsterEnemy::BeginPlay()
{
	Super::BeginPlay();
	if (GunMesh)
	{
		GunMesh->AttachToComponent(EnemyMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("WeaponSocket"));
	}
	if (MuzzleFlashComp)
	{
		MuzzleFlashComp->Deactivate();
	}
	GetCharacterMovement()->bUseRVOAvoidance = true;
	GetCharacterMovement()->AvoidanceConsiderationRadius = 300.0f;

}

void AGangsterEnemy::Attack(AActor* TargetActor)
{
	if (!TargetActor) return;

	// Rotate towards Player
	if (AGangsterAIController* GangsterAIController = Cast<AGangsterAIController>(this->GetController()))
	{
		EPathFollowingRequestResult::Type MoveResult = GangsterAIController->MoveToActor(TargetActor, 5.0f, true, true, false, nullptr, true);
	}

	// Splash Niagara Effect
	MuzzleFlashComp->Activate(false);
	FVector MuzzleLocation = GunMesh->GetSocketLocation(TEXT("MuzzleSocket"));
	FVector TargetLocation = CalculateDestination();

	// Bullet Niagara Effect
	FVector ShootDirection = TargetLocation - MuzzleLocation;
	FRotator ShootRotation = ShootDirection.Rotation();
	ADefaultBullet* Bullet = GetWorld()->SpawnActor<ADefaultBullet>(BulletClass, MuzzleLocation, ShootRotation);
	Bullet->FireInDirection(ShootDirection.GetSafeNormal());

	if (TargetLocation.IsZero()) // Does not fire if obscured by obstacles
	{
		TargetLocation = TargetActor->GetActorLocation() + FVector(0, 0, 50);
	}

	FVector FireDirection = (TargetLocation - MuzzleLocation).GetSafeNormal();
	if (FireDirection.IsNearlyZero())
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("Invalid Fire Direction!"));
		return;
	}
	// Bullet Spread
	float SpreadAngle = FMath::DegreesToRadians(10.0f);
	FireDirection = FMath::VRandCone(FireDirection, SpreadAngle);

	FVector EndLocation = MuzzleLocation + FireDirection * AttackRange;

	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(Bullet);

	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, MuzzleLocation, EndLocation, ECC_Visibility, Params);
	FColor DrawColor = bHit ? FColor::Blue : FColor::Purple;

	if (bHit && HitResult.GetActor())
	{
		UGameplayStatics::ApplyDamage(HitResult.GetActor(), Damage, nullptr, this, UDamageType::StaticClass());
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, HitResult.GetActor()->GetName());
	}

	//DrawDebugLine(GetWorld(), MuzzleLocation, EndLocation, DrawColor, false, 1.0f, 0, 2.0f); // 피격 여부에 따라 색상 변경
}


float AGangsterEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (!DamageCauser->ActorHasTag(FName("Enemy")))
	{
		// Get GangsterAIController
		AGangsterAIController* AIController = Cast<AGangsterAIController>(GetController());
		if (!AIController) return ActualDamage;

		// When attacked while Player Undetected
		if (AIController->GetBlackboardComponent()->GetValueAsObject(TEXT("TargetActor")) == nullptr)
		{
			ADefaultGameState* DefaultGameState = Cast<ADefaultGameState>(GetWorld()->GetGameState());
			if (DefaultGameState)
			{
				APlayerCharacter* Player = DefaultGameState->GetPlayerCharacter();
				if (Player)
				{
					AIController->GetBlackboardComponent()->SetValueAsBool(TEXT("PlayerDetected"), true);
					AIController->GetBlackboardComponent()->SetValueAsObject(TEXT("TargetActor"), Player);
					AIController->GetBlackboardComponent()->SetValueAsObject(TEXT("PatrolPath"), nullptr);
				}
			}
		}

		// Hit Animation
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			if (HitMontage)
			{
				AnimInstance->Montage_Play(HitMontage);
			}
		}
	}

	return ActualDamage;
}

FVector AGangsterEnemy::CalculateDestination()
{
	// Get GangsterAIController
	AGangsterAIController* AIController = Cast<AGangsterAIController>(GetController());
	if (!AIController) return FVector::ZeroVector;

	// Get Target from Blackboard TargetActor(=PlayerCharacter)
	APlayerCharacter* Target = Cast<APlayerCharacter>(AIController->GetBlackboardComponent()->GetValueAsObject(TEXT("TargetActor")));
	if (!Target) return FVector::ZeroVector;

	FVector Start = GunMesh->GetSocketLocation(TEXT("MuzzleSocket"));
	FVector End = Target->GetActorLocation() + FVector(0, 0, 50);

	//DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 2.0f, 0, 2.0f);

	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this); // Ignore SelfEnemy

	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params);
	if (bHit && HitResult.GetActor() == Target)
	{
		return HitResult.ImpactPoint;  // Returns to target point when aiming is successful
	}

	return Target->GetActorLocation() + FVector(0, 0, 50); // Cancel shooting if blocked by an obstacle
}


