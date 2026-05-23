#include "SGameSaveDeleteManagerToolbar.h"
#include "GameSaveDeleteManagerSettings.h"

#include "HAL/FileManager.h"
#include "Misc/Paths.h"
#include "Misc/MessageDialog.h"

#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Framework/Application/SlateApplication.h"

#include "Widgets/Input/SComboButton.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/SBoxPanel.h"

#include "Styling/AppStyle.h"
#include "ISettingsModule.h"
#include "Modules/ModuleManager.h"

DEFINE_LOG_CATEGORY(LogGameSaveDeleteManager);

// ─────────────────────────────────────────────────────────────────────────────
// Construct
// ─────────────────────────────────────────────────────────────────────────────

void SGameSaveDeleteManagerToolbar::Construct(const FArguments& inArgs)
{
    ChildSlot
    [
        SNew(SComboButton)
        // Match the "Play" toolbar button style — same pill shape as Pixel Streaming
        .ButtonStyle(FAppStyle::Get(), "EditorViewportToolBar.ComboMenu.Button")
        .ToolTipText(NSLOCTEXT("GameSaveDeleteManager", "ComboTip",
            "Delete save game files.\nClick arrow to configure slots or open settings."))
        .HasDownArrow(true)
        .OnGetMenuContent(this, &SGameSaveDeleteManagerToolbar::BuildDropdownContent)
        .ContentPadding(FMargin(6.f, 2.f))
        .ButtonContent()
        [
            SNew(SHorizontalBox)

            // ── Trash icon ────────────────────────────────────────────────────
            + SHorizontalBox::Slot()
            .AutoWidth()
            .VAlign(VAlign_Center)
            .Padding(0.f, 0.f, 4.f, 0.f)
            [
                SNew(SImage)
                .Image(FAppStyle::GetBrush("Icons.Delete"))
                .DesiredSizeOverride(FVector2D(14.f, 14.f))
                .ColorAndOpacity(FSlateColor(FLinearColor(1.f, 0.35f, 0.35f, 1.f)))
            ]

            // ── Label ─────────────────────────────────────────────────────────
            + SHorizontalBox::Slot()
            .AutoWidth()
            .VAlign(VAlign_Center)
            [
                SNew(STextBlock)
                .Text(NSLOCTEXT("GameSaveDeleteManager", "DeleteBtnLabel", "Del Saves"))
                .TextStyle(FAppStyle::Get(), "EditorViewportToolBar.ComboMenu.TextStyle")
            ]
        ]
    ];
}

// ─────────────────────────────────────────────────────────────────────────────
// Dropdown Content
// ─────────────────────────────────────────────────────────────────────────────

