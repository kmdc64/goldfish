// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "TP_WeaponComponent.generated.h"

class Aproject_goldfishCharacter;

UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECT_GOLDFISH_API UTP_WeaponComponent : public USkeletalMeshComponent
{
	GENERATED_BODY()

public:
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

	/** Sets default values for this component's properties */
	UTP_WeaponComponent();

	/** Attaches the actor to a FirstPersonCharacter */
	UFUNCTION(BlueprintCallable, Category="Weapon")
	void AttachWeapon(Aproject_goldfishCharacter* TargetCharacter);

	/** Make the weapon Fire a Projectile */
	UFUNCTION(BlueprintCallable, Category="Weapon")
	void Fire();

	/** Fill the weapon with ammo from the available pool */
	UFUNCTION(BlueprintCallable, Category="Weapon")
	void Reload();

	/** Return true if there is available ammo to reload */
	UFUNCTION(BlueprintCallable, Category="Weapon")
	virtual bool CanReload();

	// Ammo values
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	int m_iClipSize = 8;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	int m_iMaxAmmo = 80;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int m_iCurrentAmmo = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int m_iTotalAmmo = 0;

	float GetShotDamage();

protected:
	/** Ends gameplay for this component. */
	UFUNCTION()
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
	float m_fDamagePerShotMin;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
	float m_fDamagePerShotMax;

private:
	/** The Character holding this weapon*/
	Aproject_goldfishCharacter* character;
};
