#include "Drone/Drone.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "NiagaraComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/PlayerCharacterController.h"
#include "Components/SphereComponent.h"
#include "Enemy/EnemyBase.h"
#include "Item/ItemInterface.h"
#include "Kismet/GameplayStatics.h"
#include "System/DefaultGameState.h"

ADrone::ADrone()
	: MoveForce(300000.f)
	, AntiForce(980.f)
	, AirResistance(0.9f)
	, MaxTiltAngle(20.0f)
	, InterpSpeed(1.0f)
	, DetectionRange(5000.0f)
	, AttackRange(1500.0f)
	, FollowRange(500.0f)
	, AttackDamage(5.0f)
	, AttackCooldown(0.3f)
	, SpreadAngle(5.0f)
{
	PrimaryActorTick.bCanEverTick = true;
	ComponentInit();
}

void ADrone::BeginPlay()
{
	Super::BeginPlay();

	Cast<ADefaultGameState>(GetWorld()->GetGameState())->SetDrone(this);

	RightMuzzleFlashComp->Deactivate();
	LeftMuzzleFlashComp->Deactivate();
}

void ADrone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CapsuleComp)
	{
		AddAntiGravity();
		AddAirResistance();
	}

	TiltDrone(DeltaTime);
	MoveInput = FVector::ZeroVector;

	if (bIsGrabbing)
	{
		FVector HoldLocation = GetActorLocation() + -GetActorUpVector() * 30.f;
		PhysicsHandleComp->SetTargetLocationAndRotation(HoldLocation, GetActorRotation());
	}
}

void ADrone::ComponentInit()
{
	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	SetRootComponent(CapsuleComp);
	CapsuleComp->SetCollisionProfileName("Pawn");
	CapsuleComp->SetSimulatePhysics(true);

	CameraSceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("CameraScene"));
	CameraSceneComp->SetupAttachment(CapsuleComp);

	TiltSceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("TiltScene"));
	TiltSceneComp->SetupAttachment(CameraSceneComp);

	SkeletalMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMeshComp->SetupAttachment(TiltSceneComp);
	
	PhysicsHandleComp = CreateDefaultSubobject<UPhysicsHandleComponent>(TEXT("PhysicsHandle"));

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComp->SetupAttachment(CameraSceneComp);
	SpringArmComp->TargetArmLength = 120;
	SpringArmComp->bUsePawnControlRotation = false;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);

	DetectionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("DetectionSphere"));
	DetectionSphere->SetupAttachment(CapsuleComp);
	DetectionSphere->InitSphereRadius(DetectionRange);

	RightMuzzleFlashComp = CreateDefaultSubobject<UNiagaraComponent>("RightMuzzleFlash");
	RightMuzzleFlashComp->SetupAttachment(SkeletalMeshComp, "barrel-gun_R_1__end");

	LeftMuzzleFlashComp = CreateDefaultSubobject<UNiagaraComponent>("LeftMuzzleFlash");
	LeftMuzzleFlashComp->SetupAttachment(SkeletalMeshComp, "barrel-gun_L_1__end");

	BindingFunction();
}

void ADrone::BindingFunction()
{
	DetectionSphere->OnComponentBeginOverlap.AddDynamic(this, &ADrone::OnDetectionOverlapBegin);
	DetectionSphere->OnComponentEndOverlap.AddDynamic(this, &ADrone::OnDetectionOverlapEnd);
}

void ADrone::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (const TObjectPtr<UEnhancedInputComponent> EnhancedInput
		= Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (const TObjectPtr<APlayerCharacterController> PlayerController
			= Cast<APlayerCharacterController>(GetController()))
		{
			if (PlayerController->DroneMoveAction)
			{
				EnhancedInput->BindAction(PlayerController->DroneMoveAction
					, ETriggerEvent::Triggered
					,this
					, &ADrone::Move);
			}

			if (PlayerController->DroneLookAction)
			{
				EnhancedInput->BindAction(PlayerController->DroneLookAction
					, ETriggerEvent::Triggered
					,this
					, &ADrone::Look);
			}
			
			if (PlayerController->DronePossessAction)
			{
				EnhancedInput->BindAction(PlayerController->DronePossessAction
					, ETriggerEvent::Started
					,this
					, &ADrone::PossessToCharacter);
			}

			if (PlayerController->DronePossessAction)
			{
				EnhancedInput->BindAction(PlayerController->DroneGrabAction
					, ETriggerEvent::Started
					,this
					, &ADrone::Grab);
			}
		}
	}
}

