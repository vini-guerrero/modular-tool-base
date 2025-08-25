
#include "MineSweeper/MineSweeper.h"

#include "MTB.h"
#include "MineSweeper/SMineSweeperWidget.h"

void MineSweeperTool::Initialize()
{
    TabName = "MineSweeper";
    TabDisplayName = FText::FromString("MineSweeper");
    ToolTipText = FText::FromString("MineSweeper");
}

void MineSweeperTool::OnStartupModule()
{
    FMTBNomadTab::OnStartupModule();
}

void MineSweeperTool::OnShutdownModule()
{
    FMTBNomadTab::OnShutdownModule();
    FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(TabName);
    if (MineSweeperTabPtr.IsValid())
    {
        MineSweeperTabPtr.Pin()->RequestCloseTab();
        MineSweeperTabPtr.Reset();
    }
}

TSharedRef<SDockTab> MineSweeperTool::SpawnTab(const FSpawnTabArgs& TabSpawnArgs)
{
    TSharedRef<SDockTab> SpawnedTab = SNew(SDockTab)
    .TabRole(ETabRole::NomadTab)
    [
        SNew(SMineSweeperWidget)
    ];
    MineSweeperTabPtr = SpawnedTab;
    return SpawnedTab;
}