// Copyright Epic Games, Inc. All Rights Reserved.

#include "FpsCharacter.h"
#include "project_goldfishProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Engine/LocalPlayer.h"
#include "Weapon.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PlayerStats.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// Aproject_goldfishCharacter

AFpsCharacter::AFpsCharacter()
{
	// Character doesn't have a rifle at start
	bHasRifle = false;
	
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
		
	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	//Mesh1P->SetRelativeRotation(FRotator(0.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));

	// Create Player Stats component.
	Stats = CreateDefaultSubobject<APlayerStats>(TEXT("PlayerStats"));
}

void AFpsCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	// Add Input Mapping Context
	if (APlayerController* pPlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(pPlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	// Bind events
	GetMesh1P()->GetAnimInstance()->OnMontageEnded.AddDynamic(this, &AFpsCharacter::HandleOnMontageEnd);

	// Equip the default weapon
	EquipWeapon(m_cStartingWeapon);

	// Add the HUD to our viewport.
	if (m_cPlayerHud != nullptr)
	{
		UUserWidget* pHud = CreateWidget<UUserWidget>(Cast<APlayerController>(GetController()), m_cPlayerHud);
		pHud->AddToViewport(9999);

		// Update Health UI.
		OnPlayerHealthChanged.Broadcast(m_fHealth, m_fHealthMax);
	}

	// Set our starting stats.
	if (Stats != nullptr)
	{
		Stats->AddPoints(m_iStartingPoints);
	}
}

//////////////////////////////////////////////////////////////////////////// Input

void AFpsCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AFpsCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AFpsCharacter::Look);

		// Shoot
		EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Triggered, this, &AFpsCharacter::Shoot);

		// Reload
		EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Triggered, this, &AFpsCharacter::Reload);

		// Pause
		EnhancedInputComponent->BindAction(PauseAction, ETriggerEvent::Triggered, this, &AFpsCharacter::Pause);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}


void AFpsCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add movement 
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void AFpsCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AFpsCharacter::Pause(const FInputActionValue & Value)
{
	// TODO: Replace this with actual pause menu logic.
	UWorld* pWorld = GetWorld();
	UGameInstance* pGameInstance = pWorld->GetGameInstance();
	UGameplayStatics::OpenLevel(pWorld, FName("Map_Menu_MainMenu"), true);
	
	// Alternative quit flow.
	// APlayerController* pSpecificPlayer = pWorld->GetFirstPlayerController();
	// UKismetSystemLibrary::QuitGame(pWorld, pSpecificPlayer, EQuitPreference::Quit, true);
}

void AFpsCharacter::RefreshUI()
{
	OnPlayerHealthChanged.Broadcast(m_fHealth, m_fHealthMax);
	OnWeaponChanged.Broadcast(m_pCurrentlyEquippedWeapon->DisplayName);
	AmmoChanged();
}

void AFpsCharacter::SetHasRifle(bool bNewHasRifle)
{
	bHasRifle = bNewHasRifle;
}

bool AFpsCharacter::GetHasRifle()
{
	return bHasRifle;
}

void AFpsCharacter::Shoot()
{
	UAnimInstance* pAnimInstance = GetMesh1P()->GetAnimInstance();
	if (pAnimInstance != nullptr)
	{
		if (m_pReloadMontage != nullptr && m_pShootMontage != nullptr)
		{
			// Interrupt reload montage.
			pAnimInstance->Montage_Stop(0.2f, m_pReloadMontage);

			// Play reload montage.
			pAnimInstance->Montage_Play(m_pShootMontage, 1.0f);
		}
	}

	if (m_pCurrentlyEquippedWeapon != nullptr)
	{
		m_pCurrentlyEquippedWeapon->Fire();
		AmmoChanged();
	}
}

void AFpsCharacter::Reload()
{
	// Check if the weapon is reloadable.
	if (!m_pCurrentlyEquippedWeapon->CanReload())
		return;

	UAnimInstance* pAnimInstance = GetMesh1P()->GetAnimInstance();
	if (pAnimInstance != nullptr)
	{
		if (m_pReloadMontage != nullptr)
		{
			// Play reload montage.
			pAnimInstance->Montage_Play(m_pReloadMontage, 1.0f);
		}
	}
}

void AFpsCharacter::EquipWeapon(TSubclassOf<class AActor> cWeapon)
{
	APlayerController* pController = Cast<APlayerController>(GetController());
	const FRotator pRotation = pController->PlayerCameraManager->GetCameraRotation();
	const FVector pLocation = GetOwner()->GetActorLocation();

	FActorSpawnParameters pSpawnParams;
	pSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// Spawn & set weapon
	m_currentWeapon = GetWorld()->SpawnActor<AWeapon>(cWeapon, pLocation, pRotation, pSpawnParams);
	m_pCurrentlyEquippedWeapon = Cast<UTP_WeaponComponent>(m_currentWeapon->GetComponentByClass(UTP_WeaponComponent::StaticClass()));
	m_pCurrentlyEquippedWeapon->AttachWeapon(this);

	OnWeaponChanged.Broadcast(m_pCurrentlyEquippedWeapon->DisplayName);
}

void AFpsCharacter::HandleOnMontageEnd(UAnimMontage* pMontage, bool bInterrupted)
{
	// Get reload montage logic...
	if (pMontage->GetName().Contains("reload") && !bInterrupted)
	{
		// Check a weapon is equipped.
		if (m_pCurrentlyEquippedWeapon == nullptr)
			return;

		m_pCurrentlyEquippedWeapon->Reload();
		AmmoChanged();
	}
}

void AFpsCharacter::ReceiveDamage(int iAmount)
{
	m_fHealth -= iAmount;
	OnPlayerHealthChanged.Broadcast(m_fHealth, m_fHealthMax);

	if (m_fHealth <= 0)
	{
		UWorld* pWorld = GetWorld();
		FName levelName = FName(pWorld->GetName());
		UGameplayStatics::OpenLevel(pWorld, levelName, true);
	}
}

void AFpsCharacter::RecoverHealth(int iAmount)
{
	m_fHealth += iAmount;
	OnPlayerHealthChanged.Broadcast(m_fHealth, m_fHealthMax);
}

void AFpsCharacter::AmmoChanged()
{
	int iCurrentMagazineAmmo = m_pCurrentlyEquippedWeapon->GetCurrentMagazineAmmo();
	int iHolsteredAmmo = m_pCurrentlyEquippedWeapon->GetHolsteredAmmoAvailable();
	OnAmmoChanged.Broadcast(iCurrentMagazineAmmo, iHolsteredAmmo);
}
