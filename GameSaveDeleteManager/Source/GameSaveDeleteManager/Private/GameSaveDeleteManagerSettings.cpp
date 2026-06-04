// Copyright 2025 Vinnava. All Rights Reserved.

#include "GameSaveDeleteManagerSettings.h"
#include "Misc/Paths.h"

UGameSaveDeleteManagerSettings::UGameSaveDeleteManagerSettings()
{
    bDeleteAllSaves     = true;
    customSavePath.Path = TEXT("");
}

FString UGameSaveDeleteManagerSettings::GetResolvedSavePath() const
{
    if (!customSavePath.Path.IsEmpty())
    {
        FString resolved = customSavePath.Path;

        if (FPaths::IsRelative(resolved))
        {
            resolved = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir(), resolved);
        }

        // FIX: NormalizeDirectoryName returns void — mutate in-place, then return
        FPaths::NormalizeDirectoryName(resolved);
        return resolved;
    }

    // Default UE save path: <Project>/Saved/SaveGames/
    return FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir() / TEXT("SaveGames"));
}
