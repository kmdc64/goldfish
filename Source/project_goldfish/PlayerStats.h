// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlayerStats.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPointsChanged, int, iPoints);

/**
 * APlayerStats:
 * Manages a player's in-game stats.
 */
UCLASS()
class PROJECT_GOLDFISH_API APlayerStats : public AActor
{
	GENERATED_BODY()

public:
	APlayerStats();

	// Event delegate functions
	UPROPERTY(BLueprintAssignable)
	FOnPointsChanged OnPointsChanged;

	const int MaximumPoints = 999999;

	// Returns the total points.
	UFUNCTION(BlueprintCallable, Category="Points")
	int GetPoints();

	// Adds an amount to the points and returns the new total.
	UFUNCTION(BlueprintCallable, Category="Points")
	int AddPoints(int amount);

	// Removes an amount of points and returns the new total.
	UFUNCTION(BlueprintCallable, Category="Points")
	int RemovePoints(int amount);

private:
	int m_iPoints = 0;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
