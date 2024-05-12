// Fill out your copyright notice in the Description page of Project Settings.

#include "BehaviorTree/BlackboardComponent.h"
#include "Runtime/NavigationSystem/Public/NavigationSystem.h"
#include "Enemy_Controller.h"
#include "EnemyKeys.h"
#include "Kismet/GameplayStatics.h"
#include "BTT_FindPlayerLocation.h"

UBTT_FindPlayerLocation::UBTT_FindPlayerLocation(FObjectInitializer const& a_pObjectInit)
{
    NodeName = TEXT("Find Player Location");
}

EBTNodeResult::Type UBTT_FindPlayerLocation::ExecuteTask(UBehaviorTreeComponent& a_pTreeComp, uint8* a_pNodeMem)
{
    // Get AI Controller.
    auto const pAIController = Cast<AEnemy_Controller>(a_pTreeComp.GetAIOwner());

    // Get Nav system.
    UNavigationSystemV1* pNavSystem = UNavigationSystemV1::GetCurrent(GetWorld());

    // Get player reference.
    APlayerController* pPlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    APawn* pPlayer = pPlayerController->GetPawn();

    if (pNavSystem != nullptr)
    {
        // If valid, set target location to player location.
        pAIController->GetBlackboard()->SetValueAsVector(EnemyKeys::targetLocation, pPlayer->GetActorLocation());
    }

    // Finish execution.
    FinishLatentTask(a_pTreeComp, EBTNodeResult::Succeeded);

    return EBTNodeResult::Succeeded;
}