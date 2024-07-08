// A learning project by KDC64.

#pragma once

#include "CoreMinimal.h"

USTRUCT(BlueprintType, NoExport)
struct FRecyclingListParameters
{
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	float CycleItemDurationSeconds;
};