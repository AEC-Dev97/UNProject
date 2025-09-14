#pragma once
#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "TwinPluginFramework/Core/TwinToolInterface.h"
#include "TwinPluginFramework/Core/TwinPluginRegistry.h"
#include "TwinPluginManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnToolRegistered, const FString&, ToolID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnToolUnregistered, const FString&, ToolID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnToolActivated, const FString&, ToolID, TScriptInterface<ITwinToolInterface>, Tool);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnToolDeactivated, const FString&, ToolID);

UCLASS(BlueprintType)
class TWINPLUSV2_API UTwinPluginManager : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    // Events
    UPROPERTY(BlueprintAssignable, Category = "Plugin Manager")
    FOnToolRegistered OnToolRegistered;

    UPROPERTY(BlueprintAssignable, Category = "Plugin Manager")
    FOnToolUnregistered OnToolUnregistered;

    UPROPERTY(BlueprintAssignable, Category = "Plugin Manager")
    FOnToolActivated OnToolActivated;

    UPROPERTY(BlueprintAssignable, Category = "Plugin Manager")
    FOnToolDeactivated OnToolDeactivated;

    // Tool Management
    UFUNCTION(BlueprintCallable, Category = "Plugin Manager")
    bool RegisterTool(const FString& ToolID, TSubclassOf<UObject> ToolClass, const FToolMetadata& Metadata);

    UFUNCTION(BlueprintCallable, Category = "Plugin Manager")
    bool UnregisterTool(const FString& ToolID);

    UFUNCTION(BlueprintCallable, Category = "Plugin Manager")
    TScriptInterface<ITwinToolInterface> CreateTool(const FString& ToolID);

    UFUNCTION(BlueprintCallable, Category = "Plugin Manager")
    bool ActivateTool(const FString& ToolID);

    UFUNCTION(BlueprintCallable, Category = "Plugin Manager")
    void DeactivateTool(const FString& ToolID);

    UFUNCTION(BlueprintCallable, Category = "Plugin Manager")
    void DeactivateAllTools();

    // Tool Queries
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Plugin Manager")
    TArray<FString> GetAvailableTools() const;

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Plugin Manager")
    TArray<FString> GetActiveTools() const;

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Plugin Manager")
    TArray<FString> GetToolsByCategory(EToolCategory Category) const;

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Plugin Manager")
    TScriptInterface<ITwinToolInterface> GetActiveTool(const FString& ToolID) const;

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Plugin Manager")
    FToolMetadata GetToolMetadata(const FString& ToolID) const;

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Plugin Manager")
    bool IsToolActive(const FString& ToolID) const;

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Plugin Manager")
    bool IsToolAvailable(const FString& ToolID) const;

    // Mode Integration
    UFUNCTION(BlueprintCallable, Category = "Plugin Manager")
    void OnModeChanged(EAppMode NewMode);

    UFUNCTION(BlueprintCallable, Category = "Plugin Manager")
    TArray<FString> GetToolsForCurrentMode() const;

    // Console Commands
    UFUNCTION(Exec, Category = "Twin Plugin")
    bool ActivateToolCommand(const FString& ToolID);

    UFUNCTION(Exec, Category = "Twin Plugin")
    bool DeactivateToolCommand(const FString& ToolID);

    UFUNCTION(Exec, Category = "Twin Plugin")
    void ListAvailableToolsCommand();

protected:
    UPROPERTY()
    TObjectPtr<UTwinPluginRegistry> PluginRegistry;

    UPROPERTY()
    TMap<FString, TObjectPtr<UObject>> LoadedTools;

    UPROPERTY()
    TMap<FString, TScriptInterface<ITwinToolInterface>> ActiveTools;

    UPROPERTY()
    EAppMode CurrentMode = EAppMode::MainMenu;

    void LoadRegisteredTools();
    void UnloadAllTools();
    bool ValidateToolClass(TSubclassOf<UObject> ToolClass) const;
};