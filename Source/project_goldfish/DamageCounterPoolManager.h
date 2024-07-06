// A learning project by KDC64.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DamageCounterPoolManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCounterRequested, int, iDamage, FVector, vLocation);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCounterFinished, int, iActiveIndex);
UCLASS()
class PROJECT_GOLDFISH_API ADamageCounterPoolManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADamageCounterPoolManager();

	UFUNCTION(BlueprintCallable)
	void Setup();

	UFUNCTION(BlueprintCallable)
	void RequestCounter(int iDamage, FVector vSpawnLocation);

protected:
	UPROPERTY(BlueprintAssignable)
	FOnCounterRequested OnCounterRequested;

	UPROPERTY(BlueprintAssignable)
	FOnCounterFinished OnCounterFinished;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UUserWidget* m_pDamageCounterTemplate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int m_iMaxPoolAmount;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<UUserWidget*> m_pPooledCounters;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<UUserWidget*> m_pActiveCounters;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void ReturnToPool(int iActiveCounterIndex);
};