TSharedRef<SWidget> SGameSaveDeleteManagerToolbar::BuildDropdownContent()
{
    const UGameSaveDeleteManagerSettings* settings = GetDefault<UGameSaveDeleteManagerSettings>();
    if (!settings)
    {
        return SNew(STextBlock)
            .Text(NSLOCTEXT("GameSaveDeleteManager", "NoSettings", "Settings unavailable"));
    }

    FMenuBuilder menuBuilder(true, nullptr);

    // ── Primary action — top of menu, most prominent ──────────────────────────
    menuBuilder.AddMenuEntry(
        settings->bDeleteAllSaves
            ? NSLOCTEXT("GameSaveDeleteManager", "DeleteNowAll",      "🗑  Delete All Saves Now")
            : NSLOCTEXT("GameSaveDeleteManager", "DeleteNowSelected", "🗑  Delete Selected Saves Now"),
        NSLOCTEXT("GameSaveDeleteManager", "DeleteNowTip",
            "Run delete immediately based on current settings"),
        FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.Delete"),
        FUIAction(FExecuteAction::CreateSP(this, &SGameSaveDeleteManagerToolbar::OnDeleteClickedVoid))
    );

    menuBuilder.AddSeparator();
    const FText modeLabel = settings->bDeleteAllSaves
        ? NSLOCTEXT("GameSaveDeleteManager", "ModeAll",       "Mode: Delete ALL saves")
        : NSLOCTEXT("GameSaveDeleteManager", "ModeSelective", "Mode: Delete selected slots");

    menuBuilder.AddMenuEntry(
        modeLabel,
        NSLOCTEXT("GameSaveDeleteManager", "ModeDesc",
            "Change this in Project Settings → Plugins → Game Save Delete Manager"),
        FSlateIcon(),
        FUIAction()
    );

    menuBuilder.AddSeparator();

    // ── Discovered .sav files ─────────────────────────────────────────────────
    TArray<FString> discovered = ScanSaveFiles();

    if (discovered.IsEmpty())
    {
        menuBuilder.AddMenuEntry(
            NSLOCTEXT("GameSaveDeleteManager", "NoSaves", "No .sav files found"),
            NSLOCTEXT("GameSaveDeleteManager", "NoSavesTip",
                "Play the game at least once to generate save files"),
            FSlateIcon(),
            FUIAction()
        );
    }
    else
    {
        menuBuilder.BeginSection("DiscoveredSlots",
            NSLOCTEXT("GameSaveDeleteManager", "SlotsHeader", "Discovered Save Slots"));

        for (const FString& slotName : discovered)
        {
            // FIX: Capture by value — copy into local FString so CreateSP binds
            // against FString (by-value) matching the method signature void(FString).
            // Binding const FString& directly causes template deduction failure
            // because std::decay_t strips the ref, leaving a type mismatch.
            const FString slotCopy = slotName;

            const bool bAlreadyTracked = settings->saveSlots.Contains(slotName);

            // FIX: AddMenuEntry correct overload order: Label, Tooltip, Icon, FUIAction
            menuBuilder.AddMenuEntry(
                FText::Format(NSLOCTEXT("GameSaveDeleteManager", "DeleteSlot",
                    "Delete: {0}"), FText::FromString(slotName)),
                FText::Format(NSLOCTEXT("GameSaveDeleteManager", "DeleteSlotTip",
                    "Delete {0}.sav immediately (with confirmation)"), FText::FromString(slotName)),
                FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.Delete"),
                FUIAction(FExecuteAction::CreateSP(
                    this, &SGameSaveDeleteManagerToolbar::DeleteSingleSlot, slotCopy))
            );

            if (!settings->bDeleteAllSaves)
            {
                if (bAlreadyTracked)
                {
                    menuBuilder.AddMenuEntry(
                        FText::Format(NSLOCTEXT("GameSaveDeleteManager", "RemoveSlot",
                            "Remove '{0}' from delete list"), FText::FromString(slotName)),
                        FText::Format(NSLOCTEXT("GameSaveDeleteManager", "RemoveSlotTip",
                            "Remove {0} from the selective delete list in settings"),
                            FText::FromString(slotName)),
                        FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.Minus"),
                        FUIAction(FExecuteAction::CreateSP(
                            this, &SGameSaveDeleteManagerToolbar::RemoveSlotFromSettings, slotCopy))
                    );
                }
                else
                {
                    menuBuilder.AddMenuEntry(
                        FText::Format(NSLOCTEXT("GameSaveDeleteManager", "AddSlot",
                            "Add '{0}' to delete list"), FText::FromString(slotName)),
                        FText::Format(NSLOCTEXT("GameSaveDeleteManager", "AddSlotTip",
                            "Add {0} to the selective delete list in settings"),
                            FText::FromString(slotName)),
                        FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.Plus"),
                        FUIAction(FExecuteAction::CreateSP(
                            this, &SGameSaveDeleteManagerToolbar::AddSlotToSettings, slotCopy))
                    );
                }
            }
        }

        menuBuilder.EndSection();
    }

    menuBuilder.AddSeparator();

    // ── Open plugin settings ──────────────────────────────────────────────────
    menuBuilder.AddMenuEntry(
        NSLOCTEXT("GameSaveDeleteManager", "OpenSettings", "Open Plugin Settings..."),
        NSLOCTEXT("GameSaveDeleteManager", "OpenSettingsTip",
            "Open Project Settings → Plugins → Game Save Delete Manager"),
        FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.Settings"),
        FUIAction(FExecuteAction::CreateSP(
            this, &SGameSaveDeleteManagerToolbar::OpenPluginSettings))
    );

    return menuBuilder.MakeWidget();
}

