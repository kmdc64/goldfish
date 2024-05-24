// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTT_FindPlayerLocation.generated.h"

/**
 * UBTT_FindPlayerLocation:
 * Behavior for finding the player location and setting it as a target value.
 */
UCLASS()
class PROJECT_GOLDFISH_API UBTT_FindPlayerLocation : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UBTT_FindPlayerLocation(FObjectInitializer const& a_pObjectInit);
	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& pTreeComp, uint8* pNodeMem);
};
