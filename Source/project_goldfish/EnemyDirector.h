// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyDirector.generated.h"

UCLASS()
class PROJECT_GOLDFISH_API AEnemyDirector : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemyDirector();

	TArray<AActor*> m_pEnemies;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Management")
	TArray<FVector> m_pSpawnLocations;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Points")
	int m_iPointsPerStandardKill = 150;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Waves")
	int m_iInitialWaveSpawnCount = 5;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Waves")
	int m_iCurrentWave = 0;

	// The last wave in which enemy count will grow.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Waves")
	int m_iFinalGrowthWave = 50;

	// The maximum amount of enemies a wave can have.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Waves")
	int m_iMaxEnemiesInWave = 666;

	// The final amount of enemies that can spawn into the arena at one time.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Waves")
	int m_iMaxEnemyArenaCapacity = 50;

	// The wave at which maximum enemy arena capacity is reached.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Waves")
	int m_iWaveMaxEnemyArenaCapacityReached = 22;

	// The current maximum amount of enemies that can spawn into the arena at one time.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Waves")
	int m_iMaxEnemiesInArena = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Waves")
	float m_fSecondsBeforeWaveStarts = 4.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Waves")
	float m_fSecondsBeforeWaveEnds = 4.0f;

	int m_iCurrentWaveSize;
	int m_iWaveKills = 0;

	void AttemptSpawnEnemies();
	int UpdateWaveSize();
	int UpdateEnemyArenaCapacity();
	void UpdateWaveParameters();
	void NextWave();
	void EndWave();
	void SpawnMoreEnemies();
	TArray<AActor*> GetAllEnemiesInArena();
	TArray<AActor*> GetAllEnemiesInPool();
	void ModifyWaveSpeeds();

	UFUNCTION()
	void ConfirmEnemyKilled();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// The final maximum speed an enemy can move at.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Speed")
	float m_fGlobalFinalMaxWalkSpeed = 400.0f;

	// The final minimum speed an enemy can move at.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Speed")
	float m_fGlobalFinalMinWalkSpeed = 200.0f;

	// The current maximum speed an enemy can move at (ignoring base speed modifiers).
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Speed")
	float m_fGlobalMaxWalkSpeed = 120.0f;

	// The current minimum speed an enemy can move at (ignoring base speed modifiers).
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Speed")
	float m_fGlobalMinWalkSpeed = 70.0f;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
};
