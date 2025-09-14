#include "TwinPluginFramework/Core/TwinPluginManager.h"
#include "Subsystems/GlobalServices.h"
#include "Subsystems/ModeManagerSubsystem.h"
#include "Core/Logs.h"

void UTwinPluginManager::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    // Load plugin registry
    FSoftObjectPath RegistryPath(TEXT("/Game/Config/PluginRegistry.PluginRegistry"));
    if (UObject* LoadedAsset = RegistryPath.TryLoad())
    {
        PluginRegistry = Cast<UTwinPluginRegistry>(LoadedAsset);
    }

    if (!PluginRegistry)
    {
        UE_LOG(LogMode, Warning, TEXT("PluginRegistry not found, creating default"));
        PluginRegistry = NewObject<UTwinPluginRegistry>(this);
    }

    // Listen to mode changes
    if (UModeManagerSubsystem* ModeManager = UGlobalServices::GetModeManager(this))
    {
        ModeManager->OnModeChanged.AddDynamic(this, &UTwinPluginManager::OnModeChanged);
    }

    LoadRegisteredTools();
}

void UTwinPluginManager::LoadRegisteredTools()
{
    if (!PluginRegistry) return;

    for (const FRegisteredTool& Tool : PluginRegistry->RegisteredTools)
    {
        if (Tool.bEnabled && Tool.bAutoLoad)
        {
            if (UClass* ToolClass = Tool.ToolClass.LoadSynchronous())
            {
                LoadedTools.Add(Tool.ToolID, NewObject<UObject>(this, ToolClass));
                UE_LOG(LogMode, Log, TEXT("Auto-loaded tool: %s"), *Tool.ToolID);
            }
        }
    }
}

bool UTwinPluginManager::RegisterTool(const FString& ToolID, TSubclassOf<UObject> ToolClass, const FToolMetadata& Metadata)
{
    if (!ValidateToolClass(ToolClass))
    {
        UE_LOG(LogMode, Error, TEXT("Invalid tool class for %s"), *ToolID);
        return false;
    }

    FRegisteredTool NewTool;
    NewTool.ToolID = ToolID;
    NewTool.ToolClass = ToolClass;
    NewTool.Metadata = Metadata;
    NewTool.bEnabled = true;

    PluginRegistry->RegisterTool(NewTool);
    OnToolRegistered.Broadcast(ToolID);

    return true;
}

TScriptInterface<ITwinToolInterface> UTwinPluginManager::CreateTool(const FString& ToolID)
{
    FRegisteredTool Tool = PluginRegistry->FindTool(ToolID);

    if (Tool.ToolID.IsEmpty() || !Tool.bEnabled)
    {
        return nullptr;
    }


    if (UObject* ExistingTool = LoadedTools.FindRef(ToolID))
    {
        if (ExistingTool->GetClass()->ImplementsInterface(UTwinToolInterface::StaticClass()))
        {
            return TScriptInterface<ITwinToolInterface>(ExistingTool);
        }
    }


    if (UClass* ToolClass = Tool.ToolClass.LoadSynchronous())
    {
        UObject* NewTool = NewObject<UObject>(this, ToolClass);
        LoadedTools.Add(ToolID, NewTool);

        if (NewTool->GetClass()->ImplementsInterface(UTwinToolInterface::StaticClass()))
        {
            TScriptInterface<ITwinToolInterface> ToolInterface(NewTool);
            ITwinToolInterface::Execute_InitializeTool(NewTool);
            return ToolInterface;
        }
    }

    return nullptr;
}

bool UTwinPluginManager::ActivateTool(const FString& ToolID)
{
    TScriptInterface<ITwinToolInterface> Tool = CreateTool(ToolID);
    if (!Tool.GetInterface())
    {
        return false;
    }

    // Check if tool can activate in current mode
    //if (!ITwinToolInterface::Execute_CanActivateInMode(Tool.GetObject(), CurrentMode))
    //{
    //    UE_LOG(LogMode, Warning, TEXT("Tool %s cannot activate in current mode"), *ToolID);
    //    return false;
    //}

    // Deactivate conflicting tools if needed
    // (Implementation depends on your tool conflict rules)

    ITwinToolInterface::Execute_ActivateTool(Tool.GetObject());
    ActiveTools.Add(ToolID, Tool);
    OnToolActivated.Broadcast(ToolID, Tool);

    return true;
}

