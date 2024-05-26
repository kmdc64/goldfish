// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyDirector.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Enemy.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "FpsCharacter.h"

// Sets default values
AEnemyDirector::AEnemyDirector()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void AEnemyDirector::AttemptSpawnEnemies()
{
	TArray<AActor*> pooledEnemies = GetAllEnemiesInPool();
	TArray<AActor*> arenaEnemies = GetAllEnemiesInArena();
	int pooledEnemiesCount = pooledEnemies.Num();
	int arenaEnemiesCount = arenaEnemies.Num();
	if (pooledEnemiesCount == 0)
		return; // No enemies available to take from the pool.

	int leftToKill = ICurrentWaveSize - IWaveKills;
	int leftToSpawn = leftToKill - arenaEnemiesCount;
	if (leftToSpawn <= 0)
		return;
	
	// Limit the amount of enemies we can spawn at once to our chosen limit, or the pool size.
	int arenaCapacityLeft = IMaxEnemiesInArena - arenaEnemiesCount;
	if (arenaCapacityLeft <= 0)
		return;

	int amountSpawnable = UKismetMathLibrary::Min(arenaCapacityLeft, leftToSpawn);
	amountSpawnable = UKismetMathLibrary::Min(pooledEnemiesCount, amountSpawnable);
	for (int i = 0; i < amountSpawnable; ++i)
	{
		AEnemy* pEnemy = Cast<AEnemy>(pooledEnemies[i]);

		// Bind delegates.
		pEnemy->OnEnemyKilled.Clear();
		pEnemy->OnEnemyKilled.BindUFunction(this, "ConfirmEnemyKilled");

		// Disable collisions temporarily in case enemies spawn on top of one another.
		pEnemy->SetActorEnableCollision(false);
		auto spawnLocation = PSpawnLocations[FMath::RandRange(0, PSpawnLocations.Num() - 1)];
		auto enemyRotation = pEnemy->GetActorRotation();
		pEnemy->TeleportTo(spawnLocation, enemyRotation);
		pEnemy->BInArena = true;
		pEnemy->SetActorEnableCollision(true);
	}
}

int AEnemyDirector::UpdateWaveSize()
{
	int totalEnemyGrowth = IMaxEnemiesInWave - IInitialWaveSpawnCount;
	float rateOfGrowth = (float)totalEnemyGrowth / IFinalGrowthWave;
	int newEnemyCount = (ICurrentWave - 1) * rateOfGrowth;
	ICurrentWaveSize = UKismetMathLibrary::Min(IMaxEnemiesInWave, (IInitialWaveSpawnCount + newEnemyCount)); 
    return ICurrentWaveSize;
}

int AEnemyDirector::UpdateEnemyArenaCapacity()
{
    int totalCapacityGrowth = IMaxEnemyArenaCapacity - IMaxEnemiesInArena;
	float rateOfGrowth = (float)totalCapacityGrowth / IWaveMaxEnemyArenaCapacityReached;
	int newCapacity = (ICurrentWave - 1) * rateOfGrowth;
	IMaxEnemiesInArena = UKismetMathLibrary::Min(IMaxEnemyArenaCapacity, (IMaxEnemiesInArena + newCapacity));
	return IMaxEnemiesInArena;
}

void AEnemyDirector::UpdateWaveParameters()
{
	// Reset and move to the next wave.
	ICurrentWave++;
	IWaveKills = 0;
	UpdateWaveSize();
	UpdateEnemyArenaCapacity();

	// Update the enemy speeds.
	m_fGlobalMaxWalkSpeed = FMath::Clamp(m_fGlobalMaxWalkSpeed + 50.0f, 0, m_fGlobalFinalMaxWalkSpeed);
	m_fGlobalMinWalkSpeed = FMath::Clamp(m_fGlobalMinWalkSpeed + 15.0f, 0, m_fGlobalFinalMinWalkSpeed);
}

void AEnemyDirector::NextWave()
{
	UpdateWaveParameters();

	OnWaveChanged.Broadcast(ICurrentWave);
	
	// Delay spawning of enemies.
	FTimerHandle pTimerHandle;
	UWorld* world = GetWorld();
	world->GetTimerManager().SetTimer(pTimerHandle, [&]()
	{
		ModifyWaveSpeeds();
		m_bWaveIntermission = false;
	}, FSecondsBeforeWaveStarts, false);
}

void AEnemyDirector::EndWave()
{
	// Delay next wave.
	m_bWaveIntermission = true;
	FTimerHandle pTimerHandle;
	UWorld* world = GetWorld();
	world->GetTimerManager().SetTimer(pTimerHandle, [&]()
	{
		NextWave();
	}, FSecondsBeforeWaveEnds, false);
}

void AEnemyDirector::SpawnMoreEnemies()
{
	int enemiesInArenaCount = GetAllEnemiesInArena().Num();
	int leftToSpawn = (ICurrentWaveSize - IWaveKills) - enemiesInArenaCount;
	bool arenaFull = enemiesInArenaCount == IMaxEnemiesInArena;
	if ((leftToSpawn > 0) && !arenaFull)
	{
		AttemptSpawnEnemies();
	}
}

TArray<AActor*> AEnemyDirector::GetAllEnemiesInArena()
{
	TArray<AActor*> pEnemiesInArena = TArray<AActor*>();
	for (AActor* actor : PEnemies)
	{
		AEnemy* pEnemy = Cast<AEnemy>(actor);
		if (pEnemy->BInArena)
		{
			pEnemiesInArena.Add(actor);
		}
	}

    return pEnemiesInArena;
}

TArray<AActor*> AEnemyDirector::GetAllEnemiesInPool()
{
    TArray<AActor*> pEnemiesInPool = TArray<AActor*>();
	for (AActor* actor : PEnemies)
	{
		AEnemy* pEnemy = Cast<AEnemy>(actor);
		if (!pEnemy->BInArena)
		{
			pEnemiesInPool.Add(actor);
		}
	}

    return pEnemiesInPool;
}

void AEnemyDirector::ModifyWaveSpeeds()
{
	for (int i = 0; i < PEnemies.Num(); ++i)
	{
		AEnemy* pEnemy = Cast<AEnemy>(PEnemies[i]);
		float maxWalkSpeed = FMath::RandRange(m_fGlobalMinWalkSpeed, m_fGlobalMaxWalkSpeed);
		float finalSpeed = maxWalkSpeed + pEnemy->GetBaseSpeed();
		pEnemy->GetCharacterMovement()->MaxWalkSpeed = finalSpeed;
	}
}

void AEnemyDirector::ConfirmEnemyKilled()
{
	IWaveKills++;

	if (IWaveKills >= ICurrentWaveSize)
	{
		EndWave();
	}
}

void AEnemyDirector::RefreshUI()
{
	OnWaveChanged.Broadcast(ICurrentWave);
}

// Called when the game starts or when spawned
void AEnemyDirector::BeginPlay()
{
	Super::BeginPlay();
	
	// Get all instances of enemy actors and store in the enemy list.
	auto world = GetWorld();
	UGameplayStatics::GetAllActorsOfClass(world, AEnemy::StaticClass(), PEnemies);

	NextWave();
}

// Called every frame
void AEnemyDirector::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!m_bWaveIntermission)
	{
		AttemptSpawnEnemies();
	}
}

