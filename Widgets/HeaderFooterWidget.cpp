#include "HeaderFooterWidget.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SBox.h"

void SHeaderFooterWidget::Construct(const FArguments& InArgs)
{
    ChildSlot
    [
        SNew(SVerticalBox)       
        + SVerticalBox::Slot()
        .FillHeight(0.25f)
        [
            SNew(SBox) // Empty box
        ]
    ];
}
