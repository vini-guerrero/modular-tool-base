# Overview

For this project, I decided to prepare an easy extendable structure so that it could scale with new EditorTabs or Menu subsection categories. Therefore I choose a custom interface for registering multiple modules and MineSweeper implemented as one of them.
The plugin features also two sample slate widgets, one for the acctual minesweeper game and one for general about UI.
Additional implementations cover a helper class for in editor slate notifications and a custom string table handler for exposing tool UI texts to blueprints for easier and faster text labels manipulation / prototyping.

# Implementation

### Development Steps
- Create Game and Plugin Project Solution
- Plan Plugin Entry Point File and Its Requirements
- Analyze Required Dependencies in Build.cs
- Adjust Plugin Load Phase in .uplugin file
- Design Nomad Tab Interface and Custom Module Classes
- Implement Slate Widgets (About Window, MineSweeper UI)
- Integrate String Tables and Notification Helpers
- Register Tabs with FGlobalTabmanager and Connect to Menu
- Implement Tab and Widget Memory Management (TSharedPtr / TWeakPtr)
- Test Plugin Workflow and Editor Integration

### NomadTabs

I built a custom Nomad tab and module interface to give each feature its own dedicated space inside the editor, rather than relying solely on menu entries or pop-ups. This improves usability by grouping related tools under consistent, discoverable tabs, and provides a scalable way to add or remove modules without cluttering the editor UI.

![Tabs](/Screenshots/tabs.png "Tabs")

Each tool module implements `IMTBCustomModule`, which plugs into the `IMTBModuleInterface` lifecycle for startup and shutdown. Tabs are registered with `FGlobalTabmanager` via `FMTBNomadTab`, which manages their spawner, menu entry, and cleanup. This ensures memory-safe tab handling, unified initialization across modules, and a consistent integration pattern for all plugin features.

### MineSweeper Tool

A dedicated Nomad tab was used to integrate the tool directly into the Unreal Editor workspace. This makes the plugin feel seamless within the Unreal Editor, while keeping layout and interaction entirely under our control, make use of editor's docking system, and remain accessible alongside native panels.

![MineSweeper](/Screenshots/minesweeper.png "MineSweeper")

Memory and lifecycle of the tab and its Slate widget are managed via reference counting `(TSharedRef/TSharedPtr)`. I've stored a TWeakPtr to the `SDockTab` for optional programmatic access, and on module shutdown we call `RequestCloseTab()` on any open instance before resetting the pointer. This ensures Slate properly destroys the tab and all child widgets, while `UnregisterNomadTabSpawner` prevents new tabs from being spawned without leaving dangling references.

### Utils

I decided to use string tables for all UI labels and editor notifications to keep text consistent, centralized, and easily editable. This avoids hard-coded strings in Slate widgets, supports localization out of the box, and ensures that status updates or notifications always use the same wording across the plugin.

![StringTable](/Screenshots/stringtable.png "StringTable")

Labels and messages are retrieved through a helper `(MTBUtils::GetMineSweeperText)` that wraps `FText::FromStringTable`. The `FMineSweeperTexts` struct caches the common entries for Slate widgets, while the notification helper formats and displays messages via `FSlateNotificationManager`. This keeps text lookup lightweight, memory-safe, and editor-native.

## External References

### AI

AI was used to assist in resolving correct #include paths for different Unreal Engine versions, structuring Slate widget logic efficiently, and implementing game logic patterns such as the BFS flood-fill algorithm for revealing empty Minesweeper tiles. This helped speed up development, ensured correct header usage, and maintained robust, editor-friendly UI behavior.

### Articles

Inspired by Eric Zhang's How to Make Tools in UE4 (2019), which guided general integration concepts, adapting ideas to a Nomad tab workflow, Slate widgets, and Minesweeper-specific game logic on my own project.

## Project Structure

### MTB - Modular Tool Base (Plugin)

```core.xml
/MTB
│
├── MTB                           # Plugin Entry Point / Core Menus Registration
├── MTBNomadTab                   # Custom Nomad Tab Module with General Helpers
├── MTBUtils                      # Slate / Editor Related Reusable Helpers
│
├── Interfaces/
│   ├── IMTBModuleInterface       # Extensible Module Listeners
│
├── MineSweeper/
│   ├── MineSweeper/              # Custom Nomad Tab Entry Point
│   ├── SMineSweeperWidget        # Custom Slate Widget
│   └── SAboutMineSweeperWidget   # Custom Slate Widget
└──
```

## Plugin Loading Phase

```uplugin.json
{
  "Name": "MTB",
  "Type": "Editor",
  "LoadingPhase": "PostEngineInit",
  "AdditionalDependencies": [
    "Engine"
  ]
}
```

## Plugin Build Dependencies Used
- `"InputCore", "UnrealEd", "EditorStyle"` - Custom Slate Widgets
- `"DeveloperSettings", "Projects"` - Plugin About (Dynamic Pull)

## Other Development Implementations

### About

I created this About window to give users a clear and accessible place to see the plugin’s purpose, version, and authorship.
It helps maintain transparency and professionalism while keeping information directly inside the editor, ensuring users can always see the latest plugin details.

![About](/Screenshots/about.png "About")

We keep a `TSharedPtr<SWindow> (AboutWindowPtr)` to manage the About window’s lifetime.
Before opening a new one, plugin checks validity and explicitly call `RequestDestroyWindow()` followed by `Reset()` to release resources.
On module shutdown, plugin also resets it, ensuring no dangling references remain and memory is fully reclaimed by Slate.

### RefreshPlugins Bat

I used a custom script to quickly clear plugin caches and enable live reload, speeding up iteration and avoiding stale build artifacts.

The script iterates over all plugin subdirectories and deletes Binaries, Intermediate, and Saved folders using RD /S /Q. It operates relative to the current directory, making it portable across projects. This approach ensures that the next build starts from a clean state, while keeping the script simple and safe to run without affecting unrelated files.

## Plugin Development Environment
- Software - `Unreal Engine 5.5, VSCode Solution`
- Hardware - `Intel i9 2.5GHz, GPU 3050, 16gb RAM, 500gb Storage`
- Special Thanks - `My co-purgrammer cat 😼`
