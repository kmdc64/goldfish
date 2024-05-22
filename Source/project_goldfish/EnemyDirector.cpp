// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyDirector.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Enemy.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "project_goldfishCharacter.h"

// Sets default values
AEnemyDirector::AEnemyDirector()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void AEnemyDirector::AttemptSpawnEnemies()
{
	TArray<AActor*> pooledEnemies = GetAllEnemiesInPool();
	int pooledEnemiesCount = pooledEnemies.Num();
	if (pooledEnemiesCount > 0)
	{
		int enemiesInArenaCount = GetAllEnemiesInArena().Num();
		int leftToSpawn = (m_iCurrentWaveSize - m_iWaveKills) - enemiesInArenaCount;
		if (leftToSpawn <= 0)
			return;
		
		// Limit the amount of enemies we can spawn at once to our chosen limit, or the pool size.
		int arenaCapacityLeft = m_iMaxEnemiesInArena - enemiesInArenaCount;
		if (arenaCapacityLeft <= 0)
			return;

		int amountSpawnable = UKismetMathLibrary::Min(arenaCapacityLeft, leftToSpawn);
		amountSpawnable = UKismetMathLibrary::Min(pooledEnemiesCount, amountSpawnable);
		for (int i = 0; i < amountSpawnable; ++i)
		{
			AEnemy* pEnemy = Cast<AEnemy>(pooledEnemies[i]);

			// Bind delegates.
			pEnemy->OnEnemyKilled.BindUFunction(this, "ConfirmEnemyKilled");

			// Disable collisions temporarily in case enemies spawn on top of one another.
			pEnemy->SetActorEnableCollision(false);
			auto spawnLocation = m_pSpawnLocations[FMath::RandRange(0, m_pSpawnLocations.Num() - 1)];
			auto enemyRotation = pEnemy->GetActorRotation();
			pEnemy->TeleportTo(spawnLocation, enemyRotation);
			pEnemy->InArena = true;
			pEnemy->SetActorEnableCollision(true);
		}
	}
}

int AEnemyDirector::UpdateWaveSize()
{
	int totalEnemyGrowth = m_iMaxEnemiesInWave - m_iInitialWaveSpawnCount;
	float rateOfGrowth = (float)totalEnemyGrowth / m_iFinalGrowthWave;
	int newEnemyCount = (m_iCurrentWave - 1) * rateOfGrowth;
	m_iCurrentWaveSize = UKismetMathLibrary::Min(m_iMaxEnemiesInWave, (m_iInitialWaveSpawnCount + newEnemyCount)); 
    return m_iCurrentWaveSize;
}

int AEnemyDirector::UpdateEnemyArenaCapacity()
{
    int totalCapacityGrowth = m_iMaxEnemyArenaCapacity - m_iMaxEnemiesInArena;
	float rateOfGrowth = (float)totalCapacityGrowth / m_iWaveMaxEnemyArenaCapacityReached;
	int newCapacity = (m_iCurrentWave - 1) * rateOfGrowth;
	m_iMaxEnemiesInArena = UKismetMathLibrary::Min(m_iMaxEnemyArenaCapacity, (m_iMaxEnemiesInArena + newCapacity));
	return m_iMaxEnemiesInArena;
}

void AEnemyDirector::UpdateWaveParameters()
{
	// Reset and move to the next wave.
	m_iCurrentWave++;
	m_iWaveKills = 0;
	UpdateWaveSize();
	UpdateEnemyArenaCapacity();

	// Update the enemy speeds.
	m_fGlobalMaxWalkSpeed = FMath::Clamp(m_fGlobalMaxWalkSpeed + 50.0f, 0, m_fGlobalFinalMaxWalkSpeed);
	m_fGlobalMinWalkSpeed = FMath::Clamp(m_fGlobalMinWalkSpeed + 15.0f, 0, m_fGlobalFinalMinWalkSpeed);
}

void AEnemyDirector::NextWave()
{
	UpdateWaveParameters();
	
	// Delay spawning of enemies.
	FTimerHandle pTimerHandle;
	UWorld* world = GetWorld();
	world->GetTimerManager().SetTimer(pTimerHandle, [&]()
	{
		AttemptSpawnEnemies();
		ModifyWaveSpeeds();
	}, m_fSecondsBeforeWaveStarts, false);
}

void AEnemyDirector::EndWave()
{
	// Delay next wave.
	FTimerHandle pTimerHandle;
	UWorld* world = GetWorld();
	world->GetTimerManager().SetTimer(pTimerHandle, [&]()
	{
		NextWave();
	}, m_fSecondsBeforeWaveEnds, false);
}

void AEnemyDirector::SpawnMoreEnemies()
{
	int enemiesInArenaCount = GetAllEnemiesInArena().Num();
	int leftToSpawn = (m_iCurrentWaveSize - m_iWaveKills) - enemiesInArenaCount;
	bool arenaFull = enemiesInArenaCount == m_iMaxEnemiesInArena;
	if ((leftToSpawn > 0) && !arenaFull)
	{
		AttemptSpawnEnemies();
	}
}

TArray<AActor*> AEnemyDirector::GetAllEnemiesInArena()
{
	TArray<AActor*> pEnemiesInArena = TArray<AActor*>();
	for (AActor* actor : m_pEnemies)
	{
		AEnemy* pEnemy = Cast<AEnemy>(actor);
		if (pEnemy->InArena)
		{
			pEnemiesInArena.Add(actor);
		}
	}

    return pEnemiesInArena;
}

TArray<AActor*> AEnemyDirector::GetAllEnemiesInPool()
{
    TArray<AActor*> pEnemiesInPool = TArray<AActor*>();
	for (AActor* actor : m_pEnemies)
	{
		AEnemy* pEnemy = Cast<AEnemy>(actor);
		if (!pEnemy->InArena)
		{
			pEnemiesInPool.Add(actor);
		}
	}

    return pEnemiesInPool;
}

void AEnemyDirector::ModifyWaveSpeeds()
{
	for (int i = 0; i < m_pEnemies.Num(); ++i)
	{
		AEnemy* pEnemy = Cast<AEnemy>(m_pEnemies[i]);
		float maxWalkSpeed = FMath::RandRange(m_fGlobalMinWalkSpeed, m_fGlobalMaxWalkSpeed);
		float finalSpeed = maxWalkSpeed + pEnemy->GetBaseSpeed();
		pEnemy->GetCharacterMovement()->MaxWalkSpeed = finalSpeed;
	}
}

void AEnemyDirector::ConfirmEnemyKilled()
{
	m_iWaveKills++;

	if (m_iWaveKills >= m_iCurrentWaveSize)
	{
		EndWave();
	}
	else
	{
		AttemptSpawnEnemies();
	}
}

// Called when the game starts or when spawned
void AEnemyDirector::BeginPlay()
{
	Super::BeginPlay();
	
	// Get all instances of enemy actors and store in the enemy list.
	auto world = GetWorld();
	UGameplayStatics::GetAllActorsOfClass(world, AEnemy::StaticClass(), m_pEnemies);

	NextWave();
}

// Called every frame
void AEnemyDirector::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

