// Copyright Epic Games, Inc. All Rights Reserved.


#include "TP_WeaponComponent.h"
#include "project_goldfishCharacter.h"
#include "project_goldfishProjectile.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

// Sets default values for this component's properties
UTP_WeaponComponent::UTP_WeaponComponent()
{

}


void UTP_WeaponComponent::Fire()
{
	if (character == nullptr || character->GetController() == nullptr)
	{
		return;
	}

	// Try and line trace.
	UWorld* const world = GetWorld();
	if (world != nullptr)
	{
		APlayerController* playerController = Cast<APlayerController>(character->GetController());
		const FRotator spawnRotation = playerController->PlayerCameraManager->GetCameraRotation();
		// m_vMuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
		const FVector spawnLocation = character->GetActorLocation() + spawnRotation.RotateVector(m_vMuzzleOffset);

		// Set up Query params.
		FCollisionQueryParams queryParams;
		queryParams.AddIgnoredActor(playerController->GetPawn());
		queryParams.AddIgnoredActor(GetOwner());

		// Setup the hit result.
		FHitResult outHit;

		// Perform the line trace.
		world->LineTraceSingleByChannel(outHit, spawnLocation, spawnLocation + (spawnRotation.Vector() * 3000), ECollisionChannel::ECC_Pawn, queryParams);
		DrawDebugLine(world, spawnLocation, spawnLocation + (spawnRotation.Vector() * 3000), FColor::Red, false, 1.0f, 5, 10.0f);
	}
	
	// Try and play the SFX.
	if (m_pFireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, m_pFireSound, character->GetActorLocation());
	}

	// Try and spawn the muzzle flash PFX.
	if (m_pMuzzleFlash != nullptr)
	{
		APlayerController* playerController = Cast<APlayerController>(character->GetController());
		const FRotator spawnRotation = playerController->PlayerCameraManager->GetCameraRotation();
		// m_vMuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
		const FVector spawnLocation = GetOwner()->GetActorLocation() + spawnRotation.RotateVector(m_vMuzzleOffset);

		UNiagaraFunctionLibrary::SpawnSystemAtLocation(world, m_pMuzzleFlash, spawnLocation, spawnRotation);
	}
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