#pragma once
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TwinPluginFramework/Core/TwinToolInterface.h"
#include "BasicToolTemplate.generated.h"

/**
 * Basic Tool Template - Use this as a starting point for simple tools
 *
 * Instructions:
 * 1. Inherit from this class
 * 2. Override GetToolMetadata() to provide tool information
 * 3. Implement ActivateTool() and DeactivateTool() for your tool logic
 * 4. Optionally override other virtual functions as needed
 */
 // Delegate to report tool progress (e.g., 0.0 to 1.0 with a message)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FToolProgressDelegate, UObject*, Tool, float, Progress, const FString&, Message);

// Delegate to report state changes (active/inactive)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FToolStateChangedDelegate, UObject*, Tool, bool, bIsActive);


UCLASS(BlueprintType, Blueprintable, Abstract)
class TWINPLUSV2_API UBasicToolTemplate : public UObject, public ITwinToolInterface
{
    GENERATED_BODY()

public:
    UBasicToolTemplate();

    // ITwinToolInterface Implementation
    virtual bool InitializeTool_Implementation();
    virtual void ActivateTool_Implementation() ;
    virtual void DeactivateTool_Implementation();
    virtual void ShutdownTool_Implementation();
    virtual FToolMetadata GetToolMetadata_Implementation() const;
    virtual bool IsToolActive_Implementation() const;
    virtual bool CanActivateInMode_Implementation(EAppMode Mode) const;
    virtual UUserWidget* CreateToolWidget_Implementation();
    virtual void OnModeChanged_Implementation(EAppMode NewMode);

protected:
    // Tool State
    UPROPERTY(BlueprintReadOnly, Category = "Tool State")
    bool bIsActive = false;

    UPROPERTY(BlueprintReadOnly, Category = "Tool State")
    bool bIsInitialized = false;

    UPROPERTY(BlueprintReadOnly, Category = "Tool State")
    EAppMode CurrentMode = EAppMode::MainMenu;

    // Tool Configuration
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tool Config")
    FToolMetadata ToolMetadata;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tool Config")
    TSoftClassPtr<UUserWidget> ToolWidgetClass;

    // Virtual Functions for Override
    UFUNCTION(BlueprintImplementableEvent, Category = "Tool Events")
    void OnToolInitialized();

    UFUNCTION(BlueprintImplementableEvent, Category = "Tool Events")
    void OnToolActivated();

    UFUNCTION(BlueprintImplementableEvent, Category = "Tool Events")
    void OnToolDeactivated();

    UFUNCTION(BlueprintImplementableEvent, Category = "Tool Events")
    void OnToolShutdown();

    UFUNCTION(BlueprintImplementableEvent, Category = "Tool Events")
    void OnToolModeChanged(EAppMode NewMode);

    // Helper Functions
    UFUNCTION(BlueprintCallable, Category = "Tool Helpers")
    void BroadcastProgress(float Progress, const FString& Message = TEXT(""));

    UFUNCTION(BlueprintCallable, Category = "Tool Helpers")
    void BroadcastStateChanged(bool bActive);

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Tool Helpers")
    class UTwinPluginManager* GetPluginManager() const;

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Tool Helpers")
    class UUIManagerSubsystem* GetUIManager() const;

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Tool Helpers")
    class UModeManagerSubsystem* GetModeManager() const;

    // Delegates
    UPROPERTY(BlueprintAssignable, Category = "Tool Events")
    FToolProgressDelegate OnToolProgress;

    UPROPERTY(BlueprintAssignable, Category = "Tool Events")
    FToolStateChangedDelegate OnToolStateChanged;


private:
    UPROPERTY()
    TObjectPtr<UUserWidget> CachedWidget;
};