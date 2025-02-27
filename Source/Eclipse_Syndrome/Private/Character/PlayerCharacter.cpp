#include "Character/PlayerCharacter.h"

#include "Character/PlayerCharacterController.h"
#include "Item/BaseItem.h"
#include "System/DefaultGameState.h"
#include "System/DefaultGameInstance.h"
#include "Weapon/Weapon.h"
#include "Weapon/WeaponAR1.h"
#include "Weapon/WeaponAR2.h"
#include "Weapon/WeaponSR.h"

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
	, NormalSpeed(500.f)
	, CurrentInventoryAmmos(100)
	, bCanFire(false)
	, bCanReload(false)
	, bCanTraceForItemPeeking(false)
	, bCanGrapple(false)
	, bIsWeaponEquipped(false)
	, BlendPoseVariable(0)
	, PeekingItem(nullptr)
	, CurrentWeapon(nullptr)
	, GrappleEndTime(0.5f) //fix
	, bIsWeaponEquippedBack(false)
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

	static ConstructorHelpers::FObjectFinder<UAnimMontage>ReloadAsset(TEXT("/Game/HJ/Animation/AM_ReloadAR1.AM_ReloadAR1"));
	if (ReloadAsset.Succeeded())
	{
		ReloadAnimMontage = ReloadAsset.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage>DamageAsset(TEXT("/Game/HJ/Animation/AM_Damaged.AM_Damaged"));
	if (DamageAsset.Succeeded())
	{
		DamageAnimMontage = DamageAsset.Object;
	}

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
			if (PlayerController->ShowInventoryAction)
			{
				EnhancedInputComponent->BindAction(PlayerController->ShowInventoryAction,
					ETriggerEvent::Started, this, &APlayerCharacter::ShowInventory);

				//EnhancedInputComponent->BindAction(PlayerController->ShowInventoryAction,
				//	ETriggerEvent::Completed, this, &APlayerCharacter::StopShowInventory);
			}
			if (PlayerController->DroneMoveCommandAction)
			{
				EnhancedInputComponent->BindAction(PlayerController->DroneMoveCommandAction,
					ETriggerEvent::Started, this, &APlayerCharacter::DroneMoveCommand);
			}
			if (PlayerController->PossessAction)
			{
				EnhancedInputComponent->BindAction(PlayerController->PossessAction,
					ETriggerEvent::Started, this, &APlayerCharacter::PossessToDrone);
			}
		}
	}
}

void APlayerCharacter::Tick(float DeltaTime)
{
	if (bCanTraceForItemPeeking)
		BeginTraceForPickItem();

	if (PeekingItem && bCanTraceForItemPeeking)
	{
		if (PeekingItem->ActorHasTag("Weapon"))
			Cast<AWeapon>(PeekingItem)->bIsPeeking = true;
		if (PeekingItem->ActorHasTag("Item"))
			Cast<ABaseItem>(PeekingItem)->bIsPeeking = true;
	}
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	Cast<ADefaultGameState>(GetWorld()->GetGameState())->SetPlayerCharacter(this);
}

