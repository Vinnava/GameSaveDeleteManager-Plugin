// Copyright 2025 Vinnava. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

DECLARE_LOG_CATEGORY_EXTERN(LogGameSaveDeleteManager, Log, All);

/**
 * SGameSaveDeleteManagerToolbar
 *
 * Slate compound widget injected into the Level Editor toolbar (next to Play).
 * Contains:
 *   - A primary "Delete Saves" button → triggers delete based on current settings
 *   - A dropdown arrow               → shows scanned .sav files + quick-config options
 */
class GAMESAVEDELETEMANAGER_API SGameSaveDeleteManagerToolbar : public SCompoundWidget
{
public:

    SLATE_BEGIN_ARGS(SGameSaveDeleteManagerToolbar) {}
    SLATE_END_ARGS()

    void Construct(const FArguments& inArgs);

private:

    // ─── Styling ─────────────────────────────────────────────────────────────

    /** Custom toolbar button style. Must be a member — Slate stores a raw pointer
     *  to it via .ButtonStyle() and reads it on every OnPaint() call. */
    FButtonStyle buttonStyle;

    /** Per-state rounded brushes — owned here so pointers in buttonStyle stay valid. */
    FSlateBrush brushNormal;
    FSlateBrush brushHovered;
    FSlateBrush brushPressed;

    // ─── Dropdown ────────────────────────────────────────────────────────────

    /** Builds the dropdown menu content shown when the arrow is clicked. */
    TSharedRef<SWidget> BuildDropdownContent();

    /** Scans the configured save path and returns all discovered .sav slot names. */
    TArray<FString> ScanSaveFiles() const;

    // ─── Delete Logic ────────────────────────────────────────────────────────

    /** Entry point: reads settings, shows confirmation, then delegates to delete helpers. */
    FReply OnDeleteClicked();

    /** Void wrapper for FExecuteAction binding (delegates require void return). */
    void OnDeleteClickedVoid();

    /**
     * Deletes a specific list of save slots.
     * @param slotNames  Slot names without .sav extension.
     * @param savePath   Resolved absolute directory path.
     */
    void DeleteSpecificSlots(const TArray<FString>& slotNames, const FString& savePath);

    /** Deletes every .sav file found in savePath. */
    void DeleteAllSaves(const FString& savePath);

    // ─── Confirmation Dialog ─────────────────────────────────────────────────

    /**
     * Shows a modal confirmation dialog.
     * @param message  Body text describing what will be deleted.
     * @return         True if the user confirmed.
     */
    bool ShowConfirmationDialog(const FString& message) const;

    // ─── Dropdown Slot Actions ────────────────────────────────────────────────

    /** Adds a slot name to the settings array (and saves config). */
    void AddSlotToSettings(FString slotName);

    /** Removes a slot name from the settings array (and saves config). */
    void RemoveSlotFromSettings(FString slotName);

    /** Deletes a single slot immediately after confirmation. */
    void DeleteSingleSlot(FString slotName);

    /** Opens the Project Settings panel filtered to Game Save Delete Manager. */
    void OpenPluginSettings() const;
};