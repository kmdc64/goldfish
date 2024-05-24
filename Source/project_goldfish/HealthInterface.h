// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HealthInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UHealthInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * IHealthInterface:
 * An UnrealInterface for an entity that can receive and recover damage.
 */
class PROJECT_GOLDFISH_API IHealthInterface
{
	GENERATED_BODY()

public:
	// Remove health from the damageable object.
	virtual void ReceiveDamage(int amount);
	// Add health to the damageable object.
	virtual void RecoverHealth(int amount);
public:
};
