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
}

void ADrone::ComponentInit()
{
	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	SetRootComponent(CapsuleComp);
	CapsuleComp->SetCollisionProfileName("Pawn");
	CapsuleComp->SetSimulatePhysics(true);

	SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SceneComp->SetupAttachment(CapsuleComp);

	SkeletalMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMeshComp->SetupAttachment(SceneComp);
	
	PhysicsHandleComp = CreateDefaultSubobject<UPhysicsHandleComponent>(TEXT("PhysicsHandle"));

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComp->SetupAttachment(SceneComp);
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

	const FVector MoveInput = Value.Get<FVector>();

	FVector MoveToForce = FVector::ZeroVector;
	FVector MoveToDirection = FVector::ZeroVector;

	if (!FMath::IsNearlyZero(MoveInput.X))
	{
		//MoveToDirection += GetActorForwardVector() * MoveInput.X;
		MoveToDirection += SceneComp->GetForwardVector() * MoveInput.X;
	}
	if (!FMath::IsNearlyZero(MoveInput.Y))
	{
		//MoveToDirection += GetActorRightVector() * MoveInput.Y;
		MoveToDirection += SceneComp->GetRightVector() * MoveInput.Y;
	}
	if (!FMath::IsNearlyZero(MoveInput.Z))
	{
		//MoveToDirection += GetActorUpVector() * MoveInput.Z;
		MoveToDirection += SceneComp->GetUpVector() * MoveInput.Z;
	}

	MoveToDirection = MoveToDirection.GetSafeNormal();
	MoveToForce = MoveToDirection * MoveForce;
	CapsuleComp->AddForce(MoveToForce);
}

void ADrone::Look(const FInputActionValue& Value)
{
	const FVector2d LookInput = Value.Get<FVector2d>();

	float LookSensitivity = 1.0f;

	FRotator NewYawRotation(0.0f, LookInput.X * LookSensitivity, 0.0f);
	SceneComp->AddRelativeRotation(NewYawRotation);
	
	const float PitchLimit = 80.0f;
	const FRotator CurrentRotation = SpringArmComp->GetRelativeRotation();
	const float NewPitch = FMath::Clamp(CurrentRotation.Pitch + LookInput.Y * LookSensitivity, -PitchLimit, PitchLimit);
    
	SpringArmComp->SetRelativeRotation(FRotator(NewPitch, 0.0f, 0.0f));
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
