#pragma once

#include "UnrealEd.h"
#include "Modules/ModuleManager.h"
#include "Interfaces/IMTBModuleInterface.h"

class IMTBCustomModule
{

public:

    virtual void OnStartupModule() {};
    virtual void OnShutdownModule() {};

};

class IMTBModuleInterface : public IModuleInterface
{

public:
    void StartupModule() override
    {
        if (!IsRunningCommandlet())
        {
            AddModuleListeners();
            for (int32 i = 0; i < ModuleListeners.Num(); ++i)
            {
                ModuleListeners[i]->OnStartupModule();
            }
        }
    }
    void ShutdownModule() override
    {
        for (int32 i = 0; i < ModuleListeners.Num(); ++i)
        {
            ModuleListeners[i]->OnShutdownModule();
        }
    }
    virtual void AddModuleListeners() {};

protected:

    TArray<TSharedRef<IMTBCustomModule>> ModuleListeners;

};