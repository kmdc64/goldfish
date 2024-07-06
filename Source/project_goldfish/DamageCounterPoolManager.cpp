// A learning project by KDC64.


#include "DamageCounterPoolManager.h"

// Sets default values
ADamageCounterPoolManager::ADamageCounterPoolManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

void ADamageCounterPoolManager::Setup()
{
}

void ADamageCounterPoolManager::RequestCounter(int iDamage, FVector vSpawnLocation)
{
	if (m_pPooledCounters.IsEmpty())
	{
		// We'll recycle the oldest active counter.
		ReturnToPool(0);
	}

	OnCounterRequested.Broadcast(iDamage, vSpawnLocation);
}

// Called when the game starts or when spawned
void ADamageCounterPoolManager::BeginPlay()
{
	Super::BeginPlay();
	
}

void ADamageCounterPoolManager::ReturnToPool(int iActiveCounterIndex)
{
	UUserWidget* pActiveCounterToReturn = m_pActiveCounters[iActiveCounterIndex];
	m_pPooledCounters.Push(pActiveCounterToReturn);
	m_pActiveCounters.RemoveAt(iActiveCounterIndex);
}