void APlayerCharacter::Shoot()
{	
	if (CurrentWeapon)
	{
		if (CurrentWeapon->GetCurrentAmmo() <= 0)
		{
			return;
		}

		if (bCanFire)
		{
			//Real shooting 
			CurrentWeapon->Fire();		

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
			return;
		}
		if (PlusAmmo > 0)
		{
			PlusAmmo = FMath::Min(PlusAmmo, CurrentInventoryAmmos);
			CurrentWeapon->Reload(PlusAmmo);

			//Reload Animation
			UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
			AnimInstance->Montage_Play(ReloadAnimMontage);

			CurrentInventoryAmmos -= PlusAmmo;

			if (UGameInstance* GameInstance = GetGameInstance())
			{
				UDefaultGameInstance* DefaultGameInstance = Cast<UDefaultGameInstance>(GameInstance);
				if (DefaultGameInstance)
				{
					DefaultGameInstance->UseAmmo(PlusAmmo);
				}
			}		
		}
	}	
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
			FVector TraceStart = WorldLocation + WorldDirection * 100.f;
			FVector TraceEnd = TraceStart + (WorldDirection * 10000.f);

			FHitResult HitResult;
			FCollisionQueryParams TraceParams;
			TraceParams.AddIgnoredActor(this);

			bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, TraceParams);

			/*FColor LineColor = bHit ? FColor::Red : FColor::Blue;
			DrawDebugLine(GetWorld(), TraceStart, TraceEnd, LineColor, false, 2.0f, 0, 2.0f);*/

			if (bHit)
			{						
				PeekingItem = HitResult.GetActor();				
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

	if (PeekingItem)
	{
		if (Cast<AWeapon>(PeekingItem))
		{
			Cast<AWeapon>(PeekingItem)->bIsPeeking = false;
		}
		if (Cast<ABaseItem>(PeekingItem))
		{
			Cast<ABaseItem>(PeekingItem)->bIsPeeking = false;
		}
	}
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

void APlayerCharacter::EquipWeaponBack(int32 WeaponIdx)
{
	//attack animation -> can't change weapon
	if (bIsWeaponEquipped)
		return;
	GetWorldTimerManager().ClearTimer(FireRateTimerHandle);
	//idle animation -> spawn weapon by index
	if (WeaponIdx == 1)
	{
		TempWeapon = GetWorld()->SpawnActor<AWeaponAR1>();
	}
	else if (WeaponIdx == 2)
	{
		TempWeapon = GetWorld()->SpawnActor<AWeaponAR2>();
	}
	else if (WeaponIdx == 3)
	{
		TempWeapon = GetWorld()->SpawnActor<AWeaponSR>();
	}
	else if (WeaponIdx == 4)
	{
		//CurrentWeapon = GetWorld()->SpawnActor<AWeapon>();
	}	

	//already equip weapon
	if (CurrentWeapon || bIsWeaponEquippedBack)
	{		
		TObjectPtr<AWeapon> Dummy;
		Dummy = TempWeapon;
		TempWeapon = CurrentWeapon;
		CurrentWeapon = Dummy;
		TempWeapon->Destroy();
	}
	else
	{
		CurrentWeapon = TempWeapon;		
	}

	CurrentWeapon->SetActorEnableCollision(false);
	FName WeaponSocket(TEXT("back_socket"));
	CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocket);
	bIsWeaponEquippedBack = true;
}

void APlayerCharacter::UseHealthItem()
{
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		UDefaultGameInstance* DefaultGameInstance = Cast<UDefaultGameInstance>(GameInstance);
		if (DefaultGameInstance)
		{
			if (DefaultGameInstance->InventoryItem[1] > 0)
			{				
				//[TODO] how to get health amount?
				DefaultGameInstance->PlusHealth(20.f);
				DefaultGameInstance->InventoryItem[1] -= 1;
			}
		}
	}
}

int32 APlayerCharacter::GetCurrentWeaponAmmo()
{
	if (CurrentWeapon)
	{
		return CurrentWeapon->GetCurrentAmmo();
	}
	return 0;
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
	if (bCanFire && bIsWeaponEquipped)
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
}

