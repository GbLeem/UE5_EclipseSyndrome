#include "Character/PlayerCamera.h"
#include "Camera/CameraComponent.h"

APlayerCamera::APlayerCamera()
{
	PrimaryActorTick.bCanEverTick = false;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	RootComponent = CameraComp;

	//CameraComp->FieldOfView = 200.f;
	//CameraComp->bUsePawnControlRotation = true;
	//RootComponent->SetRelativeLocation(FVector(0.f, -90.f, 0.f));
}
