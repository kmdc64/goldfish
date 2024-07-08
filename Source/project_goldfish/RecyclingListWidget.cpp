// A learning project by KDC64.


#include "RecyclingListWidget.h"
#include "RecyclingItemWidget.h"

void URecyclingListWidget::Setup(TArray<RecyclingItemData> data)
{
	m_data = data;

	UWorld* world = GetWorld();

	// Generate the recycling item widgets.
	for (int i = 0; i < 3; i++)
	{
		URecyclingItemWidget* item = CreateWidget<URecyclingItemWidget>(world, ItemTemplate);
		item->AddToViewport();
		m_items.Add(item);
	}
}

void URecyclingListWidget::TearDown()
{
	int itemNumber = m_items.Num();
	if (itemNumber > 0)
	{
		for (int i = itemNumber; i > 0; i--)
		{
			m_items[i]->RemoveFromParent();
		}

		m_items.Empty();
	}
}

void URecyclingListWidget::MoveBackwards()
{
}

void URecyclingListWidget::MoveForwards()
{
}

void URecyclingListWidget::RefreshAll()
{
}

void URecyclingListWidget::RefreshItem(int index)
{
}
