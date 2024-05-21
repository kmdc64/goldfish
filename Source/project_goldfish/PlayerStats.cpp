// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerStats.h"

APlayerStats::APlayerStats()
{
    PrimaryActorTick.bCanEverTick = false;
}

int APlayerStats::GetPoints()
{
    return m_iPoints;
}

int APlayerStats::AddPoints(int amount)
{
    m_iPoints = FMath::Clamp(m_iPoints + amount, 0, MaximumPoints);
    return m_iPoints;
}

int APlayerStats::RemovePoints(int amount)
{
    m_iPoints = FMath::Clamp(m_iPoints - amount, 0, MaximumPoints);
    return m_iPoints;
}

void APlayerStats::BeginPlay()
{
    // Call the base class  
	Super::BeginPlay();
}

void APlayerStats::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}