void APlayerCharacter::PickUp(const FInputActionValue& value)
{
	if (PeekingItem != nullptr)
	{
		if (PeekingItem->ActorHasTag("Weapon"))
		{
			if (GetGameInstance())
			{
				UDefaultGameInstance* DefaultGameInstance = Cast<UDefaultGameInstance>(GetGameInstance());
				if (DefaultGameInstance)
				{
					int32 WeaponIdx = Cast<AWeapon>(PeekingItem)->GetWeaponNumber();
					DefaultGameInstance->AddWeapon(WeaponIdx);
				}
			}
			PeekingItem->Destroy();
		}

		else if(PeekingItem->ActorHasTag("Item"))
		{
			if (GetGameInstance())
			{
				UDefaultGameInstance* DefaultGameInstance = Cast<UDefaultGameInstance>(GetGameInstance());
				if (DefaultGameInstance)
				{
					int32 ItemIdx = Cast<ABaseItem>(PeekingItem)->GetItemNumber();
					int32 ItemAmount = Cast<ABaseItem>(PeekingItem)->GetItemAmount();
					DefaultGameInstance->AddItem(ItemIdx, ItemAmount);
				}
			}
			PeekingItem->Destroy();
		}
	}
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

void APlayerCharacter::ShowInventory(const FInputActionValue& value)
{	
	//if pressed UI
	if (GetController())
	{			
		if (!Cast<APlayerCharacterController>(GetController())->bIsInventoryUIOpen)
		{
			Cast<APlayerCharacterController>(GetController())->ShowInventoryUI();		
		}
		else
		{
			Cast<APlayerCharacterController>(GetController())->StopShowInventoryUI();
		}
	}	

}

//void APlayerCharacter::StopShowInventory(const FInputActionValue& value)
//{	
//	if (GetController())
//	{
//		Cast<APlayerCharacterController>(GetController())->StopShowInventoryUI();
//	}	
//}

void APlayerCharacter::PossessToDrone(const FInputActionValue& value)
{
	if (!value.Get<bool>())
	{
		Cast<APlayerCharacterController>(GetController())->SetPlayerPawn(this);
		Cast<APlayerCharacterController>(GetController())->ChangeMappingContext(1);
		Cast<APlayerCharacterController>(GetController())->ChangePossess(Cast<ADefaultGameState>(GetWorld()->GetGameState())->GetDrone());
	}
}

void APlayerCharacter::DroneMoveCommand(const FInputActionValue& value)
{
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, FString::Printf(TEXT("drone move command")));

	if (!value.Get<bool>())
	{
		FVector TargetLocation = FVector::ZeroVector;
		
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
				const FVector TraceStart = WorldLocation + WorldDirection * 100.f;
				const FVector TraceEnd = TraceStart + (WorldDirection * 1200.f);
				TargetLocation = TraceEnd;

				FHitResult HitResult;
				FCollisionQueryParams TraceParams;
				TraceParams.AddIgnoredActor(this);

				bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, TraceParams);

				FColor LineColor = bHit ? FColor::Orange : FColor::Cyan;
				DrawDebugLine(GetWorld(), TraceStart, TraceEnd, LineColor, false, 2.0f, 0, 2.0f);

				if (bHit)
				{						
					TargetLocation = HitResult.Location;// + HitResult.ImpactNormal * 100.0f;
				}
			}
		}

		// 드론을 찾아와서 컨트롤러를 가져옴
		if (ADefaultGameState* DefaultGameState = Cast<ADefaultGameState>(GetWorld()->GetGameState()))
		{
			if (APawn* DronePawn = DefaultGameState->GetDrone())
			{
				if (AController* DroneController = Cast<ADrone>(DronePawn)->GetController())
				{
					if (ADroneAIController* DroneAIController = Cast<ADroneAIController>(DroneController))
					{
						// movecommand를 해줌 이때, 타겟 로케이션이랑 상태 전환해주어야하는데..
						DroneAIController->DroneMoveCommand(TargetLocation);
					}
				}
			}
		}
	}
}

void APlayerCharacter::SetEnhancedInput()
{
	InputComponent->ClearActionBindings();
	SetupPlayerInputComponent(InputComponent);
}

float APlayerCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (UGameInstance* GameInstance = GetGameInstance())
	{
		UDefaultGameInstance* DefaultGameInstance = Cast<UDefaultGameInstance>(GameInstance);
		if (DefaultGameInstance)
		{
			DefaultGameInstance->MinusHealth(ActualDamage);

			//Damage Animation
			UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
			AnimInstance->Montage_Play(DamageAnimMontage);
		}
	}
	return ActualDamage;
}
