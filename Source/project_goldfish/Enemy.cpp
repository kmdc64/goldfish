// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "Kismet/GameplayStatics.h"
#include "project_goldfishCharacter.h"


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

	m_fHealth = m_fInitialHealth;
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
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AEnemy::ReceiveDamage(int amount)
{
	if (m_fHealth <= 0)
		return; // Already dead or dying.

	m_fHealth -= amount;

	// Play reaction to damage sfx.
	UWorld* world = GetWorld();
	UGameplayStatics::PlaySoundAtLocation(world, m_pDamagedSound, GetActorLocation());

	if (m_fHealth <= 0)
	{
		Die();
	}
}

void AEnemy::RecoverHealth(int amount)
{
	m_fHealth += amount;
}

UAnimMontage* AEnemy::GetAttackMontage()
{
    return m_pAttackMontage;
}

float AEnemy::GetAttackRange()
{
    return m_fAttackRange;
}

float AEnemy::GetBaseSpeed()
{
	return m_fBaseSpeed;
}

void AEnemy::Attack()
{
	UWorld* world = GetWorld();

	// Play the attack animation.
	UAnimInstance* pAnimInstance = GetMesh()->GetAnimInstance();
	if (pAnimInstance != nullptr)
	{
		pAnimInstance->Montage_Play(m_pAttackMontage);
	}

	// Play an attack sfx.
	USoundBase* attackSound = m_pAttackSounds[FMath::RandRange(0, m_pAttackSounds.Num() - 1)];
	UGameplayStatics::PlaySoundAtLocation(world, attackSound, GetActorLocation());

	// Deal damage to the player.
	Aproject_goldfishCharacter* pPlayer = Cast<Aproject_goldfishCharacter>(UGameplayStatics::GetPlayerCharacter(world, 0));
	pPlayer->ReceiveDamage(m_fAttackDamage);
}

void AEnemy::Die()
{
	// Play the death animation.
	UAnimInstance* pAnimInstance = GetMesh()->GetAnimInstance();
	if (pAnimInstance != nullptr)
	{
		if (!pAnimInstance->Montage_IsPlaying(m_pDeathMontage))
		{
			pAnimInstance->Montage_Play(m_pDeathMontage);
		}
	}

	// Play a death sfx.
	UWorld* world = GetWorld();
	USoundBase* deathSound = m_pDeathSounds[FMath::RandRange(0, m_pDeathSounds.Num() - 1)];
	UGameplayStatics::PlaySoundAtLocation(world, deathSound, GetActorLocation());
}

void AEnemy::ReturnToPool()
{
	// Return enemy back to the pool.
	TeleportTo(m_vSpawnLocation, GetActorRotation());

	// Reset health and combat status.
	m_fHealth = m_fInitialHealth;
	InArena = false;

	// Execute then reset delegates.
	OnEnemyKilled.ExecuteIfBound();
	OnEnemyKilled.Clear();
}


void AEnemy::HandleOnMontageEnded(UAnimMontage* montage, bool wasInterrupted)
{
	if (montage->GetName().Contains("Death"))
	{
		ReturnToPool();
	}
}
