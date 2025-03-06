#include "Character/PlayerCharacter.h"

#include "Character/PlayerCamera.h"
#include "Character/PlayerCharacterController.h"
#include "Item/BaseItem.h"
#include "System/DefaultGameState.h"
#include "System/DefaultGameInstance.h"
#include "Weapon/Weapon.h"
#include "Weapon/WeaponAR1.h"
#include "Weapon/WeaponAR2.h"
#include "Weapon/WeaponSR.h"
#include "Weapon/NewWeapon.h"
#include "Weapon/WeaponShotgun.h"
#include "InteractableItem/PuzzleItem/KeyItem.h"
#include "InteractableItem/PuzzleItem/PuzzleBlock.h"

#include "CableComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/MeshComponent.h"
#include "EnhancedInputComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Materials/MaterialInterface.h"
#include "Sound/SoundBase.h"

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
	, bIsReloading(false)
	, bIsTPSMode(true)
	, bIsCrouch(false)
	, bMoveForward(true)
	, bIsRolling(false)
	, FooStepIdx(0)
{
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -88.f));
	GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));

	CableComp = CreateDefaultSubobject<UCableComponent>(TEXT("Hook"));
	CableComp->SetupAttachment(GetMesh());	
	FName HandSocket(TEXT("hand_l_socket"));
	CableComp->SetupAttachment(GetMesh(), HandSocket);	
	CableComp->CableWidth = 3.f;
	CableComp->NumSides = 5.f;
	CableComp->NumSegments = 60;
	CableComp->TileMaterial = 60;
	CableComp->SetVisibility(false);

	TPSSpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("TPS Spring Arm"));
	TPSSpringArmComp->SetupAttachment(RootComponent);
	TPSCamera = CreateDefaultSubobject<UChildActorComponent>(TEXT("TPS Camera actor"));
	TPSCamera->SetupAttachment(TPSSpringArmComp);
	TPSCamera->SetChildActorClass(APlayerCamera::StaticClass());
	TPSSpringArmComp->TargetArmLength = 200.f;
	TPSSpringArmComp->TargetOffset = FVector(0.f, 0.f, 100.f);
	TPSSpringArmComp->bUsePawnControlRotation = true;

	/*static ConstructorHelpers::FObjectFinder<USkeletalMesh>SkeletalMesh(TEXT("/Game/HJ/Assets/QuantumCharacter/Mesh/SKM_QuantumCharacter.SKM_QuantumCharacter"));
	if (SkeletalMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(SkeletalMesh.Object);
	}*/

	static ConstructorHelpers::FObjectFinder<UAnimMontage>ReloadAsset(TEXT("/Game/HJ/Animation/AM_ReloadAR1.AM_ReloadAR1"));
	if (ReloadAsset.Succeeded())
	{
		ReloadAnimMontage = ReloadAsset.Object;
	}

	//anim montage
	static ConstructorHelpers::FObjectFinder<UAnimMontage>ShotGunReloadAnimAsset(TEXT("/Game/HJ/Animation/AM_ReloadShotgun.AM_ReloadShotgun"));
	if (ShotGunReloadAnimAsset.Succeeded())
	{
		ShotgunReloadAnimMontage = ShotGunReloadAnimAsset.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage>DamageAsset(TEXT("/Game/HJ/Animation/AM_Damaged.AM_Damaged"));
	if (DamageAsset.Succeeded())
	{
		DamageAnimMontage = DamageAsset.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage>RollingAsset(TEXT("/Game/HJ/Animation/Rolling/AM_Rolling.AM_Rolling"));
	if (RollingAsset.Succeeded())
	{
		RollingAnimMontage = RollingAsset.Object;
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInterface>CableMat(TEXT("/Game/SH/MT_Rope.MT_Rope"));
	if (CableMat.Succeeded())
	{
		CableComp->SetMaterial(0, CableMat.Object);
	}

	static ConstructorHelpers::FObjectFinder<USoundBase>FootSound1(TEXT("/Game/HJ/Assets/Sound/Foot.Foot"));
	if (FootSound1.Succeeded())
	{
		FootStepSound.Add(FootSound1.Object);
	}
	static ConstructorHelpers::FObjectFinder<USoundBase>FootSound2(TEXT("/Game/HJ/Assets/Sound/Foot2.Foot2"));
	if (FootSound2.Succeeded())
	{
		FootStepSound.Add(FootSound2.Object);
	}
	static ConstructorHelpers::FObjectFinder<USoundBase>EquipSoundAsset(TEXT("/Game/HJ/Assets/Sound/cocked.cocked"));
	if (EquipSoundAsset.Succeeded())
	{
		EquipSound = EquipSoundAsset.Object;
	}

	/*static ConstructorHelpers::FClassFinder<UAnimInstance> AnimClass(TEXT("/Game/HJ/Animation/ABP_PlayerCharacter.ABP_PlayerCharacter_C"));
	if (AnimClass.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(AnimClass.Class);
	}*/

	//crouch setting
	if (GetMovementComponent())
	{
		GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;				
	}
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->bCanWalkOffLedgesWhenCrouching = true;
	}


	PlayerWeaponInventory.Add(1, nullptr);
	PlayerWeaponInventory.Add(2, nullptr);
	PlayerWeaponInventory.Add(3, nullptr);
	PlayerWeaponInventory.Add(4, nullptr);
	PlayerWeaponInventory.Add(5, nullptr);

	Tags.Add("Player");

	//[YJ] for pick up item
	static ConstructorHelpers::FObjectFinder<USoundBase> SoundAsset(TEXT("/Game/Yujin/Audio/PickUpItem.PickUpItem"));
	if (SoundAsset.Succeeded())
	{
		PickupItemSound = SoundAsset.Object;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to load pickup sound!"));
	}
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
			}
			if (PlayerController->ViewChangeAction)
			{
				EnhancedInputComponent->BindAction(PlayerController->ViewChangeAction,
					ETriggerEvent::Started, this, &APlayerCharacter::ChangeView);
			}
			if (PlayerController->ZoomAction)
			{
				EnhancedInputComponent->BindAction(PlayerController->ZoomAction,
					ETriggerEvent::Started, this, &APlayerCharacter::ZoomInOut);
			}
			if (PlayerController->CrouchAction)
			{
				EnhancedInputComponent->BindAction(PlayerController->CrouchAction,
					ETriggerEvent::Started, this, &APlayerCharacter::CrouchCharacter);
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
	
	//CheckFootStep();

	if (bIsSwinging)
	{
		HandleSwingMovement(DeltaTime);
	}
	else if (bIsPullingToAnchor)
	{
		HandlePullMovement(DeltaTime);
	}	

	/*if (CurrentWeapon)
	{
		if (!bIsTPSMode)
		{
			if (GetController())
			{
				APlayerCharacterController* PlayerCharacterController = Cast<APlayerCharacterController>(GetController());
				if (PlayerCharacterController)
				{
					FRotator CameraRotation = CurrentWeapon->WeaponSpringArmComp->GetComponentRotation();
					CameraRotation.Pitch = PlayerCharacterController->GetControlRotation().Pitch;
					CurrentWeapon->WeaponCameraComp->SetWorldRotation(CameraRotation);					
				}	
			}
		}
	}*/
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	Cast<ADefaultGameState>(GetWorld()->GetGameState())->SetPlayerCharacter(this);

	APlayerCharacterController* PlayerCharacterController = Cast<APlayerCharacterController>(GetController());
	if (PlayerCharacterController)
	{
		PlayerCharacterController->SetViewTargetWithBlend(TPSCamera->GetChildActor());
	}
	bIsTPSMode = true;
}

void APlayerCharacter::Shoot()
{	
	if (CurrentWeapon && !bIsReloading)
	{
		/*if (CurrentWeapon->GetCurrentAmmo() <= 0)
		{
			return;
		}*/

		if (bCanFire)
		{
			//Real shooting 
			CurrentWeapon->Fire();	

			//apply recoil
			Recoil();
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
	UDefaultGameInstance* DefaultGameInstance = Cast<UDefaultGameInstance>(GetGameInstance());
	if (DefaultGameInstance)
	{
		CurrentInventoryAmmos = DefaultGameInstance->GetCurrentInventoryAmmo();
	}
	
	if (CurrentWeapon)
	{
		int PlusAmmo = CurrentWeapon->GetMaxAmmo() - CurrentWeapon->GetCurrentAmmo();
		
		if (CurrentInventoryAmmos <= 0)
		{
			return;
		}
		if (bIsRolling)
			return;

		if (PlusAmmo > 0 && bIsWeaponEquipped)
		{
			PlusAmmo = FMath::Min(PlusAmmo, CurrentInventoryAmmos);
			CurrentWeapon->Reload(PlusAmmo);
			
			bIsReloading = true;

			//Reload Animation
			UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
			if (AnimInstance)
			{								
				if (CurrentWeapon->GetWeaponNumber() == 4)
					AnimInstance->Montage_Play(ShotgunReloadAnimMontage);
				else
					AnimInstance->Montage_Play(ReloadAnimMontage);
			}

			CurrentInventoryAmmos -= PlusAmmo;

			if (DefaultGameInstance)
			{
				DefaultGameInstance->UseAmmo(PlusAmmo);
			}
			/*if (UGameInstance* GameInstance = GetGameInstance())
			{
				UDefaultGameInstance* DefaultGameInstance = Cast<UDefaultGameInstance>(GameInstance);
			}*/		
		}
	}	
}

void APlayerCharacter::Recoil()
{
	APlayerCharacterController* PlayerCharacterController = Cast<APlayerCharacterController>(GetController());
	if (PlayerCharacterController)
	{
		CalculateRecoilValue();

		float PitchRecoil = FMath::RandRange(MinPitchRecoil, MaxPitchRecoil);
		float YawRecoil = FMath::RandRange(MinYawRecoil, MaxYawRecoil);

		PlayerCharacterController->AddPitchInput(-PitchRecoil);
		PlayerCharacterController->AddYawInput(YawRecoil);		
	}
}

void APlayerCharacter::CalculateRecoilValue()
{
	if (CurrentWeapon)
	{
		MinPitchRecoil = CurrentWeapon->MinPitchRecoil;
		MaxPitchRecoil = CurrentWeapon->MaxPitchRecoil;
		MinYawRecoil = CurrentWeapon->MinYawRecoil;
		MaxYawRecoil = CurrentWeapon->MaxYawRecoil;
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

void APlayerCharacter::EquipWeaponBack(int32 WeaponIdx)
{
	//attack animation -> can't change weapon
	if (bIsWeaponEquipped)
		return;

	//timer reset
	GetWorldTimerManager().ClearTimer(FireRateTimerHandle);
	
	//no weapon exist(check by idx) -> spawn
	if (PlayerWeaponInventory[WeaponIdx] == nullptr)
	{
		if (WeaponIdx == 1)
		{
			PlayerWeaponInventory[WeaponIdx] = GetWorld()->SpawnActor<AWeaponAR1>();
		}
		else if (WeaponIdx == 2)
		{
			PlayerWeaponInventory[WeaponIdx] = GetWorld()->SpawnActor<AWeaponAR2>();
		}
		else if (WeaponIdx == 3)
		{
			PlayerWeaponInventory[WeaponIdx] = GetWorld()->SpawnActor<AWeaponSR>();
		}
		else if (WeaponIdx == 4)
		{
			PlayerWeaponInventory[WeaponIdx] = GetWorld()->SpawnActor<AWeaponShotgun>();
		}	
		else if (WeaponIdx == 5)
		{
			PlayerWeaponInventory[WeaponIdx] = GetWorld()->SpawnActor<ANewWeapon>();
		}
	}

	if (IsValid(CurrentWeapon))
	{
		TObjectPtr<AWeapon> Temp = PlayerWeaponInventory[WeaponIdx];
		int32 Idx = CurrentWeapon->GetWeaponNumber();
		PlayerWeaponInventory[Idx] = CurrentWeapon;
		PlayerWeaponInventory[Idx]->SetActorHiddenInGame(true);
		CurrentWeapon = Temp;
		CurrentWeapon->SetActorHiddenInGame(false);
	}	
	else
	{
		CurrentWeapon = PlayerWeaponInventory[WeaponIdx];
	}
	
	//[ADD]
	CurrentWeapon->SetOwner(this);
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

//[YJ fixing]Connecting with Character
void APlayerCharacter::UseKeyItem()
{
	UE_LOG(LogTemp, Warning, TEXT("UseKeyItem called"));

	// GarageDoor
	AGarageDoor* GarageDoor = Cast<AGarageDoor>(
		UGameplayStatics::GetActorOfClass(GetWorld(), AGarageDoor::StaticClass()));

	if (GarageDoor)
	{
		if (GarageDoor->UnlockDoor())
		{
			UDefaultGameInstance* DefaultGameInstance = Cast<UDefaultGameInstance>(GetGameInstance());
			if (DefaultGameInstance)
			{
				if (DefaultGameInstance->SpecialSlotItemID != -1)
				{
					UE_LOG(LogTemp, Warning, TEXT("UseKeyItem - deleting key from inventory"));
					DefaultGameInstance->RemoveSpecialItem();
					DefaultGameInstance->InventoryItem[3] -= 1;
					if (ADefaultGameState* GameState = GetWorld()->GetGameState<ADefaultGameState>())
					{
						GameState->UpdateHUD();
					}
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("UseKeyItem - too far from the door"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("No GarageDoor found!"));
	}
	
}


//[YJ fixing]Connecting with Character(already in inventory slot + trying to insert into puzzleslot)
void APlayerCharacter::UsePuzzleBlockItem()
{
	UE_LOG(LogTemp, Warning, TEXT("UsePuzzleBlockItem called"));
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		UDefaultGameInstance* DefaultGameInstance = Cast<UDefaultGameInstance>(GameInstance);
		if (DefaultGameInstance)
		{
			if (DefaultGameInstance->InventoryItem[3] > 0)
			{
				int32 BlockID = DefaultGameInstance->PuzzleBlockIDMap[3];

				FVector SpawnLocation = GetActorLocation() + (GetActorForwardVector() * 100.0f); 
				FRotator SpawnRotation = GetActorRotation();

				FActorSpawnParameters SpawnParams;
				APuzzleBlock* PuzzleBlock = GetWorld()->SpawnActor<APuzzleBlock>(APuzzleBlock::StaticClass(), SpawnLocation, SpawnRotation, SpawnParams);
				if (PuzzleBlock)
				{
					PuzzleBlock->SetBlockID(BlockID);
					UE_LOG(LogTemp, Warning, TEXT("PuzzleBlock spawned in front of player"));
					
					DefaultGameInstance->InventoryItem[3] = 0;
					DefaultGameInstance->SpecialSlotItemID = -1;
					PuzzleBlock->ActivateItem(this);
					
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("Failed to spawn PuzzleBlock"));
				}
			}
		}
	}
}

void APlayerCharacter::CheckFootStep()
{	
	if (GetWorld()->GetTimeSeconds() - LastFootStepTime < FootStepCoolDown)
	{
		return;
	}

	if (bIsCrouch || bIsRolling)
	{
		return;
	}
	
	FVector Start = GetActorLocation();
	FVector End = Start - FVector(0.f, 0.f, 100.f);

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	FooStepIdx = 0;

	//DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 2.f, 0.f, 2.f);
	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
	{
		//DrawDebugSphere(GetWorld(), Hit.Location, 10.0f, 12, FColor::Blue, false, 2.0f);

		UGameplayStatics::PlaySoundAtLocation(this, FootStepSound[FooStepIdx], Hit.Location);
		FooStepIdx++;
		FooStepIdx %= 2;
	}

	LastFootStepTime = GetWorld()->GetTimeSeconds();
}


int32 APlayerCharacter::GetCurrentWeaponAmmo()
{
	if (CurrentWeapon)
	{
		return CurrentWeapon->GetCurrentAmmo();
	}
	return 0;
}

FTransform APlayerCharacter::GetWeaponHandSocket()
{
	if (CurrentWeapon)
	{
		return CurrentWeapon->GetHandSocket();
	}	
	return FTransform::Identity;
}

void APlayerCharacter::Move(const FInputActionValue& value)
{
	if (!Controller)
	{
		return;
	}
	
	const FVector2D MoveInput = value.Get<FVector2D>();
	// for swing
	if (bIsSwinging && !AnchorLocation.IsZero() && !GetCharacterMovement()->IsMovingOnGround())
	{
		FVector Direction = GetActorForwardVector() * MoveInput.X + GetActorRightVector() * MoveInput.Y;
		FVector SwingBoost = Direction * SwingBoostStrength;
		GetCharacterMovement()->AddForce(SwingBoost);
	}
	else
	{
		// 일반 이동		
		CheckFootStep();

		if (!FMath::IsNearlyZero(MoveInput.X))
		{
			AddMovementInput(GetActorForwardVector(), MoveInput.X);
		}
		if (!FMath::IsNearlyZero(MoveInput.Y))
		{
			AddMovementInput(GetActorRightVector(), MoveInput.Y);
		}
	}
	if (MoveInput.X > 0)
		bMoveForward = true;
	else
		bMoveForward = false;
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
		if (bIsSwinging)
		{
			if (!GetCharacterMovement()->IsMovingOnGround())
			{
				FVector LaunchVelocity = GetCharacterMovement()->Velocity + (FVector::UpVector * 500.0f);
				GetCharacterMovement()->Velocity = LaunchVelocity;
			}
			EndSwing();
		}
		else
		{
 			Jump();
		}
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
	if (GetCharacterMovement() && bMoveForward)
	{
		FootStepCoolDown = 0.25f;
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	}
}

void APlayerCharacter::StopSprint(const FInputActionValue& value)
{
	if (GetCharacterMovement())
	{
		FootStepCoolDown = 0.3f;
		GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;		
	}
}

void APlayerCharacter::Reload(const FInputActionValue& value)
{
	UDefaultGameInstance* DefaultGameInstance = Cast<UDefaultGameInstance>(GetGameInstance());
	if (DefaultGameInstance)
	{
		if (DefaultGameInstance->GetCurrentInventoryAmmo() > 0)
		{
			Reloading();
		}
	}
}

void APlayerCharacter::StartShoot(const FInputActionValue& value)
{
	if (bCanFire && bIsWeaponEquipped && !bIsRolling)
	{
		StartDroneAttack();
		Shoot();	
	}
	if (bIsSwinging)
	{
		GrappleStart();
	}
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
		//[YJ fixing]
		else if(PeekingItem->ActorHasTag("Item"))
		{
			if (GetGameInstance())
			{
				UDefaultGameInstance* DefaultGameInstance = Cast<UDefaultGameInstance>(GetGameInstance());
				if (DefaultGameInstance)
				{
					ABaseItem* BaseItem = Cast<ABaseItem>(PeekingItem);
					if (BaseItem)
					{//key

						if (PickupItemSound)
						{
							UGameplayStatics::PlaySoundAtLocation(
								GetWorld(),
								PickupItemSound,
								GetActorLocation()
							);
						}
						if (Cast<AKeyItem>(BaseItem))
						{
							int32 ItemIdx = BaseItem->GetItemNumber();
							int32 ItemAmount = BaseItem->GetItemAmount();
							DefaultGameInstance->AddItem(ItemIdx, ItemAmount, EItemType::Key);
							UE_LOG(LogTemp, Warning, TEXT("Picked Up Item: %d"), ItemIdx);
							PeekingItem->Destroy();
						}
						else if (Cast<APuzzleBlock>(BaseItem))
						{//puzzle block
							if (DefaultGameInstance->InventoryItem[3] == 0)//if inventory[3] is free
							{
								int32 ItemIdx = BaseItem->GetItemNumber();
								int32 ItemAmount = BaseItem->GetItemAmount();
								int32 BlockID = Cast<APuzzleBlock>(BaseItem)->GetBlockID();
								DefaultGameInstance->AddItem(ItemIdx, ItemAmount, EItemType::PuzzleBlock, BlockID);
								UE_LOG(LogTemp, Warning, TEXT("Picked Up Item: %d / BlockID: %d"), ItemIdx, BlockID);
								PeekingItem->Destroy();
							}
							else
							{
								UE_LOG(LogTemp, Warning, TEXT("inventory already full"));
							}
							
						}
						else
						{
							int32 ItemIdx = BaseItem->GetItemNumber();
							int32 ItemAmount = BaseItem->GetItemAmount();
							DefaultGameInstance->AddItem(ItemIdx, ItemAmount);
							UE_LOG(LogTemp, Warning, TEXT("Picked Up Item: %d"), ItemIdx);
							PeekingItem->Destroy();
						}
						
					}
					
				}
			}
			
		}


	}
}

//equip weapon
void APlayerCharacter::EquipWeapon1(const FInputActionValue& value)
{
	//equip -> idle
	if (bIsWeaponEquipped && !bIsReloading)
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
		UGameplayStatics::PlaySoundAtLocation(this, EquipSound, GetActorLocation());

		BlendPoseVariable = 1; //for animation
		bCanFire = true; // for attack
		bIsWeaponEquipped = true;

		FName WeaponSocket(TEXT("hand_socket"));
		FName SRWeaponSocekt(TEXT("hand_SR_Socket"));

		if (CurrentWeapon->GetWeaponNumber() == 5)
		{
			CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, SRWeaponSocekt);
		}
		else
		{
			CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocket);
		}
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
			CableComp->SetAttachEndToComponent(GrappleHitPoint.GetComponent());
			CableComp->EndLocation = FVector::ZeroVector;
			AnchorLocation = GrappleHitPoint.GetActor()->GetActorLocation();
			float Distance = FVector::Dist(GetActorLocation(), AnchorLocation);
			CableComp->CableLength = FVector::Dist(GetMesh()->GetSocketLocation(TEXT("hand_l_socket")), AnchorLocation); // CableComp->NumSegments;
			MaxWebLength = Distance;
			
			bIsSwinging = true;
		}
	}
}

void APlayerCharacter::HandleSwingMovement(float DeltaTime)
{
	if (!bIsSwinging || AnchorLocation.IsZero()) return;

	FVector PlayerLocation = GetActorLocation();
	float Distance = FVector::Distance(PlayerLocation, AnchorLocation);

	if (GetCharacterMovement()->IsMovingOnGround())
	{
		MaxWebLength = Distance;
	}
	else
	{
		const FVector Velocity = GetCharacterMovement()->Velocity;
		const FVector AnchorDirection = (AnchorLocation - PlayerLocation).GetSafeNormal();
		const FVector GravityForce = FVector(0.0f, 0.0f, GetCharacterMovement()->GetGravityZ());

		if (Distance > MaxWebLength)
		{
			const FVector Correction = AnchorDirection * (Distance - MaxWebLength);
			SetActorLocation(PlayerLocation + Correction);

			const FVector RestoreForce = AnchorDirection * (Distance - MaxWebLength) * 500.0f;
			GetCharacterMovement()->AddForce(RestoreForce);

			const FVector TangentialVelocity = Velocity - AnchorDirection * FVector::DotProduct(Velocity, AnchorDirection);
			GetCharacterMovement()->Velocity = TangentialVelocity;
		
			GetCharacterMovement()->Velocity = GetCharacterMovement()->Velocity.GetClampedToMaxSize(MaxSwingSpeed);
		}
		
		// GEngine->AddOnScreenDebugMessage(1, 3.0f, FColor::Orange,FString::Printf(TEXT("RopeLen : %.2f"), MaxWebLength));
		// GEngine->AddOnScreenDebugMessage(2, 3.0f, FColor::Red,FString::Printf(TEXT("Distance : %.2f"), Distance));
		
		GetCharacterMovement()->AddForce(GravityForce);
	}
}

void APlayerCharacter::GrappleStart()
{
	if (bIsSwinging)
	{
		bIsSwinging = false;
		bIsPullingToAnchor = true;

		UPrimitiveComponent* AnchorComp = Cast<UPrimitiveComponent>(CableComp->GetAttachedActor()->GetRootComponent());
		if (AnchorComp)
		{
			const FVector PlayerLocation = GetActorLocation();
			const FVector AnchorDirection = (AnchorLocation - PlayerLocation).GetSafeNormal();
			const FVector AnchorExtent = AnchorComp->Bounds.BoxExtent;
			const float AnchorRadius = AnchorExtent.Size()+ 50.f;
				
			AnchorLocation -= AnchorDirection * AnchorRadius;
			// DrawDebugSphere(GetWorld(), AnchorLocation, 30, 30, FColor::Red, false, 10.0f);
		}
		
		if (GetCharacterMovement()->IsMovingOnGround())
		{
			Jump();
		}
	}
}

void APlayerCharacter::HandlePullMovement(float DeltaTime)
{
	if (!bIsPullingToAnchor) return;

	const FVector PlayerLocation = GetActorLocation();
	const FVector AnchorDirection = (AnchorLocation - PlayerLocation).GetSafeNormal();
	const float Distance = FVector::Distance(PlayerLocation, AnchorLocation);
	
	const float SpeedFactor = FMath::Clamp(Distance / 1000.0f/*MaxWebLength*/, 0.1f, 1.0f);
	const float PullSpeed = FMath::Lerp(PullMovementMinSpeed, PullMovementMaxSpeed, SpeedFactor);

	const FVector NewVelocity = AnchorDirection * PullSpeed;
	GetCharacterMovement()->Velocity = NewVelocity;
	
	if (Distance <= MinDistance)
	{
		const FVector SwingStartVelocity = AnchorDirection * 300.0f; 
		GetCharacterMovement()->Velocity = SwingStartVelocity;
		EndSwing();
	}
}

void APlayerCharacter::EndSwing()
{
	bIsSwinging = false;
	bIsPullingToAnchor = false;
	CableComp->SetAttachEndToComponent(RootComponent);
	CableComp->SetVisibility(false);
}

void APlayerCharacter::GrappleEnd()
{
	bCanGrapple = false;	
}

void APlayerCharacter::StartDroneAttack()
{
	if (ADefaultGameState* DefaultGameState = Cast<ADefaultGameState>(GetWorld()->GetGameState()))
	{
		if (ADrone* Drone = DefaultGameState->GetDrone())
		{
			if (AAIController* AIController = Cast<AAIController>(Drone->GetController()))
			{
				Cast<ADroneAIController>(AIController)->DroneAttack();
			}
		}
	}
}

void APlayerCharacter::StartSlowMotion()
{
	GetWorldTimerManager().SetTimer(SlowDownTimerHandle, this, &APlayerCharacter::UpdateTimeDilation, 0.01f, true);
}

void APlayerCharacter::UpdateTimeDilation()
{
	static float ElapsedTime = 0.0f;
	float Duration = 0.5f;
	
	ElapsedTime += 0.01f;
	
	float Alpha = FMath::Clamp(ElapsedTime / Duration, 0.0f, 1.0f);
	float SineAlpha = 0.5f * (1.0f - FMath::Cos(Alpha * PI));
	
	float StartDilation = 1.0f;
	float TargetDilation = 0.2f;
	
	float NewDilation = FMath::Lerp(StartDilation, TargetDilation, SineAlpha);
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), NewDilation);
	
	if (Alpha >= 1.0f)
	{
		GetWorldTimerManager().ClearTimer(SlowDownTimerHandle);
		ElapsedTime = 0.0f;
	}
}

