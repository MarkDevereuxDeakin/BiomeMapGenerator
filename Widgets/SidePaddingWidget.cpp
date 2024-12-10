#include "SidePaddingWidget.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SBox.h"

void SSidePaddingWidget::Construct(const FArguments& InArgs)
{
    ChildSlot
    [
        SNew(SHorizontalBox)
        + SHorizontalBox::Slot()
        .FillWidth(0.2f)
        [
            SNew(SBox) // Empty box
        ]
    ];
}