void UTwinPluginManager::OnModeChanged(EAppMode NewMode)
{
    CurrentMode = NewMode;

    // Notify all active tools about mode change
    for (auto& ToolPair : ActiveTools)
    {
        if (ToolPair.Value.GetInterface())
        {
            ITwinToolInterface::Execute_OnModeChanged(ToolPair.Value.GetObject(), NewMode);
        }
    }

    // Deactivate tools that don't support the new mode
    TArray<FString> ToolsToDeactivate;
    for (auto& ToolPair : ActiveTools)
    {
        if (ToolPair.Value.GetInterface())
        {
            if (!ITwinToolInterface::Execute_CanActivateInMode(ToolPair.Value.GetObject(), NewMode))
            {
                ToolsToDeactivate.Add(ToolPair.Key);
            }
        }
    }

    for (const FString& ToolID : ToolsToDeactivate)
    {
        DeactivateTool(ToolID);
    }
}

void UTwinPluginManager::DeactivateTool(const FString& ToolID)
{
    TScriptInterface<ITwinToolInterface>* FoundTool = ActiveTools.Find(ToolID);
    if (!FoundTool || !FoundTool->GetInterface())
    {
        return;
    }

    ITwinToolInterface::Execute_DeactivateTool(FoundTool->GetObject());
    ActiveTools.Remove(ToolID);
    OnToolDeactivated.Broadcast(ToolID);

    UE_LOG(LogMode, Log, TEXT("Deactivated tool: %s"), *ToolID);
}

void UTwinPluginManager::DeactivateAllTools()
{
    TArray<FString> ToolsToDeactivate;
    ActiveTools.GetKeys(ToolsToDeactivate);

    for (const FString& ToolID : ToolsToDeactivate)
    {
        DeactivateTool(ToolID);
    }
}

TArray<FString> UTwinPluginManager::GetAvailableTools() const
{
    TArray<FString> AvailableTools;

    if (PluginRegistry)
    {
        for (const FRegisteredTool& Tool : PluginRegistry->RegisteredTools)
        {
            if (Tool.bEnabled)
            {
                AvailableTools.Add(Tool.ToolID);
            }
        }
    }

    return AvailableTools;
}

TArray<FString> UTwinPluginManager::GetActiveTools() const
{
    TArray<FString> ActiveToolIDs;
    ActiveTools.GetKeys(ActiveToolIDs);
    return ActiveToolIDs;
}

TArray<FString> UTwinPluginManager::GetToolsByCategory(EToolCategory Category) const
{
    TArray<FString> CategoryTools;

    if (PluginRegistry)
    {
        for (const FRegisteredTool& Tool : PluginRegistry->RegisteredTools)
        {
            if (Tool.bEnabled && Tool.Metadata.Category == Category)
            {
                CategoryTools.Add(Tool.ToolID);
            }
        }
    }

    return CategoryTools;
}

TScriptInterface<ITwinToolInterface> UTwinPluginManager::GetActiveTool(const FString& ToolID) const
{
    if (const TScriptInterface<ITwinToolInterface>* FoundTool = ActiveTools.Find(ToolID))
    {
        return *FoundTool;
    }

    return TScriptInterface<ITwinToolInterface>();
}

FToolMetadata UTwinPluginManager::GetToolMetadata(const FString& ToolID) const
{
    UE_LOG(LogMode, Warning, TEXT("GetToolMetadata called for: %s"), *ToolID);

    if (!PluginRegistry)
    {
        UE_LOG(LogMode, Error, TEXT("PluginRegistry is NULL"));
        return FToolMetadata();
    }

    // Use the same pattern as GetAvailableTools (which works!)
    for (const FRegisteredTool& Tool : PluginRegistry->RegisteredTools)
    {
        UE_LOG(LogMode, Warning, TEXT("Checking tool: '%s' against '%s'"), *Tool.ToolID, *ToolID);

        if (Tool.ToolID == ToolID)
        {
            UE_LOG(LogMode, Warning, TEXT("FOUND MATCH! Returning metadata for: %s"), *Tool.ToolID);
            UE_LOG(LogMode, Warning, TEXT("  - Name: %s"), *Tool.Metadata.ToolName);
            UE_LOG(LogMode, Warning, TEXT("  - Description: %s"), *Tool.Metadata.Description);
            return Tool.Metadata;
        }
    }

    UE_LOG(LogMode, Error, TEXT("Tool '%s' not found in %d registered tools"), *ToolID, PluginRegistry->RegisteredTools.Num());
    return FToolMetadata();
}

bool UTwinPluginManager::IsToolActive(const FString& ToolID) const
{
    return ActiveTools.Contains(ToolID);
}

