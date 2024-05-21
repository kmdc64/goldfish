// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "HealthInterface.h"
#include "Enemy.generated.h"

UCLASS()
class PROJECT_GOLDFISH_API AEnemy : public ACharacter, public IHealthInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();

	DECLARE_DYNAMIC_DELEGATE(FOnEnemyKilled);

	FOnEnemyKilled OnEnemyKilled;

	bool InArena = false;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void Attack();

	// IHealthInterface methods.
	virtual void ReceiveDamage(int amount) override;
	virtual void RecoverHealth(int amount) override;

	UAnimMontage* GetAttackMontage();
	float GetAttackRange();
	float GetBaseSpeed();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Animation")
	UAnimMontage* m_pAttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Animation")
	UAnimMontage* m_pDeathMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sound")
	TArray<USoundBase*> m_pAttackSounds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sound")
	TArray<USoundBase*> m_pDeathSounds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sound")
	USoundBase* m_pDamagedSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float m_fInitialHealth = 30.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float m_fAttackDamage = 33.4f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float m_fAttackRange = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float m_fBaseSpeed = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float m_fHealth = 0.0f;

private:
	FVector m_vSpawnLocation;

	void Die();
	void ReturnToPool();

	UFUNCTION()
	void HandleOnMontageEnded(UAnimMontage* montage, bool wasInterrupted);
};
