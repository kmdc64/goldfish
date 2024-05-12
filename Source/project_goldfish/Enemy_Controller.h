// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Enemy_Controller.generated.h"
 
/**
 * 
 */
UCLASS()
class PROJECT_GOLDFISH_API AEnemy_Controller : public AAIController
{
	GENERATED_BODY()
	
public:
	AEnemy_Controller(FObjectInitializer const& a_pObjectInit);

	// Overrides
	void BeginPlay() override;
	void OnPossess(APawn* a_pPawn) override;

	class UBlackboardComponent* GetBlackboard() const;

	// Tree components
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "AI")
	class UBehaviorTreeComponent* m_pBehaviorTreeComponent;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "AI")
	class UBehaviorTree* m_pBehaviorTree;

private:
	class UBlackboardComponent* m_pBlackboard;
};
