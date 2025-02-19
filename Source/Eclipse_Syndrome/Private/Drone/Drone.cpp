#include "Drone/Drone.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Drone/DroneController.h"
#include "GameFramework/SpringArmComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"

ADrone::ADrone()
	: MoveForce(300000.f)
	, AntiForce(980.f)
	, AirResistance(0.9f)
	, MaxTiltAngle(20.0f)
	, InterpSpeed(1.0f)
{
	PrimaryActorTick.bCanEverTick = true;
	ComponentInit();
}

void ADrone::BeginPlay()
{
	Super::BeginPlay();

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
	SpringArmComp->TargetArmLength = 400;
	SpringArmComp->bUsePawnControlRotation = false;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
}

void ADrone::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (const TObjectPtr<UEnhancedInputComponent> EnhancedInput
		= Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (const TObjectPtr<ADroneController> PlayerController
			= Cast<ADroneController>(GetController()))
		{
			if (PlayerController->MoveAction)
			{
				EnhancedInput->BindAction(PlayerController->MoveAction
					, ETriggerEvent::Triggered
					,this
					, &ADrone::Move);
			}

			if (PlayerController->LookAction)
			{
				EnhancedInput->BindAction(PlayerController->LookAction
					, ETriggerEvent::Triggered
					,this
					, &ADrone::Look);
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
