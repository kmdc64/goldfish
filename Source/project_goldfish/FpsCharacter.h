// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "TP_WeaponComponent.h"
#include "Weapon.h"
#include "HealthInterface.h"
#include "PlayerStats.h"
#include "FpsCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPlayerHealthChanged, int, iCurrentHealth, int, iMaxHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponChanged, FString, sWeaponName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAmmoChanged, int, iCurrentAmmo, int, iMaxAmmo);

UCLASS(config=Game)
class AFpsCharacter : public ACharacter, public IHealthInterface
{
	GENERATED_BODY()
	
public:
	AFpsCharacter();

	// Event delegate functions

	UPROPERTY(BlueprintAssignable)
	FOnPlayerHealthChanged OnPlayerHealthChanged;

	UPROPERTY(BlueprintAssignable)
	FOnWeaponChanged OnWeaponChanged;

	UPROPERTY(BlueprintAssignable)
	FOnAmmoChanged OnAmmoChanged;

	// End of event delegate functions

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	USkeletalMeshComponent* Mesh1P;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Pause Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* PauseAction;

	// TODO: Remove once pause logic is in place.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "To Delete")
	UWorld* PauseMap;

	// Stat tracker.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	APlayerStats* Stats;

	// Reload montage
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* PReloadMontage;

	// Shooting montage
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* PShootMontage;

	// Reference to the equipped weapon
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	AWeapon* PCurrentWeapon = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UTP_WeaponComponent* PCurrentWeaponComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float FHealthMax = 100.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health")
	float FHealth = 100.0f;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	/** Shoot Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ShootAction;

	/** Reload Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ReloadAction;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Health)
	bool m_bRegenAllowed = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Health)
	float m_fHealthRegenPerSecond = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Health)
	float m_fSecondsTillRegen = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
	int m_iStartingPoints = 500;

	// The weapon the player will spawn into the level with.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta=(AllowPrivateAccess = "true"))
	TSubclassOf<class AActor> m_cStartingWeapon;

	// The player's HUD.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD", meta=(AllowPrivateAccess = "true"))
	TSubclassOf<class UUserWidget> m_cPlayerHud;

	float m_fSecondsSinceLastDamaged = 0.0f;

public:
	UFUNCTION()
	void HandleOnMontageEnd(UAnimMontage* a_pMontage, bool a_bInterrupted);

	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }

	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	void Shoot();
	void Reload();
	void EquipWeapon(TSubclassOf<class AActor> cWeapon);

	/** IHealthInterface methods. **/
	virtual void ReceiveDamage(int amount) override;
	virtual void RecoverHealth(int amount) override;

	// Fire OnAmmoChanged event when relevant.
	void AmmoChanged();

protected:
	virtual void Tick(float fDeltaTime) override;

	virtual void BeginPlay();

	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

	/** Called for movement input. */
	void Move(const FInputActionValue& Value);

	/** Called for looking input. */
	void Look(const FInputActionValue& Value);

	/** Called for Pausing input. */
	void Pause(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void RefreshUI();

	// Heal to max over time, or wait until ready to regen.
	void UpdateHealthRegen(float fDeltaTime);
};