// ─────────────────────────────────────────────────────────────────────────────
// File Discovery
// ─────────────────────────────────────────────────────────────────────────────

TArray<FString> SGameSaveDeleteManagerToolbar::ScanSaveFiles() const
{
    TArray<FString> slotNames;

    const UGameSaveDeleteManagerSettings* settings = GetDefault<UGameSaveDeleteManagerSettings>();
    if (!settings)
    {
        return slotNames;
    }

    const FString savePath = settings->GetResolvedSavePath();

    TArray<FString> foundFiles;
    IFileManager::Get().FindFiles(foundFiles, *(savePath / TEXT("*.sav")), true, false);

    for (const FString& fileName : foundFiles)
    {
        slotNames.Add(FPaths::GetBaseFilename(fileName));
    }

    slotNames.Sort();
    return slotNames;
}

// ─────────────────────────────────────────────────────────────────────────────
// Primary Delete Button
// ─────────────────────────────────────────────────────────────────────────────

FReply SGameSaveDeleteManagerToolbar::OnDeleteClicked()
{
    const UGameSaveDeleteManagerSettings* settings = GetDefault<UGameSaveDeleteManagerSettings>();
    if (!settings)
    {
        UE_LOG(LogGameSaveDeleteManager, Error,
            TEXT("[SGameSaveDeleteManagerToolbar] [OnDeleteClicked] Settings object unavailable"));
        return FReply::Handled();
    }

    const FString savePath = settings->GetResolvedSavePath();

    if (settings->bDeleteAllSaves)
    {
        const FString msg = FString::Printf(
            TEXT("Delete ALL .sav files in:\n%s\n\nThis cannot be undone."), *savePath);

        if (!ShowConfirmationDialog(msg))
        {
            return FReply::Handled();
        }

        DeleteAllSaves(savePath);
    }
    else
    {
        if (settings->saveSlots.IsEmpty())
        {
            FMessageDialog::Open(EAppMsgType::Ok,
                NSLOCTEXT("GameSaveDeleteManager", "NoSlotsConfigured",
                    "No save slots are configured.\n\n"
                    "Add slot names via the dropdown or in:\n"
                    "Project Settings → Plugins → Game Save Delete Manager"));
            return FReply::Handled();
        }

        FString slotList;
        for (const FString& slot : settings->saveSlots)
        {
            slotList += FString::Printf(TEXT("  • %s.sav\n"), *slot);
        }

        const FString msg = FString::Printf(
            TEXT("Delete the following save files from:\n%s\n\n%s\nThis cannot be undone."),
            *savePath, *slotList);

        if (!ShowConfirmationDialog(msg))
        {
            return FReply::Handled();
        }

        DeleteSpecificSlots(settings->saveSlots, savePath);
    }

    return FReply::Handled();
}

// ─────────────────────────────────────────────────────────────────────────────
// Delete Helpers
// ─────────────────────────────────────────────────────────────────────────────

