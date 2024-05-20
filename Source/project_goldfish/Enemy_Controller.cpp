// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy_Controller.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "UObject/ConstructorHelpers.h"

AEnemy_Controller::AEnemy_Controller(FObjectInitializer const& a_pObjectInit)
{
    // Get the behaviour tree from reference.
    static ConstructorHelpers::FObjectFinder<UBehaviorTree> treeFinder(TEXT("/Script/AIModule.BehaviorTree'/Game/Goldfish/AI/BT_Enemy.BT_Enemy'"));
    if (treeFinder.Succeeded())
    {
        m_pBehaviorTree = treeFinder.Object;
    }

    // Init the tree and blackboard components.
    m_pBehaviorTreeComponent = a_pObjectInit.CreateDefaultSubobject<UBehaviorTreeComponent>(this, TEXT("BehaviorTree Component"));
    m_pBlackboard = a_pObjectInit.CreateDefaultSubobject<UBlackboardComponent>(this, TEXT("Blackboard Component"));
}

void AEnemy_Controller::BeginPlay()
{
    Super::BeginPlay();

    // Run the behaviour tree.
    RunBehaviorTree(m_pBehaviorTree);
    m_pBehaviorTreeComponent->StartTree(*m_pBehaviorTree);
}

void AEnemy_Controller::OnPossess(APawn* a_pPawn)
{
    // Run default possess method.
    Super::OnPossess(a_pPawn);

    // Init blackboard.
    if (m_pBlackboard)
    {
        m_pBlackboard->InitializeBlackboard(*m_pBehaviorTree->BlackboardAsset);
    }
}

UBlackboardComponent* AEnemy_Controller::GetBlackboard() const
{
    return m_pBlackboard;
}