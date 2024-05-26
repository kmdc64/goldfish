// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "TP_WeaponComponent.generated.h"

class AFpsCharacter;

UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECT_GOLDFISH_API UTP_WeaponComponent : public USkeletalMeshComponent
{
	GENERATED_BODY()

public:
	/** Sets default values for this component's properties */
	UTP_WeaponComponent();

	/** Attaches the actor to a FirstPersonCharacter. */
	UFUNCTION(BlueprintCallable, Category="Weapon")
	void AttachWeapon(AFpsCharacter* TargetCharacter);

	/** Make the weapon shoot. */
	UFUNCTION(BlueprintCallable, Category="Weapon")
	void Fire();

	/** Fill the weapon with ammo from the available pool. */
	UFUNCTION(BlueprintCallable, Category="Weapon")
	void Reload();

	/** Return true if there is available ammo to reload. */
	UFUNCTION(BlueprintCallable, Category="Weapon")
	virtual bool CanReload();

	// Get a damage value within the damage range.
	float GetShotDamage();

	// Get the ammo in the weapon's current magazine.
	int GetCurrentMagazineAmmo();

	// Get the holstered ammo available for the weapon.
	int GetHolsteredAmmoAvailable();

	// Name of the weapon.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Information", meta=(AllowPrivateAccess = "true"))
	FString DisplayName;

	// SFX that plays upon firing.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound", meta=(AllowPrivateAccess = "true"))
	USoundBase* FireSound;

	// SFX that plays upon attempting to fire with an empty clip.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound", meta=(AllowPrivateAccess = "true"))
	USoundBase* EmptyClipSound;

	// SFX that plays upon attempting to fire into the environment.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound", meta=(AllowPrivateAccess = "true"))
	TArray<USoundBase*> EnvironmentalSounds;

	// A collection of various impact SFX.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound", meta=(AllowPrivateAccess = "true"))
	TArray<USoundBase*> ImpactSounds;

	// The muzzle flash offset.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay, meta=(AllowPrivateAccess = "true"))
	FVector MuzzleOffset = FVector(100.0f, 0.0f, 10.0f);;

	// The muzzle flash particle system.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FX", meta=(AllowPrivateAccess = "true"))
	UNiagaraSystem* MuzzleFlashPfx;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputMappingContext* FireMappingContext;

	/** Fire Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* FireAction;

protected:
	/** Ends gameplay for this component. */
	UFUNCTION()
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
	float m_fDamagePerShotMin;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
	float m_fDamagePerShotMax;

	/*
	Ammo Values
	*/
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	int m_iClipSize = 8;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	int m_iMaxAmmo = 80;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ammo")
	int m_iCurrentAmmo = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ammo")
	int m_iHolsteredAmmo = 0;

private:
	/** The Character holding this weapon*/
	AFpsCharacter* m_pCharacter;
};
