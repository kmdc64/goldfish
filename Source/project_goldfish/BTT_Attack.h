// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "Enemy.h"
#include "BTT_Attack.generated.h"

/**
 * UBTT_Attack:
 * Attack behavior.
 */
UCLASS()
class PROJECT_GOLDFISH_API UBTT_Attack : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UBTT_Attack(FObjectInitializer const& pObjectInit);
	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& pTreeComponent, uint8* pNodeMemory);
	bool AttackMontageFinished(AEnemy* pEnemy);
};