void ADrone::Move(const FInputActionValue& Value)
{
	if (!Controller) return;

	MoveInput = Value.Get<FVector>();

	FVector MoveToForce = FVector::ZeroVector;
	FVector MoveToDirection = FVector::ZeroVector;

	if (!FMath::IsNearlyZero(MoveInput.X))
	{
		MoveToDirection += CameraSceneComp->GetForwardVector() * MoveInput.X;
	}
	if (!FMath::IsNearlyZero(MoveInput.Y))
	{
		MoveToDirection += CameraSceneComp->GetRightVector() * MoveInput.Y;
	}
	if (!FMath::IsNearlyZero(MoveInput.Z))
	{
		MoveToDirection += CameraSceneComp->GetUpVector() * MoveInput.Z;
	}

	MoveToDirection = MoveToDirection.GetSafeNormal();
	MoveToForce = MoveToDirection * MoveForce;
	CapsuleComp->AddForce(MoveToForce);
}

void ADrone::Look(const FInputActionValue& Value)
{
	const FVector2d LookInput = Value.Get<FVector2d>();

	const FRotator NewYawRotation(0.0f, LookInput.X, 0.0f);
	CameraSceneComp->AddRelativeRotation(NewYawRotation);

	constexpr float PitchLimit = 80.0f;
	const FRotator CurrentRotation = SpringArmComp->GetRelativeRotation();
	const float NewPitch = FMath::Clamp(CurrentRotation.Pitch + LookInput.Y, -PitchLimit, PitchLimit);
    
	SpringArmComp->SetRelativeRotation(FRotator(NewPitch, 0.0f, 0.0f));
}

void ADrone::PossessToCharacter(const FInputActionValue& Value)
{
	if (!Value.Get<bool>())
	{
		Cast<APlayerCharacterController>(GetController())->SetPlayerPawn(this);
		Cast<APlayerCharacterController>(GetController())->ChangeMappingContext(0);
		Cast<APlayerCharacterController>(GetController())->ChangePossess(Cast<ADefaultGameState>(GetWorld()->GetGameState())->GetPlayerCharacter());
	}
}

void ADrone::Grab(const FInputActionValue& Value)
{
	static bool ToggleGrabbing = false;
	if (!Value.Get<bool>())
	{
		if (!ToggleGrabbing)
		{
			FVector Start = GetActorLocation();
			// GrabLength, GrabDirection
			FVector End = Start + 300.0f * -GetActorUpVector();

			FHitResult HitResult;
			FCollisionQueryParams Params;
			Params.AddIgnoredActor(this);

			DrawDebugLine(GetWorld(), Start, End, FColor::Red, false);
			if (GetWorld()->SweepSingleByChannel(HitResult,Start,End,FQuat::Identity,ECC_Visibility,FCollisionShape::MakeSphere(50.f),Params))
			{
				// Grabbable
				if (HitResult.GetActor()->ActorHasTag("Grabbable"))
				{
					Cast<UPrimitiveComponent>(HitResult.GetActor()->GetRootComponent())->SetSimulatePhysics(true);
					PhysicsHandleComp->GrabComponentAtLocationWithRotation(Cast<UPrimitiveComponent>(HitResult.GetActor()->GetRootComponent()), NAME_None, HitResult.ImpactPoint, GetActorRotation());
					bIsGrabbing = true;
					ToggleGrabbing = true;
				}
			}
		}
		else
		{
			DetachGrabActor(true);
			ToggleGrabbing = false;
		}
	}
}

void ADrone::DetachGrabActor(bool OnPhysics)
{
	PhysicsHandleComp->GetGrabbedComponent()->SetSimulatePhysics(OnPhysics);
	PhysicsHandleComp->ReleaseComponent();
	bIsGrabbing = false;
}

