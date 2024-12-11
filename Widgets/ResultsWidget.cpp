#include "ResultsWidget.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/SBoxPanel.h"


void SResultsWidget::Construct(const FArguments& InArgs)
{
    ChildSlot
    [
        SNew(SVerticalBox)
        // Result Display
        + SVerticalBox::Slot()
        .AutoHeight()
        .HAlign(HAlign_Center)
        [
            SAssignNew(ResultsTextBlock, STextBlock)
            .Text(FText::FromString("Results will be displayed here."))
        ]        
        
    ];
}

void SResultsWidget::UpdateResults(const FString& ResultsText)
{
    if (ResultsTextBlock.IsValid())
    {
        ResultsTextBlock->SetText(FText::FromString(ResultsText));
    }
}

