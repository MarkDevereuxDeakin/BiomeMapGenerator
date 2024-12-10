#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class BIOMEMAPPER_API SSidePaddingWidget : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SSidePaddingWidget) {}
    SLATE_END_ARGS()

    /** Constructs the widget */
    void Construct(const FArguments& InArgs);
};