bool UTwinPluginManager::IsToolAvailable(const FString& ToolID) const
{
    FRegisteredTool Tool = PluginRegistry->FindTool(ToolID);
    if (!Tool.ToolID.IsEmpty())
    {
        return Tool.bEnabled;
    }
    return false;

}

TArray<FString> UTwinPluginManager::GetToolsForCurrentMode() const
{
    TArray<FString> ModeTools;

    if (PluginRegistry)
    {
        for (const FRegisteredTool& Tool : PluginRegistry->RegisteredTools)
        {
            if (Tool.bEnabled)
            {
                // If tool has no specific modes, it works in all modes
                if (Tool.Metadata.SupportedModes.IsEmpty() ||
                    Tool.Metadata.SupportedModes.Contains(CurrentMode))
                {
                    ModeTools.Add(Tool.ToolID);
                }
            }
        }
    }

    return ModeTools;
}

bool UTwinPluginManager::ValidateToolClass(TSubclassOf<UObject> ToolClass) const
{
    if (!ToolClass)
    {
        return false;
    }

    // Check if class implements ITwinToolInterface
    return ToolClass->ImplementsInterface(UTwinToolInterface::StaticClass());
}

void UTwinPluginManager::UnloadAllTools()
{
    DeactivateAllTools();

    // Shutdown all loaded tools
    for (auto& ToolPair : LoadedTools)
    {
        if (UObject* Tool = ToolPair.Value)
        {
            if (Tool->GetClass()->ImplementsInterface(UTwinToolInterface::StaticClass()))
            {
                ITwinToolInterface::Execute_ShutdownTool(Tool);
            }
        }
    }

    LoadedTools.Empty();
}

void UTwinPluginManager::Deinitialize()
{
    UnloadAllTools();

    // Remove mode manager delegate
    if (UModeManagerSubsystem* ModeManager = UGlobalServices::GetModeManager(this))
    {
        ModeManager->OnModeChanged.RemoveDynamic(this, &UTwinPluginManager::OnModeChanged);
    }

    Super::Deinitialize();
}

bool UTwinPluginManager::UnregisterTool(const FString& ToolID)
{
    if (!PluginRegistry)
    {
        return false;
    }

    // Remove from registry
    bool bUnregistered = PluginRegistry->UnregisterTool(ToolID);

    if (bUnregistered)
    {
        // Deactivate if currently active
        if (ActiveTools.Contains(ToolID))
        {
            DeactivateTool(ToolID);
        }

        // Remove from loaded tools
        LoadedTools.Remove(ToolID);

        OnToolUnregistered.Broadcast(ToolID);
        UE_LOG(LogMode, Log, TEXT("Unregistered tool: %s"), *ToolID);
    }

    return bUnregistered;
}

bool UTwinPluginManager::ActivateToolCommand(const FString& ToolID)
{
    bool bSuccess = ActivateTool(ToolID);

    if (bSuccess)
    {
        UE_LOG(LogMode, Log, TEXT("Console: Successfully activated tool '%s'"), *ToolID);
    }
    else
    {
        UE_LOG(LogMode, Error, TEXT("Console: Failed to activate tool '%s'"), *ToolID);
    }

    return bSuccess;
}

bool UTwinPluginManager::DeactivateToolCommand(const FString& ToolID)
{
    if (ActiveTools.Contains(ToolID))
    {
        DeactivateTool(ToolID);
        UE_LOG(LogMode, Log, TEXT("Console: Successfully deactivated tool '%s'"), *ToolID);
        return true;
    }
    else
    {
        UE_LOG(LogMode, Warning, TEXT("Console: Tool '%s' is not currently active"), *ToolID);
        return false;
    }
}

void UTwinPluginManager::ListAvailableToolsCommand()
{
    TArray<FString> AvailableTools = GetAvailableTools();
    TArray<FString> ActiveToolsList = GetActiveTools();

    UE_LOG(LogMode, Log, TEXT("=== TWIN FRAMEWORK TOOLS ==="));
    UE_LOG(LogMode, Log, TEXT("Available Tools (%d):"), AvailableTools.Num());

    for (const FString& Tool : AvailableTools)
    {
        bool bIsActive = ActiveToolsList.Contains(Tool);
        FString Status = bIsActive ? TEXT("[ACTIVE]") : TEXT("[INACTIVE]");
        UE_LOG(LogMode, Log, TEXT("  - %s %s"), *Tool, *Status);
    }

    UE_LOG(LogMode, Log, TEXT("============================="));
}