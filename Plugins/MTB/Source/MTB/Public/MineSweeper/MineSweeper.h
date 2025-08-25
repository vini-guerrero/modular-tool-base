#pragma once

#include "MTBNomadTab.h"

class MineSweeperTool : public FMTBNomadTab
{

public:
    
    virtual void Initialize() override;
    virtual void OnStartupModule() override;
    virtual void OnShutdownModule() override;
    virtual TSharedRef<SDockTab> SpawnTab(const FSpawnTabArgs& TabSpawnArgs) override;

private:

    TWeakPtr<SDockTab> MineSweeperTabPtr;

};