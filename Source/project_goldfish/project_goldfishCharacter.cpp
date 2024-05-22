// Copyright Epic Games, Inc. All Rights Reserved.

#include "project_goldfishCharacter.h"
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
#include "PlayerStats.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// Aproject_goldfishCharacter

Aproject_goldfishCharacter::Aproject_goldfishCharacter()
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

void Aproject_goldfishCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	// Bind events
	GetMesh1P()->GetAnimInstance()->OnMontageEnded.AddDynamic(this, &Aproject_goldfishCharacter::HandleOnMontageEnd);

	// Equip the default weapon
	EquipWeapon();

	// Add the HUD to our viewport.
	if (m_cPlayerHud != nullptr)
	{
		UUserWidget* Hud = CreateWidget<UUserWidget>(Cast<APlayerController>(GetController()), m_cPlayerHud);
		Hud->AddToViewport(9999);
	}

	// Set our starting stats.
	if (Stats != nullptr)
	{
		Stats->AddPoints(m_iStartingPoints);
	}
}

//////////////////////////////////////////////////////////////////////////// Input

void Aproject_goldfishCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &Aproject_goldfishCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &Aproject_goldfishCharacter::Look);

		// Shoot
		EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Triggered, this, &Aproject_goldfishCharacter::Shoot);

		// Reload
		EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Triggered, this, &Aproject_goldfishCharacter::Reload);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}


void Aproject_goldfishCharacter::Move(const FInputActionValue& Value)
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

void Aproject_goldfishCharacter::Look(const FInputActionValue& Value)
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

void Aproject_goldfishCharacter::SetHasRifle(bool bNewHasRifle)
{
	bHasRifle = bNewHasRifle;
}

bool Aproject_goldfishCharacter::GetHasRifle()
{
	return bHasRifle;
}

void Aproject_goldfishCharacter::Shoot()
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
	}
}

void Aproject_goldfishCharacter::Reload()
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

void Aproject_goldfishCharacter::EquipWeapon()
{
	APlayerController* pController = Cast<APlayerController>(GetController());
	const FRotator pRotation = pController->PlayerCameraManager->GetCameraRotation();
	const FVector pLocation = GetOwner()->GetActorLocation();

	FActorSpawnParameters pSpawnParams;
	pSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// Spawn weapon
	m_currentWeapon = GetWorld()->SpawnActor<AWeapon>(m_cWeapon, pLocation, pRotation, pSpawnParams);
	// Set weapon
	m_pCurrentlyEquippedWeapon = Cast<UTP_WeaponComponent>(m_currentWeapon->GetComponentByClass(UTP_WeaponComponent::StaticClass()));
	m_pCurrentlyEquippedWeapon->AttachWeapon(this);
}

void Aproject_goldfishCharacter::HandleOnMontageEnd(UAnimMontage* a_pMontage, bool a_bInterrupted)
{
	// Get reload montage logic...
	if (a_pMontage->GetName().Contains("reload") && !a_bInterrupted)
	{
		// Check a weapon is equipped.
		if (m_pCurrentlyEquippedWeapon == nullptr)
			return;

		m_pCurrentlyEquippedWeapon->Reload();
	}
}

void Aproject_goldfishCharacter::ReceiveDamage(int amount)
{
	m_fHealth -= amount;

	if (m_fHealth <= 0)
	{
		UWorld* world = GetWorld();
		FName levelName = FName(world->GetName());
		UGameplayStatics::OpenLevel(world, levelName, true);
	}
}

void Aproject_goldfishCharacter::RecoverHealth(int amount)
{
	m_fHealth += amount;
}