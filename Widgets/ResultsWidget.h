#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class BIOMEMAPPER_API SResultsWidget : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SResultsWidget) {}
    SLATE_END_ARGS()

    /** Constructs the widget */
    void Construct(const FArguments& InArgs);

    /** Updates the displayed results. */
    void UpdateResults(const FString& ResultsText);

    // Result Display
    TSharedPtr<STextBlock> ResultsTextBlock;
};