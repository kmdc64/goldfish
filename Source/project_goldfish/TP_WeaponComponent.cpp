// Copyright Epic Games, Inc. All Rights Reserved.


#include "TP_WeaponComponent.h"
#include "project_goldfishCharacter.h"
#include "project_goldfishProjectile.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "HealthInterface.h"

// Sets default values for this component's properties
UTP_WeaponComponent::UTP_WeaponComponent()
{
	// Initialise the ammo.
	m_iCurrentAmmo = m_iClipSize;
	m_iTotalAmmo = m_iMaxAmmo;
}


void UTP_WeaponComponent::Fire()
{
	if (character == nullptr || character->GetController() == nullptr)
	{
		return;
	}

	APlayerController* playerController = Cast<APlayerController>(character->GetController());
	const FVector playerLocation = character->GetActorLocation();
	const FRotator spawnRotation = playerController->PlayerCameraManager->GetCameraRotation();
	
	UWorld* const world = GetWorld();
	if (world != nullptr)
	{
		// Check clip is not empty.
		if (m_iCurrentAmmo <= 0)
		{
			UGameplayStatics::PlaySoundAtLocation(world, EmptyClipSound, playerLocation);
			return;
		}
		
		// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
		const FVector spawnLocation = playerLocation + spawnRotation.RotateVector(MuzzleOffset);

		// Set up Query params.
		FCollisionQueryParams queryParams;
		queryParams.AddIgnoredActor(playerController->GetPawn());
		queryParams.AddIgnoredActor(GetOwner());

		// Setup the hit result.
		FHitResult outHit;

		// Perform the line trace.
		world->LineTraceSingleByChannel(outHit, spawnLocation, spawnLocation + (spawnRotation.Vector() * 3000), ECollisionChannel::ECC_Pawn, queryParams);
		DrawDebugLine(world, spawnLocation, spawnLocation + (spawnRotation.Vector() * 3000), FColor::Red, false, 1.0f, 5, 10.0f);

		// Try to hit a damageable object.
		IHealthInterface* pHealth = Cast<IHealthInterface>(outHit.GetActor());
		if (pHealth != nullptr)
		{
			float damageDealt = GetShotDamage();
			pHealth->ReceiveDamage(damageDealt);
		}
		else
		{
			// Play environmental sounds.
			auto sound = EnvironmentalSounds[FMath::RandRange(0, EnvironmentalSounds.Num() - 1)];
			UGameplayStatics::PlaySoundAtLocation(world, sound, playerLocation);
		}

		// Expend ammo.
		m_iCurrentAmmo = m_iCurrentAmmo - 1;
	}
	
	// Try and play the SFX.
	if (FireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, playerLocation);
	}

	// Try and spawn the muzzle flash PFX.
	if (MuzzleFlashPfx != nullptr)
	{
		// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
		const FVector spawnLocation = GetOwner()->GetActorLocation() + spawnRotation.RotateVector(MuzzleOffset);

		UNiagaraFunctionLibrary::SpawnSystemAtLocation(world, MuzzleFlashPfx, spawnLocation, spawnRotation);
	}
}

void UTP_WeaponComponent::Reload()
{
	// Fill the clip with available ammo.
	int reloadAmount = m_iClipSize - m_iCurrentAmmo;
	reloadAmount = UKismetMathLibrary::Min(reloadAmount, m_iTotalAmmo);
	m_iTotalAmmo -= reloadAmount;
	m_iCurrentAmmo += reloadAmount;
}

bool UTP_WeaponComponent::CanReload()
{
	bool clipFull = m_iCurrentAmmo == m_iClipSize;
	bool ammoAvailable = m_iTotalAmmo > 0;
	if (clipFull || !ammoAvailable)
		return false;

	return true;
}

void UTP_WeaponComponent::AttachWeapon(Aproject_goldfishCharacter* TargetCharacter)
{
	character = TargetCharacter;

	// Check that the character is valid, and has no rifle yet
	if (character == nullptr || character->GetHasRifle())
	{
		return;
	}

	// Attach the weapon to the First Person Character
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
	USkeletalMeshComponent* pCharacterMesh = character->GetMesh1P();
	AttachToComponent(pCharacterMesh, AttachmentRules, FName(TEXT("WeaponSocket")));
	
	// switch bHasRifle so the animation blueprint can switch to another animation set
	character->SetHasRifle(true);

	// Set up action bindings
	if (APlayerController* playerController = Cast<APlayerController>(character->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(playerController->GetLocalPlayer()))
		{
			// Set the priority of the mapping to 1, so that it overrides the Jump action with the Fire action when using touch input
			Subsystem->AddMappingContext(FireMappingContext, 1);
		}

		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(playerController->InputComponent))
		{
			// Fire
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &UTP_WeaponComponent::Fire);
		}
	}
}

float UTP_WeaponComponent::GetShotDamage()
{
    return FMath::RandRange(m_fDamagePerShotMin, m_fDamagePerShotMax);
}

void UTP_WeaponComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (character == nullptr)
	{
		return;
	}

	if (APlayerController* playerController = Cast<APlayerController>(character->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(playerController->GetLocalPlayer()))
		{
			Subsystem->RemoveMappingContext(FireMappingContext);
		}
	}
}