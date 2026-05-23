#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "GameSaveDeleteManagerSettings.generated.h"

UCLASS(Config = EditorPerProjectUserSettings, meta = (DisplayName = "Game Save Delete Manager"))
class GAMESAVEDELETEMANAGER_API UGameSaveDeleteManagerSettings : public UDeveloperSettings
{
    GENERATED_BODY()

public:

    UGameSaveDeleteManagerSettings();

    // UDeveloperSettings interface
    virtual FName GetContainerName() const override { return FName("Project"); }
    virtual FName GetCategoryName() const override  { return FName("Plugins"); }
    virtual FName GetSectionName()  const override  { return FName("GameSaveDeleteManager"); }

    // ─── Settings ────────────────────────────────────────────────────────────

    /** When true, the toolbar button deletes ALL .sav files in the save path. */
    UPROPERTY(Config, EditAnywhere, Category = "Game Save Delete Manager",
        meta = (DisplayName = "Delete All Saves on Click"))
    bool bDeleteAllSaves;

    /**
     * Specific save slot names to delete (without .sav extension).
     * Only used when 'Delete All Saves' is disabled.
     * Example: ["PlayerSave", "SettingsSave"]
     */
    UPROPERTY(Config, EditAnywhere, Category = "Game Save Delete Manager",
        meta = (DisplayName = "Save Slots to Delete", EditCondition = "!bDeleteAllSaves"))
    TArray<FString> saveSlots;

    /**
     * Override the default save directory.
     * Leave empty to use the default: [ProjectSaved]/SaveGames/
     * Must be an absolute path or a path relative to the project root.
     */
    UPROPERTY(Config, EditAnywhere, Category = "Game Save Delete Manager",
        meta = (DisplayName = "Custom Save Path (optional)"))
    FDirectoryPath customSavePath;

    // ─── Helpers ─────────────────────────────────────────────────────────────

    /** Returns the resolved absolute save directory to operate on. */
    FString GetResolvedSavePath() const;
};
