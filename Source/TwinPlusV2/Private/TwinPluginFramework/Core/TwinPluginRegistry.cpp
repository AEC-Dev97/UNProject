#include "TwinPluginFramework/Core/TwinPluginRegistry.h"

FRegisteredTool UTwinPluginRegistry::FindTool(const FString& ToolID) const
{
    for (const FRegisteredTool& Tool : RegisteredTools)
    {
        if (Tool.ToolID == ToolID)
        {
            return Tool;
        }
    }

    return FRegisteredTool();
}

TArray<FRegisteredTool> UTwinPluginRegistry::GetToolsByCategory(EToolCategory Category) const
{
    TArray<FRegisteredTool> CategoryTools;
    for (const FRegisteredTool& Tool : RegisteredTools)
    {
        if (Tool.Metadata.Category == Category)
        {
            CategoryTools.Add(Tool);
        }
    }
    return CategoryTools;
}

TArray<FRegisteredTool> UTwinPluginRegistry::GetToolsForMode(EAppMode Mode) const
{
    TArray<FRegisteredTool> ModeTools;
    for (const FRegisteredTool& Tool : RegisteredTools)
    {
        if (Tool.Metadata.SupportedModes.Contains(Mode) || Tool.Metadata.SupportedModes.IsEmpty())
        {
            ModeTools.Add(Tool);
        }
    }
    return ModeTools;
}

void UTwinPluginRegistry::RegisterTool(const FRegisteredTool& Tool)
{
    for (FRegisteredTool& ExistingTool : RegisteredTools)
    {
        if (ExistingTool.ToolID == Tool.ToolID)
        {
            ExistingTool = Tool;
            return;
        }
    }
    RegisteredTools.Add(Tool);
}

bool UTwinPluginRegistry::UnregisterTool(const FString& ToolID)
{
    for (int32 i = RegisteredTools.Num() - 1; i >= 0; i--)
    {
        if (RegisteredTools[i].ToolID == ToolID)
        {
            RegisteredTools.RemoveAt(i);
            return true;
        }
    }
    return false;
}