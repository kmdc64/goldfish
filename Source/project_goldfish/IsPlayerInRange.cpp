// Fill out your copyright notice in the Description page of Project Settings.


#include "IsPlayerInRange.h"
#include "Kismet/GameplayStatics.h"
#include "Enemy_Controller.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EnemyKeys.h"
#include "Enemy.h"

UIsPlayerInRange::UIsPlayerInRange()
{
    bNotifyBecomeRelevant = true;
    NodeName = TEXT("Is Player In Range");
}

void UIsPlayerInRange::OnBecomeRelevant(UBehaviorTreeComponent& behaviorTreeComponent, uint8* nodeMemory)
{
    Super::OnBecomeRelevant(behaviorTreeComponent, nodeMemory);

    // Get AI controller & Pawn.
    AEnemy_Controller* pAiController = Cast<AEnemy_Controller>(behaviorTreeComponent.GetAIOwner());
    APawn* pEnemyPawn = pAiController->GetPawn();
    AEnemy* pEnemy = Cast<AEnemy>(pEnemyPawn);

    // Get Player Controller & Pawn.
    APlayerController* pPlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    AActor* pPlayerPawn = pPlayerController->GetPawn();

    // Set blackboard key to player in range.
    float fDistanceBetweenPawns = pEnemyPawn->GetDistanceTo(pPlayerPawn);
    bool bWithinRange = fDistanceBetweenPawns <= pEnemy->GetAttackRange();
    pAiController->GetBlackboard()->SetValueAsBool(EnemyKeys::IsPlayerInRange, bWithinRange);
}
