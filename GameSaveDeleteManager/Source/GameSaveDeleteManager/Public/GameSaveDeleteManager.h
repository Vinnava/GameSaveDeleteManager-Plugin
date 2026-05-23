#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FToolBarBuilder;
class FExtender;

/**
 * FGameSaveDeleteManagerModule
 *
 * Editor-only module. Registers the toolbar widget into the Level Editor
 * play toolbar and tears it down cleanly on shutdown.
 */
class FGameSaveDeleteManagerModule : public IModuleInterface
{
public:

    // IModuleInterface
    virtual void StartupModule()  override;
    virtual void ShutdownModule() override;

private:

    /** Builds and injects the toolbar widget. */
    void AddToolbarExtension(FToolBarBuilder& builder);

    /** Kept alive for the full editor session. */
    TSharedPtr<FExtender> toolbarExtender;
};
