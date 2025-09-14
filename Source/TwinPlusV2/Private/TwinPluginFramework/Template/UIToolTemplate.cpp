#include "TwinPluginFramework/Template/UIToolTemplate.h"
#include "Subsystems/GlobalServices.h"
#include "Subsystems/UIManagerSubsystem.h"
#include "Blueprint/UserWidget.h"
#include "Core/Logs.h"

UUIToolTemplate::UUIToolTemplate()
{
    // UI-specific defaults
    TargetUILayer = EUILayer::Menu;
    bAutoShowUI = true;
    bHideOnModeChange = true;

    // Update tool metadata
    ToolMetadata.ToolName = TEXT("UI Tool");
    ToolMetadata.Description = TEXT("A UI-based tool template");
    ToolMetadata.Category = EToolCategory::UI;
}

void UUIToolTemplate::ActivateTool_Implementation()
{
    Super::ActivateTool_Implementation();

    if (bAutoShowUI)
    {
        ShowToolUI();
    }
}

void UUIToolTemplate::DeactivateTool_Implementation()
{
    if (bUIVisible)
    {
        HideToolUI();
    }

    Super::DeactivateTool_Implementation();
}

UUserWidget* UUIToolTemplate::CreateToolWidget_Implementation()
{
    // Check if we have mode-specific widgets
    if (ModeSpecificWidgets.Contains(CurrentMode))
    {
        TSoftClassPtr<UUserWidget> ModeWidget = ModeSpecificWidgets[CurrentMode];
        if (ModeWidget.IsValid())
        {
            if (UClass* WidgetClass = ModeWidget.LoadSynchronous())
            {
                return CreateWidget<UUserWidget>(GetWorld(), WidgetClass);
            }
        }
    }

    // Fall back to default widget
    return Super::CreateToolWidget_Implementation();
}

void UUIToolTemplate::OnModeChanged_Implementation(EAppMode NewMode)
{
    if (bHideOnModeChange && bUIVisible)
    {
        HideToolUI();
    }

    Super::OnModeChanged_Implementation(NewMode);

    if (bIsActive)
    {
        UpdateUIForMode(NewMode);

        if (bAutoShowUI && CanActivateInMode_Implementation(NewMode))
        {
            ShowToolUI();
        }
    }
}

void UUIToolTemplate::ShowToolUI()
{
    if (bUIVisible)
    {
        return;
    }

    UUserWidget* Widget = CreateToolWidget_Implementation();
    if (!Widget)
    {
        UE_LOG(LogMode, Warning, TEXT("No widget available for tool: %s"), *ToolMetadata.ToolName);
        return;
    }

    if (UUIManagerSubsystem* UIManager = GetUIManager())
    {
        // Create a unique UI name for this tool instance
        FName UIName = FName(*FString::Printf(TEXT("Tool_%s"), *ToolMetadata.ToolName));
        UIManager->PushUI(UIName, TargetUILayer);

        bUIVisible = true;
        OnUIShown();
        OnUIWidgetCreated(Widget);

        UE_LOG(LogMode, Log, TEXT("Showed UI for tool: %s"), *ToolMetadata.ToolName);
    }
}

void UUIToolTemplate::HideToolUI()
{
    if (!bUIVisible)
    {
        return;
    }

    if (UUIManagerSubsystem* UIManager = GetUIManager())
    {
        UIManager->PopUI(TargetUILayer);
        bUIVisible = false;
        OnUIHidden();

        UE_LOG(LogMode, Log, TEXT("Hidden UI for tool: %s"), *ToolMetadata.ToolName);
    }
}

void UUIToolTemplate::RefreshUI()
{
    if (bUIVisible)
    {
        HideToolUI();
        ShowToolUI();
    }
}

bool UUIToolTemplate::IsUIVisible() const
{
    return bUIVisible;
}

void UUIToolTemplate::UpdateUIForMode(EAppMode Mode)
{
    if (bUIVisible && ModeSpecificWidgets.Contains(Mode))
    {
        // Refresh UI with mode-specific widget
        RefreshUI();
    }
}