void APlayerCharacter::ResetTimeDilation()
{
	GetWorldTimerManager().SetTimer(SlowDownTimerHandle, this, &APlayerCharacter::UpdateTimeDilationToNormal, 0.01f, true);
}

void APlayerCharacter::UpdateTimeDilationToNormal()
{
	static float ElapsedTime = 0.0f;
	float Duration = 0.5f;

	ElapsedTime += 0.01f;

	float Alpha = FMath::Clamp(ElapsedTime / Duration, 0.0f, 1.0f);
	float SineAlpha = 0.5f * (1.0f - FMath::Cos(Alpha * PI));

	float StartDilation = 0.2f;
	float TargetDilation = 1.0f;

	float NewDilation = FMath::Lerp(StartDilation, TargetDilation, SineAlpha);
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), NewDilation);

	if (Alpha >= 1.0f)
	{
		GetWorldTimerManager().ClearTimer(SlowDownTimerHandle);
		ElapsedTime = 0.0f;
	}
}

void APlayerCharacter::ShowInventory(const FInputActionValue& value)
{	
	//if pressed UI
	if (GetController())
	{			
		if (!Cast<APlayerCharacterController>(GetController())->bIsInventoryUIOpen)
		{
			StartSlowMotion();
			Cast<APlayerCharacterController>(GetController())->ShowInventoryUI();		
		}
		else
		{
			UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.0f);
			GetWorldTimerManager().ClearTimer(SlowDownTimerHandle);
			Cast<APlayerCharacterController>(GetController())->StopShowInventoryUI();
		}
	}	
}

