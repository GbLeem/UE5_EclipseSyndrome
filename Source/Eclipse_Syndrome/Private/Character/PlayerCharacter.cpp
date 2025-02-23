#include "Character/PlayerCharacter.h"

#include "Character/PlayerCharacterController.h"
#include "Item/BaseItem.h"
#include "Weapon/Weapon.h"

#include "CableComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/MeshComponent.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"

APlayerCharacter::APlayerCharacter()
	:SprintSpeed(800.f)
	,NormalSpeed(500.f)
	,MaxHealth(100.f)
	,CurrentHealth(100.f)
	,CurrentInventoryAmmos(40)
	,bCanFire(false)
	,bCanReload(false)
	, bCanTraceForItemPeeking(false)
	, bCanGrapple(false)
	, bIsWeaponEquipped(false)
	,BlendPoseVariable(0)
	,PeekingItem(nullptr)
	,CurrentWeapon(nullptr)
	,GrappleEndTime(0.5f) //fix
{
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->bUsePawnControlRotation = true;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComp->SetupAttachment(SpringArmComp);
	CameraComp->bUsePawnControlRotation = false;

	CableComp = CreateDefaultSubobject<UCableComponent>(TEXT("Hook"));
	CableComp->SetupAttachment(GetMesh());	
	FName HandSocket(TEXT("hand_l_socket"));
	CableComp->SetupAttachment(GetMesh(), HandSocket);	
	CableComp->CableWidth = 5.f;
	CableComp->NumSegments = 2;
	CableComp->SetVisibility(false);

	OriginRootRotator = RootComponent->GetComponentRotation();
	Tags.Add("Player");
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (APlayerCharacterController* PlayerController = Cast<APlayerCharacterController>(GetController()))
		{
			if (PlayerController->MoveAction)
			{
				EnhancedInputComponent->BindAction(PlayerController->MoveAction,
					ETriggerEvent::Triggered, this, &APlayerCharacter::Move);
			}
			if (PlayerController->LookAction)
			{
				EnhancedInputComponent->BindAction(PlayerController->LookAction,
					ETriggerEvent::Triggered, this, &APlayerCharacter::Look);
			}
			if (PlayerController->JumpAction)	
			{
				EnhancedInputComponent->BindAction(PlayerController->JumpAction,
					ETriggerEvent::Triggered, this, &APlayerCharacter::StartJump);

				EnhancedInputComponent->BindAction(PlayerController->JumpAction,
					ETriggerEvent::Completed, this, &APlayerCharacter::StopJump);
			}
			if (PlayerController->SprintAction)
			{
				EnhancedInputComponent->BindAction(PlayerController->SprintAction,
					ETriggerEvent::Triggered, this, &APlayerCharacter::StartSprint);

				EnhancedInputComponent->BindAction(PlayerController->SprintAction,
					ETriggerEvent::Completed, this, &APlayerCharacter::StopSprint);

			}
			if (PlayerController->ShootAction)
			{				
				EnhancedInputComponent->BindAction(PlayerController->ShootAction,
					ETriggerEvent::Triggered, this, &APlayerCharacter::StartShoot);
						
				EnhancedInputComponent->BindAction(PlayerController->ShootAction,
					ETriggerEvent::Completed, this, &APlayerCharacter::StopShoot);								
			}
			if (PlayerController->ReloadAction)
			{
				EnhancedInputComponent->BindAction(PlayerController->ReloadAction,
					ETriggerEvent::Started, this, &APlayerCharacter::Reload);
			}
			if (PlayerController->PickUpAction)
			{
				EnhancedInputComponent->BindAction(PlayerController->PickUpAction,
					ETriggerEvent::Started, this, &APlayerCharacter::PickUp);
			}
			if (PlayerController->EquipWeapon1Action)
			{
				EnhancedInputComponent->BindAction(PlayerController->EquipWeapon1Action,
					ETriggerEvent::Started, this, &APlayerCharacter::EquipWeapon1);
			}
			if (PlayerController->GrappleAction)
			{
				EnhancedInputComponent->BindAction(PlayerController->GrappleAction,
					ETriggerEvent::Started, this, &APlayerCharacter::Grapple);
			}
		}
	}
}

