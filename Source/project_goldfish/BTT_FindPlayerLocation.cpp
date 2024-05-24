// Fill out your copyright notice in the Description page of Project Settings.

#include "BTT_FindPlayerLocation.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Runtime/NavigationSystem/Public/NavigationSystem.h"
#include "Enemy_Controller.h"
#include "EnemyKeys.h"
#include "Kismet/GameplayStatics.h"

UBTT_FindPlayerLocation::UBTT_FindPlayerLocation(FObjectInitializer const& a_pObjectInit)
{
    NodeName = TEXT("Find Player Location");
}

EBTNodeResult::Type UBTT_FindPlayerLocation::ExecuteTask(UBehaviorTreeComponent& pTreeComp, uint8* pNodeMem)
{
    UWorld* pWorld = GetWorld();
    AEnemy_Controller* const pAIController = Cast<AEnemy_Controller>(pTreeComp.GetAIOwner());
    UNavigationSystemV1* pNavSystem = UNavigationSystemV1::GetCurrent(pWorld);

    APlayerController* pPlayerController = UGameplayStatics::GetPlayerController(pWorld, 0);
    APawn* pPlayer = pPlayerController->GetPawn();

    if (pNavSystem != nullptr)
    {
        // If valid, set target location to player location.
        pAIController->GetBlackboard()->SetValueAsVector(EnemyKeys::TargetLocation, pPlayer->GetActorLocation());
    }

    // Finish execution.
    FinishLatentTask(pTreeComp, EBTNodeResult::Succeeded);

    return EBTNodeResult::Succeeded;
}