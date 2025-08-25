
#include "MineSweeper/SAboutMineSweeperWidget.h"

#include "Styling/AppStyle.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Interfaces/IPluginManager.h"

void SAboutMineSweeperWidget::Construct(const FArguments& InArgs)
{
    FString PluginVersion = TEXT("v.X.X.X");
    FString PluginDescription = TEXT("Description");
    FString PluginCreator = TEXT("Creator");
    if (TSharedPtr<IPlugin> Plugin = IPluginManager::Get().FindPlugin(TEXT("MTB")))
    {
        const FPluginDescriptor& Desc = Plugin->GetDescriptor();
        PluginVersion = FString::Printf(TEXT("v%s"), *Desc.VersionName);
        PluginDescription = Desc.Description.IsEmpty() ? PluginDescription : Desc.Description.Replace(TEXT(". "), TEXT(".\n"), ESearchCase::IgnoreCase);
        PluginCreator = Desc.CreatedBy.IsEmpty() ? PluginCreator : Desc.CreatedBy;
    }
    ChildSlot
    [
        SNew(SBorder)
        .Padding(16)
        .BorderImage(FAppStyle::Get().GetBrush("ToolPanel.GroupBorder"))
        [
            SNew(SScrollBox)
            + SScrollBox::Slot()
            [
                SNew(SBorder)
                .BorderBackgroundColor(FLinearColor(0.75f, 0.75f, 0.75f, 1.0f))
                .Padding(15.0f)
                [
                    SNew(SBox)
                    .MinDesiredWidth(400.f)
                    [
                        SNew(SVerticalBox)
                        + SVerticalBox::Slot()
                        .AutoHeight()
                        .Padding(0, 0, 0, 12)
                        [
                            SNew(STextBlock)
                            .Font(FCoreStyle::GetDefaultFontStyle("Bold", 14))
                            .Text(FText::FromString("Modular Tool Base"))
                        ]
                        + SVerticalBox::Slot()
                        .AutoHeight()
                        .Padding(0, 0, 0, 12)
                        [
                            SNew(STextBlock)
                            .AutoWrapText(true)
                            .TextStyle(&FAppStyle::Get().GetWidgetStyle<FTextBlockStyle>("NormalText"))
                            .Text(FText::FromString(PluginDescription))
                        ]
                        + SVerticalBox::Slot()
                        .AutoHeight()
                        [
                            SNew(SHorizontalBox)
                            + SHorizontalBox::Slot()
                            .HAlign(HAlign_Left)
                            [
                                SNew(STextBlock)
                                .AutoWrapText(true)
                                .TextStyle(&FAppStyle::Get().GetWidgetStyle<FTextBlockStyle>("NormalText"))
                                .Text(FText::FromString(FString::Printf(TEXT("%s - %d"), *PluginCreator, FDateTime::Now().GetYear())))
                            ]
                            + SHorizontalBox::Slot()
                            .HAlign(HAlign_Right)
                            .FillWidth(1.0f)
                            [
                                SNew(STextBlock)
                                .TextStyle(&FAppStyle::Get().GetWidgetStyle<FTextBlockStyle>("NormalText"))
                                .Text(FText::FromString(PluginVersion))
                            ]
                        ]
                    ]
                ]
            ]
        ]
    ];
}