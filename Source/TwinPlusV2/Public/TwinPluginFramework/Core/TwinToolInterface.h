#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Core/AppModes.h"

#include "TwinToolInterface.generated.h"

class UUserWidget;

UENUM(BlueprintType)
enum class EToolCategory : uint8
{
    Analysis        UMETA(DisplayName = "Analysis"),
    UI              UMETA(DisplayName = "UI Tools"),
    Utility         UMETA(DisplayName = "Utilities"),
    Workflow        UMETA(DisplayName = "Workflows"),
    Custom          UMETA(DisplayName = "Custom")
};

UENUM(BlueprintType)
enum class EToolScope : uint8
{
    Global          UMETA(DisplayName = "Global"),
    ModeSpecific    UMETA(DisplayName = "Mode Specific"),
    SceneSpecific   UMETA(DisplayName = "Scene Specific")
};

USTRUCT(BlueprintType)
struct TWINPLUSV2_API FToolMetadata
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString ToolName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Description;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Version;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Author;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EToolCategory Category = EToolCategory::Custom;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EToolScope Scope = EToolScope::Global;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<EAppMode> SupportedModes;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSoftObjectPtr<UTexture2D> Icon;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Priority = 100;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bRequiresScene = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FString> Dependencies;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnToolStateChanged, TScriptInterface<ITwinToolInterface>, Tool, bool, bActive);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnToolProgress, TScriptInterface<ITwinToolInterface>, Tool, float, Progress, const FString&, Message);


UINTERFACE(MinimalAPI, BlueprintType)
class UTwinToolInterface : public UInterface
{
    GENERATED_BODY()
};

class TWINPLUSV2_API ITwinToolInterface
{
    GENERATED_BODY()

public:
    // Core Tool Lifecycle
    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Tool Lifecycle")
    bool InitializeTool();

    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Tool Lifecycle")
    void ActivateTool();

    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Tool Lifecycle")
    void DeactivateTool();

    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Tool Lifecycle")
    void ShutdownTool();

    // Tool Information
    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Tool Info")
    FToolMetadata GetToolMetadata() const;

    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Tool Info")
    bool IsToolActive() const;

    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Tool Info")
    bool CanActivateInMode(EAppMode Mode) const;

    // UI Integration
    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Tool UI")
    UUserWidget* CreateToolWidget();

    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Tool UI")
    void OnModeChanged(EAppMode NewMode);

    // Events
    //UPROPERTY(BlueprintAssignable, Category = "Tool Events")
    //FOnToolStateChanged OnToolStateChanged;

    //UPROPERTY(BlueprintAssignable, Category = "Tool Events")
    //FOnToolProgress OnToolProgress;
};