// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "TP_WeaponComponent.generated.h"

class Aproject_goldfishCharacter;

UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECT_GOLDFISH_API UTP_WeaponComponent : public USkeletalMeshComponent
{
	GENERATED_BODY()

public:
	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category=Projectile)
	TSubclassOf<class Aproject_goldfishProjectile> ProjectileClass;

	// Weapon sounds
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound", meta=(AllowPrivateAccess = "true"))
	USoundBase* m_pFireSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound", meta=(AllowPrivateAccess = "true"))
	USoundBase* m_pEmptySound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound", meta=(AllowPrivateAccess = "true"))
	TArray<USoundBase*> m_pImpactSounds;

	// Muzzle offset
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay, meta=(AllowPrivateAccess = "true"))
	FVector m_vMuzzleOffset = FVector(100.0f, 0.0f, 10.0f);

	// Muzzle flash
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FX", meta=(AllowPrivateAccess = "true"))
	UNiagaraSystem* m_pMuzzleFlash;
	
	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UAnimMontage* FireAnimation;

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

protected:
	/** Ends gameplay for this component. */
	UFUNCTION()
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	/** The Character holding this weapon*/
	Aproject_goldfishCharacter* Character;
};