void ADrone::TiltDrone(float DeltaTime)
{
	float TargetRoll = MoveInput.Y * MaxTiltAngle;
	float TargetPitch = -MoveInput.X * MaxTiltAngle;
	
	if (FMath::IsNearlyZero(MoveInput.Size()))
	{
		TargetRoll = 0.0f;
		TargetPitch = 0.0f;
	}
	
	const FRotator CurrentRotation = TiltSceneComp->GetRelativeRotation();
	
	InterpSpeed = 1.0f;
	const float NewRoll = FMath::FInterpTo(CurrentRotation.Roll, TargetRoll, DeltaTime, InterpSpeed);
	const float NewPitch = FMath::FInterpTo(CurrentRotation.Pitch, TargetPitch, DeltaTime, InterpSpeed);

	TiltSceneComp->SetRelativeRotation(FRotator(NewPitch, CurrentRotation.Yaw, NewRoll));
}

void ADrone::AddAntiGravity() const
{
	const FVector GravityCompensation = FVector(0.0f, 0.0f, CapsuleComp->GetMass() * AntiForce);
	CapsuleComp->AddForce(GravityCompensation, NAME_None, false);
}

void ADrone::AddAirResistance() const
{
	const FVector CurrentVelocity = CapsuleComp->GetPhysicsLinearVelocity();
	const FVector DampingForce = -CurrentVelocity * CapsuleComp->GetMass() * AirResistance;
	CapsuleComp->AddForce(DampingForce, NAME_None, false);
}

void ADrone::SetEnhancedInput()
{
	InputComponent->ClearActionBindings();
	SetupPlayerInputComponent(InputComponent);
}

void ADrone::Attack(AEnemyBase* Target)
{
	AttackSingleArm(Target, TEXT("barrel-gun_R_1__end"));
	RightMuzzleFlashComp->Activate(false);
	AttackSingleArm(Target, TEXT("barrel-gun_L_1__end"));
	LeftMuzzleFlashComp->Activate(false);
	
}

void ADrone::AttackSingleArm(AEnemyBase* Target, const FName BoneName)
{
	FVector MuzzleLocation = SkeletalMeshComp->GetBoneLocation(BoneName);

	FVector BulletDirection = GetBulletDirection();
	FVector EndLocation = MuzzleLocation + (BulletDirection * AttackRange);
	
	FHitResult HitResult;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		MuzzleLocation,
		EndLocation,
		ECC_Visibility,
		QueryParams
	);

	if (bHit && HitResult.GetActor() == Target)
	{
		UGameplayStatics::ApplyDamage(Target, AttackDamage, nullptr, this, UDamageType::StaticClass());
	}
	DrawDebugLine(GetWorld(), MuzzleLocation, EndLocation, bHit ? FColor::Red : FColor::Blue, false, 1.0f, 0, 2.0f);
}

FVector ADrone::GetBulletDirection()
{
	FVector Forward = CameraSceneComp->GetForwardVector();

	FRotator RandomSpread = FRotator(
		FMath::RandRange(-SpreadAngle, SpreadAngle),
		FMath::RandRange(-SpreadAngle, SpreadAngle),
		0.0f
	);

	return RandomSpread.RotateVector(Forward).GetSafeNormal();
}

void ADrone::OnDetectionOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                     UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AEnemyBase* Enemy = Cast<AEnemyBase>(OtherActor);
	if (Enemy && !NearbyEnemies.Contains(Enemy))
	{
		NearbyEnemies.Add(Enemy);
	}
}

void ADrone::OnDetectionOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AEnemyBase* Enemy = Cast<AEnemyBase>(OtherActor);
	if (Enemy)
		{
		NearbyEnemies.Remove(Enemy);

		if (CurrentTarget == Enemy)
		{
			Cast<AAIController>(GetController())->GetBlackboardComponent()->ClearValue(TEXT("TargetEnemy"));
			Cast<AAIController>(GetController())->GetBlackboardComponent()->SetValueAsInt(TEXT("AttackType"), 0);
			CurrentTarget = nullptr;
		}
	}
}