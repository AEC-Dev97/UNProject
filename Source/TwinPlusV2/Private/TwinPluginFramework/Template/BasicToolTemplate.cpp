#include "TwinPluginFramework/Template/BasicToolTemplate.h"
#include "Subsystems/GlobalServices.h"
#include "TwinPluginFramework/Core/TwinPluginManager.h"
#include "Subsystems/UIManagerSubsystem.h"
#include "Subsystems/ModeManagerSubsystem.h"
#include "Blueprint/UserWidget.h"
#include "Core/Logs.h"

UBasicToolTemplate::UBasicToolTemplate()
{
    // Set default metadata
    ToolMetadata.ToolName = TEXT("Basic Tool");
    ToolMetadata.Description = TEXT("A basic tool template");
    ToolMetadata.Version = TEXT("1.0.0");
    ToolMetadata.Author = TEXT("Twin Framework");
    ToolMetadata.Category = EToolCategory::Custom;
    ToolMetadata.Scope = EToolScope::Global;
    ToolMetadata.Priority = 100;
    ToolMetadata.bRequiresScene = false;
}

bool UBasicToolTemplate::InitializeTool_Implementation()
{
    if (bIsInitialized)
    {
        return true;
    }

    UE_LOG(LogMode, Log, TEXT("Initializing tool: %s"), *ToolMetadata.ToolName);

    bIsInitialized = true;
    OnToolInitialized();

    return true;
}

void UBasicToolTemplate::ActivateTool_Implementation()
{
    if (!bIsInitialized)
    {
        if (!InitializeTool_Implementation())
        {
            UE_LOG(LogMode, Error, TEXT("Failed to initialize tool: %s"), *ToolMetadata.ToolName);
            return;
        }
    }

    if (bIsActive)
    {
        UE_LOG(LogMode, Warning, TEXT("Tool already active: %s"), *ToolMetadata.ToolName);
        return;
    }

    bIsActive = true;
    UE_LOG(LogMode, Log, TEXT("Activating tool: %s"), *ToolMetadata.ToolName);

    BroadcastStateChanged(true);
    OnToolActivated();
}

void UBasicToolTemplate::DeactivateTool_Implementation()
{
    if (!bIsActive)
    {
        return;
    }

    bIsActive = false;
    UE_LOG(LogMode, Log, TEXT("Deactivating tool: %s"), *ToolMetadata.ToolName);

    OnToolDeactivated();
    BroadcastStateChanged(false);
}

void UBasicToolTemplate::ShutdownTool_Implementation()
{
    if (bIsActive)
    {
        DeactivateTool_Implementation();
    }

    if (!bIsInitialized)
    {
        return;
    }

    UE_LOG(LogMode, Log, TEXT("Shutting down tool: %s"), *ToolMetadata.ToolName);

    // Clean up cached widget
    if (CachedWidget)
    {
        CachedWidget->RemoveFromParent();
        CachedWidget = nullptr;
    }

    bIsInitialized = false;
    OnToolShutdown();
}

FToolMetadata UBasicToolTemplate::GetToolMetadata_Implementation() const
{
    return ToolMetadata;
}

bool UBasicToolTemplate::IsToolActive_Implementation() const
{
    return bIsActive;
}

bool UBasicToolTemplate::CanActivateInMode_Implementation(EAppMode Mode) const
{
    // If no supported modes are specified, tool can activate in any mode
    if (ToolMetadata.SupportedModes.IsEmpty())
    {
        return true;
    }

    return ToolMetadata.SupportedModes.Contains(Mode);
}

UUserWidget* UBasicToolTemplate::CreateToolWidget_Implementation()
{
    if (CachedWidget)
    {
        return CachedWidget;
    }

    if (ToolWidgetClass.IsValid())
    {
        if (UClass* WidgetClass = ToolWidgetClass.LoadSynchronous())
        {
            CachedWidget = CreateWidget<UUserWidget>(GetWorld(), WidgetClass);
            if (CachedWidget)
            {
                UE_LOG(LogMode, Log, TEXT("Created tool widget for: %s"), *ToolMetadata.ToolName);
            }
        }
    }

    return CachedWidget;
}

void UBasicToolTemplate::OnModeChanged_Implementation(EAppMode NewMode)
{
    CurrentMode = NewMode;
    OnToolModeChanged(NewMode);

    // Deactivate tool if it doesn't support the new mode
    if (bIsActive && !CanActivateInMode_Implementation(NewMode))
    {
        UE_LOG(LogMode, Log, TEXT("Deactivating tool %s - not supported in mode %d"),
            *ToolMetadata.ToolName, static_cast<int32>(NewMode));
        DeactivateTool_Implementation();
    }
}

void UBasicToolTemplate::BroadcastProgress(float Progress, const FString& Message)
{
    OnToolProgress.Broadcast(this, Progress, Message);
}

void UBasicToolTemplate::BroadcastStateChanged(bool bActive)
{
    OnToolStateChanged.Broadcast(this, bActive);
}

UTwinPluginManager* UBasicToolTemplate::GetPluginManager() const
{
    return UGlobalServices::GetPluginManager(this);
}

UUIManagerSubsystem* UBasicToolTemplate::GetUIManager() const
{
    return UGlobalServices::GetUIManager(this);
}

UModeManagerSubsystem* UBasicToolTemplate::GetModeManager() const
{
    return UGlobalServices::GetModeManager(this);
}