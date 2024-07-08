// A learning project by KDC64.

#pragma once

#include "RecyclingItemInterface.generated.h"

UINTERFACE(Blueprintable)
class URecyclingItemInterface : public UInterface
{
	GENERATED_BODY()
};

class IRecyclingItemInterface
{
	GENERATED_BODY()

public:
	void Setup();
	void TearDown();

protected:
	void Refresh();
};
