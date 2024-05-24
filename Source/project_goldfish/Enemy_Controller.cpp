// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy_Controller.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "UObject/ConstructorHelpers.h"

AEnemy_Controller::AEnemy_Controller(FObjectInitializer const& pObjectInit)
{
    // Get the behaviour tree from reference.
    static ConstructorHelpers::FObjectFinder<UBehaviorTree> treeFinder(TEXT("/Script/AIModule.BehaviorTree'/Game/Goldfish/AI/BT_Enemy.BT_Enemy'"));
    if (treeFinder.Succeeded())
    {
        PBehaviorTree = treeFinder.Object;
    }

    PBehaviorTreeComponent = pObjectInit.CreateDefaultSubobject<UBehaviorTreeComponent>(this, TEXT("BehaviorTree Component"));
    m_pBlackboard = pObjectInit.CreateDefaultSubobject<UBlackboardComponent>(this, TEXT("Blackboard Component"));
}

void AEnemy_Controller::BeginPlay()
{
    Super::BeginPlay();

    RunBehaviorTree(PBehaviorTree);
    PBehaviorTreeComponent->StartTree(*PBehaviorTree);
}

void AEnemy_Controller::OnPossess(APawn* a_pPawn)
{
    Super::OnPossess(a_pPawn);

    if (m_pBlackboard)
    {
        m_pBlackboard->InitializeBlackboard(*PBehaviorTree->BlackboardAsset);
    }
}

UBlackboardComponent* AEnemy_Controller::GetBlackboard() const
{
    return m_pBlackboard;
}