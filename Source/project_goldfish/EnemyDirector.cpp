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
	TArray<AActor*> pPooledEnemies = GetAllEnemiesInPool();
	TArray<AActor*> pArenaEnemies = GetAllEnemiesInArena();
	int iPooledEnemiesCount = pPooledEnemies.Num();
	int iArenaEnemiesCount = pArenaEnemies.Num();
	if (iPooledEnemiesCount == 0)
		return; // No enemies available to take from the pool.

	int iLeftToKill = ICurrentWaveSize - IWaveKills;
	int iLeftToSpawn = iLeftToKill - iArenaEnemiesCount;
	if (iLeftToSpawn <= 0)
		return;
	
	// Limit the amount of enemies we can spawn at once to our chosen limit, or the pool size.
	int iArenaCapacityLeft = IMaxEnemiesInArena - iArenaEnemiesCount;
	if (iArenaCapacityLeft <= 0)
		return;

	int iAmountSpawnable = UKismetMathLibrary::Min(iArenaCapacityLeft, iLeftToSpawn);
	iAmountSpawnable = UKismetMathLibrary::Min(iPooledEnemiesCount, iAmountSpawnable);
	for (int i = 0; i < iAmountSpawnable; ++i)
	{
		AEnemy* pEnemy = Cast<AEnemy>(pPooledEnemies[i]);

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
	int iTotalEnemyGrowth = IMaxEnemiesInWave - IInitialWaveSpawnCount;
	float fRateOfGrowth = (float)iTotalEnemyGrowth / IFinalGrowthWave;
	int iNewEnemyCount = (ICurrentWave - 1) * fRateOfGrowth;
	ICurrentWaveSize = UKismetMathLibrary::Min(IMaxEnemiesInWave, (IInitialWaveSpawnCount + iNewEnemyCount)); 
    return ICurrentWaveSize;
}

int AEnemyDirector::UpdateEnemyArenaCapacity()
{
    int iTotalCapacityGrowth = IMaxEnemyArenaCapacity - IMaxEnemiesInArena;
	float fRateOfGrowth = (float)iTotalCapacityGrowth / IWaveMaxEnemyArenaCapacityReached;
	int iNewCapacity = (ICurrentWave - 1) * fRateOfGrowth;
	IMaxEnemiesInArena = UKismetMathLibrary::Min(IMaxEnemyArenaCapacity, (IMaxEnemiesInArena + iNewCapacity));
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
	UWorld* pWorld = GetWorld();

	ClearCurrentTimer();
	pWorld->GetTimerManager().SetTimer(pTimerHandleCurrent, this, &AEnemyDirector::NextWaveDelayedCallback, FSecondsBeforeWaveStarts, false);
}

void AEnemyDirector::EndWave()
{
	// Delay next wave.
	m_bWaveIntermission = true;
	UWorld* pWorld = GetWorld();

	ClearCurrentTimer();
	pWorld->GetTimerManager().SetTimer(pTimerHandleCurrent, this, &AEnemyDirector::EndWaveDelayedCallback, FSecondsBeforeWaveEnds, false);
}

void AEnemyDirector::SpawnMoreEnemies()
{
	int iEnemiesInArenaCount = GetAllEnemiesInArena().Num();
	int iLeftToSpawn = (ICurrentWaveSize - IWaveKills) - iEnemiesInArenaCount;
	bool bArenaFull = iEnemiesInArenaCount == IMaxEnemiesInArena;
	if ((iLeftToSpawn > 0) && !bArenaFull)
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

void AEnemyDirector::EndWaveDelayedCallback()
{
	NextWave();
}

void AEnemyDirector::NextWaveDelayedCallback()
{
	ModifyWaveSpeeds();
	m_bWaveIntermission = false;
}

void AEnemyDirector::ClearCurrentTimer()
{
	UWorld* pWorld = GetWorld();
	if (pWorld == nullptr)
		return;
	
	pWorld->GetTimerManager().ClearTimer(pTimerHandleCurrent);
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