void APlayerCharacter::Tick(float DeltaTime)
{
	if (bCanTraceForItemPeeking)
		BeginTraceForPickItem();
}

void APlayerCharacter::Shoot()
{	
	if (CurrentWeapon)
	{
		if (CurrentWeapon->GetCurrentAmmo() <= 0)
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Black, FString::Printf(TEXT("you need to reload!")));
			return;
		}
		if (bCanFire)
		{
			//Real shooting 
			CurrentWeapon->Fire();			
			GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Black, FString::Printf(TEXT("shoot %d"), CurrentWeapon->GetCurrentAmmo()));		
			bCanFire = false;	

			//auto fire
			if (CurrentWeapon->GetAutoFire())
			{
				GetWorld()->GetTimerManager().SetTimer(FireRateTimerHandle, this, &APlayerCharacter::ResetShoot, CurrentWeapon->GetFireRate(), true);			
			}
		}
	}	
}	

void APlayerCharacter::Reloading()
{
	if (CurrentWeapon)
	{
		int PlusAmmo = CurrentWeapon->GetMaxAmmo() - CurrentWeapon->GetCurrentAmmo();
		if (CurrentInventoryAmmos <= 0)
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("not enough ammo")));
			return;
		}
		if (PlusAmmo > 0)
		{
			PlusAmmo = FMath::Min(PlusAmmo, CurrentInventoryAmmos);
			CurrentWeapon->Reload(PlusAmmo);
			CurrentInventoryAmmos -= PlusAmmo;
			GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("current : %d plus : %d"), CurrentWeapon->GetCurrentAmmo() , PlusAmmo));
		}
	}
}

void APlayerCharacter::PickUpItem()
{
	if (PeekingItem)
		Inventory.Add(PeekingItem);
}


void APlayerCharacter::BeginTraceForPickItem()
{
	//trace 하기
	APlayerCharacterController* PlayerController = Cast<APlayerCharacterController>(GetController());
	if (PlayerController)
	{
		int32 ScreenWidth;
		int32 ScreenHeight;

		PlayerController->GetViewportSize(ScreenWidth, ScreenHeight);
		const FVector2D ScreenCenter(ScreenWidth * 0.5f, ScreenHeight * 0.5f);

		FVector WorldLocation;
		FVector WorldDirection;
		if (PlayerController->DeprojectScreenPositionToWorld(ScreenCenter.X, ScreenCenter.Y, WorldLocation, WorldDirection))
		{
			FVector TraceStart = WorldLocation + WorldDirection*100.f;
			FVector TraceEnd = TraceStart + (WorldDirection * 10000.f);

			FHitResult HitResult;
			FCollisionQueryParams TraceParams;
			TraceParams.AddIgnoredActor(this);

			bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, TraceParams);

			FColor LineColor = bHit ? FColor::Red : FColor::Blue;
			DrawDebugLine(GetWorld(), TraceStart, TraceEnd, LineColor, false, 2.0f, 0, 2.0f);

			if (bHit)
			{				
				//if trace hit weapon
				//destroy origin weapon
				//spawn new weapon and attach to character
				if(Cast<AWeapon>(HitResult.GetActor()))
				{						
					CurrentWeapon = GetWorld()->SpawnActor<AWeapon>();
					CurrentWeapon->SetActorEnableCollision(false);
					
					FName WeaponSocket(TEXT("back_socket"));
					CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocket);					
					Cast<AWeapon>(HitResult.GetActor())->DestroyItem();
				}
			}
		}
	}
}

void APlayerCharacter::StartPeek()
{
	bCanTraceForItemPeeking = true;
}

void APlayerCharacter::StopPeek()
{
	bCanTraceForItemPeeking = false;
}

