#pragma once

#include "UnrealEd.h"
#include "Interfaces/IMTBModuleInterface.h"

class FModularToolBase : public IMTBModuleInterface
{
    
public:

    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
    virtual void AddModuleListeners() override;

    static inline FModularToolBase& Get() { return FModuleManager::LoadModuleChecked<FModularToolBase>("MTB"); }
    static inline bool IsAvailable() { return FModuleManager::Get().IsModuleLoaded("MTB"); }
    
    TSharedRef<FWorkspaceItem> GetMenuRoot() { return CustomToolRoot; };
    
protected:
    
    TSharedPtr<FExtender> MenuExtender;
    TSharedPtr<SWindow> AboutWindowPtr;
    TSharedPtr<FExtensibilityManager> LevelEditorMenuExtensibilityManager;
    static TSharedRef<FWorkspaceItem> CustomToolRoot;
    
    void CreatePulldownMenu(FMenuBarBuilder &InMenuBuilder);
    void FillPulldownMenu(FMenuBuilder &InMenuBuilder);

};