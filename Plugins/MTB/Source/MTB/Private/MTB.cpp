
#include "MTB.h"
#include "LevelEditor.h"
#include "Interfaces/IMTBModuleInterface.h"

#include "MineSweeper/MineSweeper.h" // Custom Nomad Tab
#include "MineSweeper/SAboutMineSweeperWidget.h" // Custom Slate Widget

IMPLEMENT_GAME_MODULE(FModularToolBase, MTB)

TSharedRef<FWorkspaceItem> FModularToolBase::CustomToolRoot = FWorkspaceItem::NewGroup(FText::FromString("Menu Root"));

void FModularToolBase::AddModuleListeners()
{
    ModuleListeners.Add(MakeShareable(new MineSweeperTool));
}

void FModularToolBase::StartupModule()
{
    if (!IsRunningCommandlet())
    {
        FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
        LevelEditorMenuExtensibilityManager = LevelEditorModule.GetMenuExtensibilityManager();
        MenuExtender = MakeShareable(new FExtender);
        MenuExtender->AddMenuBarExtension("Tools", EExtensionHook::After, NULL, FMenuBarExtensionDelegate::CreateRaw(this, &FModularToolBase::CreatePulldownMenu));
        LevelEditorMenuExtensibilityManager->AddExtender(MenuExtender);
    }
    IMTBModuleInterface::StartupModule();
}

void FModularToolBase::ShutdownModule()
{
    IMTBModuleInterface::ShutdownModule();
    // Clean Up
    if (LevelEditorMenuExtensibilityManager.IsValid() && MenuExtender.IsValid())
    {
        LevelEditorMenuExtensibilityManager->RemoveExtender(MenuExtender);
    }
    MenuExtender.Reset();
    LevelEditorMenuExtensibilityManager.Reset();
    if (AboutWindowPtr.IsValid()) AboutWindowPtr.Reset();
}

void FModularToolBase::CreatePulldownMenu(FMenuBarBuilder &InMenuBuilder)
{
    InMenuBuilder.AddPullDownMenu(
        FText::FromString("MTB"),
        FText::FromString("Open modular tool base menu"),
        FNewMenuDelegate::CreateRaw(this, &FModularToolBase::FillPulldownMenu), "Modular Tool Base", FName(TEXT("Modular Tool Base"))
    );
}

void FModularToolBase::FillPulldownMenu(FMenuBuilder &InMenuBuilder)
{
    InMenuBuilder.BeginSection("MineSweeper", FText::FromString("MineSweeper"));
    InMenuBuilder.AddSubMenu(
        FText::FromString("MineSweeper"),
        FText::FromString("Open MineSweeper tools"),
        FNewMenuDelegate::CreateLambda([this](FMenuBuilder& SubMenuBuilder)
        {
            SubMenuBuilder.AddMenuEntry(
                FText::FromString("Play Game"),
                FText::FromString("Open MineSweeper Tool"),
                FSlateIcon(FAppStyle::GetAppStyleSetName(), "EditorViewport.LocationGridSnap"),
                FUIAction(FExecuteAction::CreateLambda([]() { 
                    FGlobalTabmanager::Get()->TryInvokeTab(FName("MineSweeper")); 
                }))
            );
            SubMenuBuilder.AddMenuEntry(
                FText::FromString("About"),
                FText::FromString("Open About Popup"),
                FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.Info"),
                FUIAction(FExecuteAction::CreateLambda([this]()
                {
                    if (AboutWindowPtr.IsValid())
                    {
                        AboutWindowPtr->RequestDestroyWindow();
                        AboutWindowPtr.Reset();
                    }
                    const FVector2D AboutWindowSize = FVector2D(480.f, 200.f);
                    TSharedRef<SWindow> AboutWindow = SNew(SWindow)
                        .Title(FText::FromString("About Modular Tool Base"))
                        .ClientSize(AboutWindowSize)
                        .SupportsMinimize(false)
                        .SizingRule(ESizingRule::FixedSize)
                        .SupportsMaximize(false);
                    AboutWindow->SetContent(SNew(SAboutMineSweeperWidget));
                    FSlateApplication::Get().AddWindow(AboutWindow);
                    AboutWindowPtr = AboutWindow;
                }))
            );
        })
    );
    InMenuBuilder.EndSection();
}