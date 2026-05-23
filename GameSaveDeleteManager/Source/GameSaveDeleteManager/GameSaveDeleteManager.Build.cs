using UnrealBuildTool;

public class GameSaveDeleteManager : ModuleRules
{
    public GameSaveDeleteManager(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core"
        });

        PrivateDependencyModuleNames.AddRange(new string[]
        {
            "CoreUObject",
            "Engine",
            "Slate",
            "SlateCore",
            "EditorStyle",
            "EditorFramework",
            "UnrealEd",
            "LevelEditor",
            "ToolMenus",
            "DeveloperSettings",
            "InputCore"
        });
    }
}
