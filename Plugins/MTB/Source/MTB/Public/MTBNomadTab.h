#pragma once

#include "MTB.h"
#include "Widgets/Docking/SDockTab.h"
#include "Framework/Docking/TabManager.h"
#include "Interfaces/IMTBModuleInterface.h"

class FMTBNomadTab : public IMTBCustomModule, public TSharedFromThis<FMTBNomadTab>
{

public:

    FMTBNomadTab() = default;
    virtual ~FMTBNomadTab() = default;

    virtual void OnStartupModule() override
    {
        Initialize();
        FGlobalTabmanager::Get()->RegisterNomadTabSpawner(TabName, FOnSpawnTab::CreateRaw(this, &FMTBNomadTab::SpawnTab))
        .SetGroup(FModularToolBase::Get().GetMenuRoot()).SetDisplayName(TabDisplayName).SetTooltipText(ToolTipText);
    };

    virtual void Initialize() {};
    virtual void OnShutdownModule() override { FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(TabName); };
    virtual void MakeMenuEntry(FMenuBuilder &InMenuBuilder) { FGlobalTabmanager::Get()->PopulateTabSpawnerMenu(InMenuBuilder, TabName); };
    virtual TSharedRef<SDockTab> SpawnTab(const FSpawnTabArgs& TabSpawnArgs) { return SNew(SDockTab); };

protected:

    FName TabName;
    FText TabDisplayName;
    FText ToolTipText;
};