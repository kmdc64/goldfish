// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "HealthInterface.h"
#include "Enemy.generated.h"

DECLARE_DYNAMIC_DELEGATE(FOnEnemyKilled);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnemyDamaged, float, fDamageTaken);

 
/**
 * AEnemy:
 * Manages an enemy Actor.
 */
UCLASS()
class PROJECT_GOLDFISH_API AEnemy : public ACharacter, public IHealthInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties.
	AEnemy();

	// Triggers when the enemy has died.
	FOnEnemyKilled OnEnemyKilled;

	// Triggers when the enemy takes damage or receives health.
	UPROPERTY(BlueprintAssignable)
	FOnEnemyDamaged OnEnemyDamaged;

	// Called every frame.
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input.
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// IHealthInterface: Deal damage to the enemy.
	virtual void ReceiveDamage(int amount) override;
	// IHealthInterface: Restore health.
	virtual void RecoverHealth(int amount) override;

	// Perform an attack.
	void Attack();

	/*
	Getter functions.
	*/
	// Return the enemy's attack montage.
	UAnimMontage* GetAttackMontage();
	// Return the enemy's attack range.
	float GetAttackRange();
	// Return the enemy's base speed stat.
	float GetBaseSpeed();

	// True if the enemy is active in the arena.
	bool BInArena = false;

protected:
	// Called when the game starts or when spawned.
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Animation")
	UAnimMontage* PAttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Animation")
	UAnimMontage* PDeathMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sound")
	TArray<USoundBase*> PAttackSounds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sound")
	TArray<USoundBase*> PDeathSounds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sound")
	USoundBase* PDamagedSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float FInitialHealth = 30.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float FAttackDamage = 33.4f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float FAttackRange = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float FBaseSpeed = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float IPointsPerHitTaken = 10;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float IPointsFromDeath = 150;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float FHealth = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<class UUserWidget> m_cDamageIndicator;

private:
	FVector m_vSpawnLocation;

	// Trigger the death sequence.
	void Die();
	// Reset the enemy and return to the pool awaiting reuse.
	void ReturnToPool();

	UFUNCTION()
	void HandleOnMontageEnded(UAnimMontage* pMontage, bool bWasInterrupted);
};
