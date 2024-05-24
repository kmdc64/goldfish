// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "IsPlayerInRange.generated.h"

/**
 * UIsPlayerInRange:
 * Behavior for checkign if the player (B) is in range of Enemy (A).
 */
UCLASS()
class PROJECT_GOLDFISH_API UIsPlayerInRange : public UBTService_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UIsPlayerInRange();

	// Triggered when service is called.
	void OnBecomeRelevant(UBehaviorTreeComponent& behaviorTreeComponent, uint8* nodeMemory);
};
