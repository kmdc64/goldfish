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

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Management")
	TArray<FVector> PSpawnLocations;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Waves")
	int IInitialWaveSpawnCount = 5;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Waves")
	int ICurrentWave = 0;

	// The last wave in which enemy count will grow.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Waves")
	int IFinalGrowthWave = 50;

	// The maximum amount of enemies a wave can have.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Waves")
	int IMaxEnemiesInWave = 666;

	// The final amount of enemies that can spawn into the arena at one time.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Waves")
	int IMaxEnemyArenaCapacity = 50;

	// The wave at which maximum enemy arena capacity is reached.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Waves")
	int IWaveMaxEnemyArenaCapacityReached = 22;

	// The current maximum amount of enemies that can spawn into the arena at one time.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Waves")
	int IMaxEnemiesInArena = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Waves")
	float FSecondsBeforeWaveStarts = 4.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Waves")
	float FSecondsBeforeWaveEnds = 4.0f;

	UFUNCTION()
	void ConfirmEnemyKilled();

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

	int ICurrentWaveSize;
	int IWaveKills = 0;
	TArray<AActor*> PEnemies;

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
	
private:
	bool m_bWaveIntermission;
};
