// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_Attack.h"
#include "Enemy_Controller.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EnemyKeys.h"

UBTT_Attack::UBTT_Attack(FObjectInitializer const& pObjectInit)
{
    NodeName = TEXT("Attack");
}

EBTNodeResult::Type UBTT_Attack::ExecuteTask(UBehaviorTreeComponent& pTreeComponent, uint8* pNodeMemory)
{
    // Get AI controller and enemy from pawn.
    AEnemy_Controller* pAiController = Cast<AEnemy_Controller>(pTreeComponent.GetAIOwner());
    AEnemy* pEnemy = Cast<AEnemy>(pAiController->GetPawn());

    // Check if attack montage is finished.
    if (AttackMontageFinished(pEnemy))
    {
        // Get bool value from key to check if we can attack or not.
        bool bCanAttack = pAiController->GetBlackboard()->GetValueAsBool(EnemyKeys::IsPlayerInRange);
        if (bCanAttack)
        {
            pEnemy->Attack();
        }
    }

    // Finish execution.
    FinishLatentTask(pTreeComponent, EBTNodeResult::Succeeded);
    return EBTNodeResult::Succeeded;
}

bool UBTT_Attack::AttackMontageFinished(AEnemy* pEnemy)
{
    UAnimInstance* pAnimInstance = pEnemy->GetMesh()->GetAnimInstance();
    if (pAnimInstance != nullptr)
    {
        return pAnimInstance->Montage_GetIsStopped(pEnemy->GetAttackMontage());
    }
    
    return false;
}