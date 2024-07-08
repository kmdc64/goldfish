// A learning project by KDC64.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RecyclingItemData.h"
#include "RecyclingItemInterface.h"
#include "RecyclingItemWidget.h"
#include "RecyclingListParameters.h"
#include "RecyclingListWidget.generated.h"

/**
 * RecyclingListWidget
 * A UI widget that populates a list with recycling sub-widgets that can hotswap their contents.
 */
UCLASS(Abstract)
class PROJECT_GOLDFISH_API URecyclingListWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRecyclingListParameters Params;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<URecyclingItemWidget> ItemTemplate;

	TArray<RecyclingItemData> m_data;
	TArray<URecyclingItemWidget*> m_items;

protected:
	void Setup(TArray<RecyclingItemData> data);
	void TearDown();
	void MoveBackwards();
	void MoveForwards();
	void RefreshAll();
	void RefreshItem(int index);
};
