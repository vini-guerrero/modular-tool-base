#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"

class SAboutMineSweeperWidget : public SCompoundWidget
{
public:

	SLATE_BEGIN_ARGS(SAboutMineSweeperWidget) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
};
