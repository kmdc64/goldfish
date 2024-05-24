// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Enemy_Controller.generated.h"
 
/**
 * AEnemy_Controller:
 * Manages enemy AI behaviors.
 */
UCLASS()
class PROJECT_GOLDFISH_API AEnemy_Controller : public AAIController
{
	GENERATED_BODY()
	
public:
	AEnemy_Controller(FObjectInitializer const& pObjectInit);

	// Overrides
	void BeginPlay() override;
	void OnPossess(APawn* pPawn) override;

	class UBlackboardComponent* GetBlackboard() const;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "AI")
	class UBehaviorTreeComponent* PBehaviorTreeComponent;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "AI")
	class UBehaviorTree* PBehaviorTree;

private:
	class UBlackboardComponent* m_pBlackboard;
};
