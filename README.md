# 🗑️ Game Save Delete Manager

> **An Unreal Engine editor plugin that puts a one-click "Delete Saves" button right next to the Play button — so you never have to dig through the file system during development again.**

[![Unreal Engine](https://img.shields.io/badge/Unreal%20Engine-5.7-0e1128?logo=unrealengine&logoColor=white)](https://www.unrealengine.com/)
[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)
[![Version](https://img.shields.io/github/v/release/Vinnava/GameSaveDeleteManager?label=release)](https://github.com/Vinnava/GameSaveDeleteManager/releases)
[![Platform](https://img.shields.io/badge/platform-Windows%20%7C%20macOS%20%7C%20Linux-lightgrey)](https://github.com/Vinnava/GameSaveDeleteManager)

---

## 📖 Overview

**Game Save Delete Manager** is an editor-only plugin for Unreal Engine. It adds a compact **Del Saves** button to the Level Editor toolbar (anchored right after the Play group) so developers can wipe save game files between test runs without leaving the editor.

The dropdown arrow reveals every `.sav` file discovered in your project's save directory — giving you one-click individual slot deletion, live tracking of which slots are targeted, and a direct shortcut into the plugin's Project Settings page.

---

## ✨ Features

| Feature | Description |
|---|---|
| **Toolbar Button** | One-click delete anchored after the Play button in the Level Editor |
| **Delete All Mode** | Wipes every `.sav` file in the configured directory |
| **Selective Mode** | Targets only the slot names you specify |
| **Auto-Discovery** | Dropdown scans the save directory live and lists all discovered slots |
| **Per-Slot Actions** | Delete a single slot immediately, or add/remove it from the tracked list |
| **Confirmation Dialog** | Always asks before destructive operations — no accidental wipes |
| **Custom Save Path** | Override the default `Saved/SaveGames/` directory per-project |
| **Project Settings Integration** | Full configuration via **Project Settings → Plugins → Game Save Delete Manager** |
| **Result Feedback** | Post-delete dialog reports how many files were deleted, skipped, or failed |
| **Output Log** | All operations logged under `LogGameSaveDeleteManager` for full traceability |

---

## 🚀 Installation

### Option A — Copy into your project (recommended)

1. Download the latest `.zip` from the [Releases](https://github.com/Vinnava/GameSaveDeleteManager/releases) page.
2. Extract and copy the `GameSaveDeleteManager` folder into your project's `Plugins/` directory.
   ```
   YourProject/
   └── Plugins/
       └── GameSaveDeleteManager/
           ├── GameSaveDeleteManager.uplugin
           └── Source/
   ```
3. Right-click your `.uproject` file → **Generate Visual Studio project files**.
4. Open the project. Unreal will prompt you to build the new plugin — click **Yes**.
5. The **Del Saves** button will appear in the Level Editor toolbar.

### Option B — Engine plugins folder

Place the folder under `{UnrealEngine}/Engine/Plugins/Editor/GameSaveDeleteManager/` instead, then rebuild the engine.

> **Note:** This is an **Editor-only** plugin. It does not ship with packaged builds.

---

## ⚙️ Configuration

Open **Edit → Project Settings → Plugins → Game Save Delete Manager**.

| Setting | Type | Default | Description |
|---|---|---|---|
| **Delete All Saves on Click** | `bool` | `true` | When enabled, the button deletes every `.sav` file in the save path |
| **Save Slots to Delete** | `TArray<FString>` | _(empty)_ | Slot names to target when "Delete All" is off. Omit the `.sav` extension. Example: `PlayerSave`, `SettingsSave` |
| **Custom Save Path** | `FDirectoryPath` | _(empty)_ | Override the default save directory. Leave blank to use `[Project]/Saved/SaveGames/` |

Settings are stored per-project in `EditorPerProjectUserSettings` and are **not** committed to source control by default.

---

## 🛠️ Usage

### Delete all saves
1. Ensure **Delete All Saves on Click** is enabled in Project Settings.
2. Click the **Del Saves** button in the toolbar.
3. Confirm the dialog → all `.sav` files in the configured path are deleted.

### Delete specific slots
1. Disable **Delete All Saves on Click** in Project Settings.
2. Add your slot names to **Save Slots to Delete** (e.g. `PlayerSave`).
3. Click **Del Saves** → only the listed slots are targeted.

### Using the dropdown
Click the **arrow** on the right side of the button to open the dropdown:

- **Delete All / Delete Selected Saves Now** — same as clicking the main button.
- **Mode indicator** — shows the current operating mode at a glance.
- **Discovered Save Slots** — lists every `.sav` file found in the save path.
  - Click any slot to **delete it immediately** (with confirmation).
  - In selective mode, use **Add to list / Remove from list** to manage tracked slots.
- **Open Settings** — jumps directly to the plugin's Project Settings page.

---

## 🔧 Requirements

- **Unreal Engine 5.x** (developed and tested on UE 5.3+)
- **C++ project** — a Blueprint-only project must be converted to C++ first
- Target platform: **Editor only** (Windows, macOS, Linux)

---

## 📁 Project Structure

```
GameSaveDeleteManager/
├── GameSaveDeleteManager.uplugin
└── Source/
    └── GameSaveDeleteManager/
        ├── GameSaveDeleteManager.Build.cs
        ├── Public/
        │   ├── GameSaveDeleteManager.h          # Module class — toolbar registration
        │   ├── GameSaveDeleteManagerSettings.h  # UDeveloperSettings config class
        │   └── SGameSaveDeleteManagerToolbar.h  # Slate widget declaration
        └── Private/
            ├── GameSaveDeleteManager.cpp         # Module startup / shutdown
            ├── GameSaveDeleteManagerSettings.cpp # Path resolution logic
            └── SGameSaveDeleteManagerToolbar.cpp # Toolbar UI + delete logic
```

---

## 🐛 Troubleshooting

**The button doesn't appear in the toolbar**
- Confirm the plugin is enabled in **Edit → Plugins → Editor → Game Save Delete Manager**.
- Rebuild the project after enabling.

**No saves are found in the dropdown**
- Play the game at least once so save files are generated.
- Check the configured save path in Project Settings matches where your game writes saves.

**Delete failed for a file**
- The output log (`LogGameSaveDeleteManager`) will show the full path that failed.
- Check file permissions on the save directory.

---

## 🤝 Contributing

Contributions are welcome! Please open an issue first to discuss any significant changes.

1. Fork the repository
2. Create a feature branch: `git checkout -b feature/your-feature`
3. Commit your changes: `git commit -m "feat: add your feature"`
4. Push to the branch: `git push origin feature/your-feature`
5. Open a Pull Request

---

## 📄 License

This project is licensed under the **MIT License** — see the [LICENSE](LICENSE) file for details.

---

## 👤 Author

**Vinnava**
- GitHub: [@Vinnava](https://github.com/Vinnava)

---

_If this plugin saved you time, consider leaving a ⭐ on the repository._
