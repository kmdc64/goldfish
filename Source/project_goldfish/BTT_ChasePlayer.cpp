// Fill out your copyright notice in the Description page of Project Settings.

#include "BTT_ChasePlayer.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Runtime/NavigationSystem/Public/NavigationSystem.h"
#include "Enemy_Controller.h"
#include "EnemyKeys.h"
#include "Kismet/GameplayStatics.h"

UBTT_ChasePlayer::UBTT_ChasePlayer(FObjectInitializer const& a_pObjectInit)
{
    NodeName = TEXT("Chase Player");
}

EBTNodeResult::Type UBTT_ChasePlayer::ExecuteTask(UBehaviorTreeComponent& pTreeComp, uint8* pNodeMem)
{
    AEnemy_Controller* const pAIController = Cast<AEnemy_Controller>(pTreeComp.GetAIOwner());
    UNavigationSystemV1* pNavSystem = UNavigationSystemV1::GetCurrent(GetWorld());

    if (pNavSystem != nullptr)
    {
        // Get location of the player and tell the AI to move towards it.
        FVector pLocation  = pAIController->GetBlackboard()->GetValueAsVector(EnemyKeys::TargetLocation);
        pAIController->MoveToLocation(pLocation);
    }

    // Finish execution.
    FinishLatentTask(pTreeComp, EBTNodeResult::Succeeded);

    return EBTNodeResult::Succeeded;
}