void SGameSaveDeleteManagerToolbar::DeleteAllSaves(const FString& savePath)
{
    TArray<FString> files;
    IFileManager::Get().FindFiles(files, *(savePath / TEXT("*.sav")), true, false);

    if (files.IsEmpty())
    {
        UE_LOG(LogGameSaveDeleteManager, Log,
            TEXT("[SGameSaveDeleteManagerToolbar] [DeleteAllSaves] No .sav files found in: %s"),
            *savePath);

        FMessageDialog::Open(EAppMsgType::Ok,
            NSLOCTEXT("GameSaveDeleteManager", "NothingToDelete",
                "No .sav files found to delete."));
        return;
    }

    int32 deletedCount = 0;
    int32 failedCount  = 0;

    for (const FString& fileName : files)
    {
        const FString fullPath = savePath / fileName;

        if (IFileManager::Get().Delete(*fullPath, false, true))
        {
            UE_LOG(LogGameSaveDeleteManager, Log,
                TEXT("[SGameSaveDeleteManagerToolbar] [DeleteAllSaves] Deleted: %s"), *fullPath);
            deletedCount++;
        }
        else
        {
            UE_LOG(LogGameSaveDeleteManager, Warning,
                TEXT("[SGameSaveDeleteManagerToolbar] [DeleteAllSaves] Failed to delete: %s"),
                *fullPath);
            failedCount++;
        }
    }

    FMessageDialog::Open(EAppMsgType::Ok,
        FText::Format(
            NSLOCTEXT("GameSaveDeleteManager", "DeleteAllResult",
                "Deleted {0} file(s). Failed: {1}."),
            FText::AsNumber(deletedCount),
            FText::AsNumber(failedCount)));
}

void SGameSaveDeleteManagerToolbar::DeleteSpecificSlots(
    const TArray<FString>& slotNames, const FString& savePath)
{
    int32 deletedCount  = 0;
    int32 failedCount   = 0;
    int32 notFoundCount = 0;

    for (const FString& slotName : slotNames)
    {
        const FString fullPath = savePath / slotName + TEXT(".sav");

        if (!IFileManager::Get().FileExists(*fullPath))
        {
            UE_LOG(LogGameSaveDeleteManager, Warning,
                TEXT("[SGameSaveDeleteManagerToolbar] [DeleteSpecificSlots] Not found: %s"),
                *fullPath);
            notFoundCount++;
            continue;
        }

        if (IFileManager::Get().Delete(*fullPath, false, true))
        {
            UE_LOG(LogGameSaveDeleteManager, Log,
                TEXT("[SGameSaveDeleteManagerToolbar] [DeleteSpecificSlots] Deleted: %s"),
                *fullPath);
            deletedCount++;
        }
        else
        {
            UE_LOG(LogGameSaveDeleteManager, Warning,
                TEXT("[SGameSaveDeleteManagerToolbar] [DeleteSpecificSlots] Failed: %s"),
                *fullPath);
            failedCount++;
        }
    }

    FMessageDialog::Open(EAppMsgType::Ok,
        FText::Format(
            NSLOCTEXT("GameSaveDeleteManager", "DeleteSlotsResult",
                "Deleted {0} file(s). Not found: {1}. Failed: {2}."),
            FText::AsNumber(deletedCount),
            FText::AsNumber(notFoundCount),
            FText::AsNumber(failedCount)));
}

// ─────────────────────────────────────────────────────────────────────────────
// Confirmation Dialog
// ─────────────────────────────────────────────────────────────────────────────

bool SGameSaveDeleteManagerToolbar::ShowConfirmationDialog(const FString& message) const
{
    const EAppReturnType::Type result = FMessageDialog::Open(
        EAppMsgType::YesNo,
        FText::FromString(message),
        NSLOCTEXT("GameSaveDeleteManager", "ConfirmTitle", "Confirm Save Deletion"));

    return result == EAppReturnType::Yes;
}

// Void wrapper — FExecuteAction requires void(), OnDeleteClicked returns FReply
void SGameSaveDeleteManagerToolbar::OnDeleteClickedVoid()
{
    OnDeleteClicked();
}

// ─────────────────────────────────────────────────────────────────────────────
// Slot Settings Management
// FIX: Parameters are FString (by value) — matches std::decay_t<FString> that
//      CreateSP deduces when binding a captured const FString local.
// ─────────────────────────────────────────────────────────────────────────────

