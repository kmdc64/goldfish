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

UCLASS(config=Game)
class AFpsCharacter : public ACharacter, public IHealthInterface
{
	GENERATED_BODY()
	
public:
	AFpsCharacter();

	// Event delegate functions
	UPROPERTY(BlueprintAssignable)
	FOnPlayerHealthChanged OnPlayerHealthChanged;

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

	// Stat tracker.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	APlayerStats* Stats;

	// Reload montage
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* m_pReloadMontage;

	// Shooting montage
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* m_pShootMontage;

	// Reference to the equipped weapon
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	AWeapon* m_currentWeapon = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UTP_WeaponComponent* m_pCurrentlyEquippedWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float m_fHealthMax = 100.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float m_fHealth = 100.0f;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	/** Shoot Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ShootAction;

	/** Reload Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ReloadAction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon)
	bool bHasRifle;
	

protected:
	virtual void BeginPlay();

	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

public:
	UFUNCTION()
	void HandleOnMontageEnd(UAnimMontage* a_pMontage, bool a_bInterrupted);

	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	/** Setter to set the bool */
	UFUNCTION(BlueprintCallable, Category = Weapon)
	void SetHasRifle(bool bNewHasRifle);

	/** Getter for the bool */
	UFUNCTION(BlueprintCallable, Category = Weapon)
	bool GetHasRifle();

	void Shoot();
	void Reload();
	void EquipWeapon();

	/** IHealthInterface methods. **/
	virtual void ReceiveDamage(int amount) override;
	virtual void RecoverHealth(int amount) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
	int m_iStartingPoints = 500;

	// The weapon the player will spawn into the level with.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta=(AllowPrivateAccess = "true"))
	TSubclassOf<class AActor> m_cStartingWeapon;

	// The player's HUD.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD", meta=(AllowPrivateAccess = "true"))
	TSubclassOf<class UUserWidget> m_cPlayerHud;
};