void APlayerCharacter::PossessToDrone(const FInputActionValue& value)
{
	if (!value.Get<bool>())
	{
		Cast<APlayerCharacterController>(GetController())->SetPlayerPawn(this);
		Cast<APlayerCharacterController>(GetController())->ChangeMappingContext(1);
		Cast<ADefaultGameState>(GetWorld()->GetGameState())->GetDrone()->GetCameraSceneComponent()->SetRelativeRotation(FRotator(0, 0, 0));
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
					TargetLocation = HitResult.Location + HitResult.ImpactNormal * 10.0f;
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

//[TODO] Change Name / this function is for rolling 
void APlayerCharacter::ChangeView(const FInputActionValue& value)
{
	if (bMoveForward && !bIsReloading && !bIsRolling)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			bIsRolling = true;
			AnimInstance->Montage_Play(RollingAnimMontage);
			//GetCharacterMovement()->MaxWalkSpeed = 1000.f;
		}
	}
}

void APlayerCharacter::ZoomInOut(const FInputActionValue& value)
{	
	if (bIsWeaponEquipped && !bIsRolling)
	{
		if (CurrentWeapon->GetWeaponNumber() == 4)
			return;
		if (GetController())
		{
			APlayerCharacterController* PlayerCharacterController = Cast<APlayerCharacterController>(GetController());

			if (PlayerCharacterController)
			{
				if (bIsTPSMode)
				{
					if (CurrentWeapon)
					{
						PlayerCharacterController->SetViewTargetWithBlend(CurrentWeapon->WeaponCameraComp->GetChildActor(), 0.2f);									
					}					
					bIsTPSMode = false;
				}
				else
				{
					PlayerCharacterController->SetViewTargetWithBlend(TPSCamera->GetChildActor(), 0.2f);

					bIsTPSMode = true;
				}
			}
		} 
	}
}

void APlayerCharacter::CrouchCharacter(const FInputActionValue& value)
{
	if (!bIsCrouch)
	{		
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString::Printf(TEXT("crouch")));
		Crouch();
		bIsCrouch = true;
	}
	else
	{
		UnCrouch();
		bIsCrouch = false;
	}
}

void APlayerCharacter::SetEnhancedInput()
{
	InputComponent->ClearActionBindings();
	SetupPlayerInputComponent(InputComponent);
}

float APlayerCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	//[TEST] no damage when character rolls
	if (!bIsRolling)
	{
		float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

		if (UGameInstance* GameInstance = GetGameInstance())
		{
			UDefaultGameInstance* DefaultGameInstance = Cast<UDefaultGameInstance>(GameInstance);
			if (DefaultGameInstance)
			{
				DefaultGameInstance->MinusHealth(ActualDamage);

				if (DefaultGameInstance->GetCurrentHealth() <= 0)
				{
					GetMesh()->SetSimulatePhysics(true);
					GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
				}

				//Damage Animation
				if (bIsTPSMode)
				{
					UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
					AnimInstance->Montage_Play(DamageAnimMontage);
				}
				bIsReloading = false;
			}
		}
		return ActualDamage;	
	}
	return DamageAmount;
}
