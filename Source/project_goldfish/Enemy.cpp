// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "Kismet/GameplayStatics.h"
#include "FpsCharacter.h"


// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	FHealth = FInitialHealth;
	m_vSpawnLocation = GetActorLocation();
	
	// Bind events.
	GetMesh()->GetAnimInstance()->OnMontageEnded.AddDynamic(this, &AEnemy::HandleOnMontageEnded);
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* pPlayerInputComponent)
{
	Super::SetupPlayerInputComponent(pPlayerInputComponent);
}

void AEnemy::ReceiveDamage(int iAmount)
{
	if (FHealth <= 0)
		return; // Already dead or dying.

	FHealth -= iAmount;
	OnEnemyDamaged.Broadcast((float)iAmount);

	// Play reaction to damage sfx.
	UWorld* pWorld = GetWorld();
	UGameplayStatics::PlaySoundAtLocation(pWorld, PDamagedSound, GetActorLocation());

	// Reward points for damaging an enemy.
	AFpsCharacter* pPlayerCharacter = Cast<AFpsCharacter>(pWorld->GetFirstPlayerController()->GetCharacter());
	pPlayerCharacter->Stats->AddPoints(IPointsPerHitTaken);

	if (FHealth <= 0)
	{
		Die();
	}
}

void AEnemy::RecoverHealth(int iAmount)
{
	FHealth += iAmount;
}

UAnimMontage* AEnemy::GetAttackMontage()
{
    return PAttackMontage;
}

float AEnemy::GetAttackRange()
{
    return FAttackRange;
}

float AEnemy::GetBaseSpeed()
{
	return FBaseSpeed;
}

void AEnemy::Attack()
{
	UWorld* pWorld = GetWorld();

	// Play the attack animation.
	UAnimInstance* pAnimInstance = GetMesh()->GetAnimInstance();
	if (pAnimInstance != nullptr)
	{
		if (pAnimInstance->Montage_IsPlaying(PDeathMontage))
			return; // Early out if the enemy is in the middle of dying.

		pAnimInstance->Montage_Play(PAttackMontage);
	}

	// Play an attack sfx.
	USoundBase* pAttackSound = PAttackSounds[FMath::RandRange(0, PAttackSounds.Num() - 1)];
	UGameplayStatics::PlaySoundAtLocation(pWorld, pAttackSound, GetActorLocation());

	// Deal damage to the player.
	AFpsCharacter* pPlayer = Cast<AFpsCharacter>(UGameplayStatics::GetPlayerCharacter(pWorld, 0));
	pPlayer->ReceiveDamage(FAttackDamage);
}

void AEnemy::Die()
{
	// Play the death animation.
	UAnimInstance* pAnimInstance = GetMesh()->GetAnimInstance();
	if (pAnimInstance != nullptr)
	{
		if (!pAnimInstance->Montage_IsPlaying(PDeathMontage))
		{
			pAnimInstance->Montage_Play(PDeathMontage);
		}
	}

	// Play a death sfx.
	UWorld* pWorld = GetWorld();
	USoundBase* pDeathSound = PDeathSounds[FMath::RandRange(0, PDeathSounds.Num() - 1)];
	UGameplayStatics::PlaySoundAtLocation(pWorld, pDeathSound, GetActorLocation());

	// Reward points for killing an enemy.
	AFpsCharacter* pPlayerCharacter = Cast<AFpsCharacter>(GetWorld()->GetFirstPlayerController()->GetCharacter());
	pPlayerCharacter->Stats->AddPoints(IPointsFromDeath);
}

void AEnemy::ReturnToPool()
{
	// Execute then reset delegates.
	OnEnemyKilled.ExecuteIfBound();
	OnEnemyKilled.Clear();

	// Return enemy back to the pool.
	TeleportTo(m_vSpawnLocation, GetActorRotation());

	// Reset health and combat status.
	FHealth = FInitialHealth;
	BInArena = false;
}


void AEnemy::HandleOnMontageEnded(UAnimMontage* pMontage, bool bWasInterrupted)
{
	if (pMontage->GetName().Contains("Death"))
	{
		ReturnToPool();
	}
}