void SGameSaveDeleteManagerToolbar::AddSlotToSettings(FString slotName)
{
    UGameSaveDeleteManagerSettings* settings = GetMutableDefault<UGameSaveDeleteManagerSettings>();
    if (!settings)
    {
        return;
    }

    if (!settings->saveSlots.Contains(slotName))
    {
        settings->saveSlots.Add(MoveTemp(slotName));
        settings->SaveConfig();

        UE_LOG(LogGameSaveDeleteManager, Log,
            TEXT("[SGameSaveDeleteManagerToolbar] [AddSlotToSettings] Added: %s"), *settings->saveSlots.Last());
    }
}

void SGameSaveDeleteManagerToolbar::RemoveSlotFromSettings(FString slotName)
{
    UGameSaveDeleteManagerSettings* settings = GetMutableDefault<UGameSaveDeleteManagerSettings>();
    if (!settings)
    {
        return;
    }

    const int32 removed = settings->saveSlots.Remove(slotName);
    if (removed > 0)
    {
        settings->SaveConfig();

        UE_LOG(LogGameSaveDeleteManager, Log,
            TEXT("[SGameSaveDeleteManagerToolbar] [RemoveSlotFromSettings] Removed: %s"), *slotName);
    }
}

void SGameSaveDeleteManagerToolbar::DeleteSingleSlot(FString slotName)
{
    const UGameSaveDeleteManagerSettings* settings = GetDefault<UGameSaveDeleteManagerSettings>();
    if (!settings)
    {
        return;
    }

    const FString savePath = settings->GetResolvedSavePath();
    const FString fullPath = savePath / slotName + TEXT(".sav");

    const FString msg = FString::Printf(
        TEXT("Delete save file:\n%s\n\nThis cannot be undone."), *fullPath);

    if (!ShowConfirmationDialog(msg))
    {
        return;
    }

    if (!IFileManager::Get().FileExists(*fullPath))
    {
        FMessageDialog::Open(EAppMsgType::Ok,
            FText::Format(
                NSLOCTEXT("GameSaveDeleteManager", "SlotNotFound", "File not found:\n{0}"),
                FText::FromString(fullPath)));
        return;
    }

    if (IFileManager::Get().Delete(*fullPath, false, true))
    {
        UE_LOG(LogGameSaveDeleteManager, Log,
            TEXT("[SGameSaveDeleteManagerToolbar] [DeleteSingleSlot] Deleted: %s"), *fullPath);

        FMessageDialog::Open(EAppMsgType::Ok,
            FText::Format(
                NSLOCTEXT("GameSaveDeleteManager", "SlotDeleted", "Deleted: {0}"),
                FText::FromString(slotName)));
    }
    else
    {
        UE_LOG(LogGameSaveDeleteManager, Error,
            TEXT("[SGameSaveDeleteManagerToolbar] [DeleteSingleSlot] Failed to delete: %s"),
            *fullPath);

        FMessageDialog::Open(EAppMsgType::Ok,
            FText::Format(
                NSLOCTEXT("GameSaveDeleteManager", "SlotDeleteFailed",
                    "Failed to delete:\n{0}\n\nCheck file permissions."),
                FText::FromString(fullPath)));
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Open Plugin Settings
// ─────────────────────────────────────────────────────────────────────────────

void SGameSaveDeleteManagerToolbar::OpenPluginSettings() const
{
    ISettingsModule* settingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");
    if (!settingsModule)
    {
        UE_LOG(LogGameSaveDeleteManager, Warning,
            TEXT("[SGameSaveDeleteManagerToolbar] [OpenPluginSettings] Settings module unavailable"));
        return;
    }

    settingsModule->ShowViewer(
        FName("Project"), FName("Plugins"), FName("GameSaveDeleteManager"));
}
