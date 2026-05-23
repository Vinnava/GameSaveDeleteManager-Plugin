#include "GameSaveDeleteManager.h"
#include "SGameSaveDeleteManagerToolbar.h"

#include "LevelEditor.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Framework/MultiBox/MultiBoxExtender.h"
#include "Modules/ModuleManager.h"

#define LOCTEXT_NAMESPACE "FGameSaveDeleteManagerModule"

// ─────────────────────────────────────────────────────────────────────────────
// Startup / Shutdown
// ─────────────────────────────────────────────────────────────────────────────

void FGameSaveDeleteManagerModule::StartupModule()
{
    if (!IsRunningCommandlet())
    {
        FLevelEditorModule& levelEditor =
            FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");

        toolbarExtender = MakeShared<FExtender>();

        // Anchor after the "Play" button group
        toolbarExtender->AddToolBarExtension(
            "Play",
            EExtensionHook::After,
            nullptr,
            FToolBarExtensionDelegate::CreateRaw(
                this, &FGameSaveDeleteManagerModule::AddToolbarExtension)
        );

        levelEditor.GetToolBarExtensibilityManager()->AddExtender(toolbarExtender);

        UE_LOG(LogGameSaveDeleteManager, Log,
            TEXT("[FGameSaveDeleteManagerModule] [StartupModule] Toolbar extension registered"));
    }
}

void FGameSaveDeleteManagerModule::ShutdownModule()
{
    if (toolbarExtender.IsValid())
    {
        FLevelEditorModule* levelEditor =
            FModuleManager::GetModulePtr<FLevelEditorModule>("LevelEditor");

        if (levelEditor)
        {
            levelEditor->GetToolBarExtensibilityManager()->RemoveExtender(toolbarExtender);
        }

        toolbarExtender.Reset();

        UE_LOG(LogGameSaveDeleteManager, Log,
            TEXT("[FGameSaveDeleteManagerModule] [ShutdownModule] Toolbar extension removed"));
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Toolbar Extension
// ─────────────────────────────────────────────────────────────────────────────

void FGameSaveDeleteManagerModule::AddToolbarExtension(FToolBarBuilder& builder)
{
    builder.BeginSection("GameSaveDeleteManager");
    {
        builder.AddWidget(
            SNew(SGameSaveDeleteManagerToolbar),
            NAME_None,
            /* bSearchable */ false
        );
    }
    builder.EndSection();
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FGameSaveDeleteManagerModule, GameSaveDeleteManager)