//character move
void APlayerCharacter::GrappleStart()
{
	if (bCanGrapple && !bIsWeaponEquipped)
	{
		FLatentActionInfo LatentInfo;
		LatentInfo.CallbackTarget = this;
		FRotator ResultRotator = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), GrappleHitPoint.ImpactPoint);
		UKismetSystemLibrary::MoveComponentTo(RootComponent, GrappleHitPoint.ImpactPoint, FRotator::ZeroRotator,/*ResultRotator,*/ false, false, 0.5f, false, EMoveComponentAction::Type::Move, LatentInfo);
		
		CableComp->SetAttachEndToComponent(RootComponent);
		CableComp->SetVisibility(false);
	}
}

void APlayerCharacter::GrappleEnd()
{
	bCanGrapple = false;	
}


void APlayerCharacter::Move(const FInputActionValue& value)
{
	if (!Controller)
	{
		return;
	}

	const FVector2D MoveInput = value.Get<FVector2D>();
	if (!FMath::IsNearlyZero(MoveInput.X))
	{
		AddMovementInput(GetActorForwardVector(), MoveInput.X);
	}
	if (!FMath::IsNearlyZero(MoveInput.Y))
	{
		AddMovementInput(GetActorRightVector(), MoveInput.Y);
	}
}

void APlayerCharacter::Look(const FInputActionValue& value)
{
	const FVector2D LookInput = value.Get<FVector2D>();
	
	AddControllerYawInput(LookInput.X);
	AddControllerPitchInput(LookInput.Y);
}

void APlayerCharacter::StartJump(const FInputActionValue& value)
{
	if (value.Get<bool>())
	{
		Jump();
	}
}

void APlayerCharacter::StopJump(const FInputActionValue& value)
{
	if (!value.Get<bool>())
	{
		StopJumping();
	}
}

void APlayerCharacter::StartSprint(const FInputActionValue& value)
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	}
}

void APlayerCharacter::StopSprint(const FInputActionValue& value)
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;		
	}
}

void APlayerCharacter::Reload(const FInputActionValue& value)
{
	//TODO		
	Reloading();		
}

void APlayerCharacter::StartShoot(const FInputActionValue& value)
{
	if (bCanFire)
	{
		Shoot();	
	}
	GrappleStart();
}

void APlayerCharacter::StopShoot(const FInputActionValue& value)
{
	if(CurrentWeapon && !CurrentWeapon->GetAutoFire())
	{
		bCanFire = true;
	}

	//if (!value.Get<bool>())
	//{
	//	//maybe animation ?
	//	//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Blue, FString::Printf(TEXT("Stop Fire")));
	//}
}

void APlayerCharacter::PickUp(const FInputActionValue& value)
{
	//empty
	if (Inventory.Num() != 0)
		PickUpItem();
}

//equip weapon
void APlayerCharacter::EquipWeapon1(const FInputActionValue& value)
{
	//equip -> idle
	if (bIsWeaponEquipped)
	{
		BlendPoseVariable = 0;
		bCanFire = false;
		bIsWeaponEquipped = false;

		if (CurrentWeapon)
		{
			FName WeaponSocket(TEXT("back_socket"));
			CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocket);
		}

		return;
	}

	//idle->equip
	if(!bIsWeaponEquipped && IsValid(CurrentWeapon))
	{
		BlendPoseVariable = 1; //for animation
		bCanFire = true; // for attack
		bIsWeaponEquipped = true;

		FName WeaponSocket(TEXT("hand_socket"));
		CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocket);
	}
}

void APlayerCharacter::Grapple(const FInputActionValue& value)
{	
	if (!bIsWeaponEquipped)
	{

		FVector Start = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->GetCameraLocation();	
		FVector End = Start + UKismetMathLibrary::GetForwardVector(UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->GetCameraRotation()) * 1500.f;
		TArray<AActor*> ActorsToIgnore;
		auto Channel = UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel1);
		bool bHit = UKismetSystemLibrary::SphereTraceSingle(GetWorld(), Start, End, 25.f, Channel, false,
			ActorsToIgnore, EDrawDebugTrace::Type::ForDuration, GrappleHitPoint, true);

		if (bHit)
		{
			CableComp->SetVisibility(true);
			bCanGrapple = true;
			CableComp->SetAttachEndToComponent(GrappleHitPoint.GetComponent());
		}
	}
}
