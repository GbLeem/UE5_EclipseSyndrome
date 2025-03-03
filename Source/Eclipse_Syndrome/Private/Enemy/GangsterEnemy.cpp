// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/GangsterEnemy.h"
#include "Enemy/GangsterAIController.h"
#include "Character/PlayerCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "System/DefaultGameState.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

AGangsterEnemy::AGangsterEnemy()
{
	Name = "GangsterEnemy";
	MaxHealth = 100.0f;
	Health = MaxHealth;
	Damage = 20.0f;
	AttackRange = 700.0f;
	AttackReadyRange = 450.0f;
	ShootRange = 5000.0f;
	AimSpeed = 200.0f;

	// AI
	AIControllerClass = AGangsterAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	// Components
	EnemyMesh = GetMesh();
	GunMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunMesh"));
}

void AGangsterEnemy::ChangeSpeedAim()
{
	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
	if (MovementComponent)
	{
		MovementComponent->MaxWalkSpeed = AimSpeed;
	}
}

void AGangsterEnemy::BeginPlay()
{
	Super::BeginPlay();
	if (GunMesh)
	{
		GunMesh->AttachToComponent(EnemyMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("WeaponSocket"));
	}
}

void AGangsterEnemy::Attack(AActor* TargetActor)
{
	FVector MuzzleLocation = GunMesh->GetSocketLocation(TEXT("MuzzleSocket"));
	FVector TargetLocation = CalculateDestination();

	if (TargetLocation.IsZero()) // 장애물에 가려져 있으면 발사하지 않음
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Yellow, TEXT("Target is behind cover!"));
		return;
	}

	FVector FireDirection = (TargetLocation - MuzzleLocation).GetSafeNormal();

	// Bullet Spread
	float SpreadAngle = FMath::DegreesToRadians(10.0f);
	FireDirection = FMath::VRandCone(FireDirection, SpreadAngle);

	FVector EndLocation = MuzzleLocation + FireDirection * ShootRange;

	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, MuzzleLocation, EndLocation, ECC_Visibility, Params);
	FColor DrawColor = bHit ? FColor::Blue : FColor::Purple;

	if (bHit && HitResult.GetActor())
	{
		UGameplayStatics::ApplyDamage(HitResult.GetActor(), Damage, nullptr, this, UDamageType::StaticClass());
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, HitResult.GetActor()->GetName());
	}

	DrawDebugLine(GetWorld(), MuzzleLocation, EndLocation, DrawColor, false, 1.0f, 0, 2.0f); // 피격 여부에 따라 색상 변경
}


float AGangsterEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

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

	DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 2.0f, 0, 2.0f);

	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this); // Ignore SelfEnemy

	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params);
	if (bHit && HitResult.GetActor() == Target)
	{
		return HitResult.ImpactPoint;  // 조준 성공 시 목표 지점 반환
	}

	return FVector::ZeroVector; // 장애물에 막혀 있으면 사격 취소
}


