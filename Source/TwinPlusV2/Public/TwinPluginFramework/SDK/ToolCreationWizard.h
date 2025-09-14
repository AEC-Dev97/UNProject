#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TwinPluginFramework/Core/TwinToolInterface.h"
#include "Core/UILayer.h"

#include "ToolCreationWizard.generated.h"

UENUM(BlueprintType)
enum class EToolTemplateType : uint8
{
    BasicTool           UMETA(DisplayName = "Basic Tool"),
    UITool              UMETA(DisplayName = "UI Tool"),
    DataProcessor       UMETA(DisplayName = "Data Processor"),
    AnalysisTool        UMETA(DisplayName = "Analysis Tool"),
    MeasurementTool     UMETA(DisplayName = "Measurement Tool"),
    SelectionTool       UMETA(DisplayName = "Selection Tool"),
    ImportExportTool    UMETA(DisplayName = "Import/Export Tool"),
    Custom              UMETA(DisplayName = "Custom Template")
};

USTRUCT(BlueprintType)
struct TWINPLUSV2_API FToolCreationSettings
{
    GENERATED_BODY()

    // Basic Information
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Basic Info")
    FString ToolName = TEXT("MyTool");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Basic Info")
    FString ClassName = TEXT("UMyTool");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Basic Info")
    FString Description = TEXT("A custom tool for Twin Framework");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Basic Info")
    FString Author = TEXT("Developer");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Basic Info")
    FString Version = TEXT("1.0.0");

    // Template Selection
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Template")
    EToolTemplateType TemplateType = EToolTemplateType::BasicTool;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Template")
    bool bCreateCppClass = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Template")
    bool bCreateBlueprintTemplate = true;

    // Tool Configuration
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
    EToolCategory Category = EToolCategory::Custom;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
    EToolScope Scope = EToolScope::Global;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
    TArray<EAppMode> SupportedModes;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
    bool bRequiresScene = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
    bool bCreateUI = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
    EUILayer UILayer = EUILayer::Menu;

    // File Paths
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Output")
    FString OutputDirectory = TEXT("Source/TwinPlusV2/Tools/");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Output")
    FString BlueprintPath = TEXT("/Game/Tools/");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Output")
    bool bAutoRegisterTool = true;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnToolCreationProgress, float, Progress, const FString&, Status);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnToolCreationComplete, bool, bSuccess, const FString&, Message);

UCLASS(BlueprintType)
class TWINPLUSV2_API UTwinToolCreationWizard : public UObject
{
    GENERATED_BODY()

public:
    // Events
    UPROPERTY(BlueprintAssignable, Category = "Tool Creation")
    FOnToolCreationProgress OnProgress;

    UPROPERTY(BlueprintAssignable, Category = "Tool Creation")
    FOnToolCreationComplete OnComplete;

    // Main Functions
    UFUNCTION(BlueprintCallable, Category = "Tool Creation")
    void CreateTool(const FToolCreationSettings& Settings);

    UFUNCTION(BlueprintCallable, Category = "Tool Creation")
    bool ValidateSettings(const FToolCreationSettings& Settings, FString& ErrorMessage);

    UFUNCTION(BlueprintCallable, Category = "Tool Creation")
    FToolCreationSettings GetDefaultSettings() const;

    UFUNCTION(BlueprintCallable, Category = "Tool Creation")
    TArray<EToolTemplateType> GetAvailableTemplates() const;

    UFUNCTION(BlueprintCallable, Category = "Tool Creation")
    FString GetTemplateDescription(EToolTemplateType TemplateType) const;

    // Utility Functions
    UFUNCTION(BlueprintCallable, Category = "Tool Creation")
    bool IsValidClassName(const FString& ClassName) const;

    UFUNCTION(BlueprintCallable, Category = "Tool Creation")
    FString SanitizeClassName(const FString& ClassName) const;

    UFUNCTION(BlueprintCallable, Category = "Tool Creation")
    FString GenerateToolID(const FString& ToolName) const;

    UFUNCTION(BlueprintCallable, Category = "Tool Creation")
    bool CheckFileExists(const FString& FilePath) const;

protected:
    // Internal Creation Functions
    void CreateCppFiles(const FToolCreationSettings& Settings);
    void CreateBlueprintAssets(const FToolCreationSettings& Settings);
    void RegisterToolInRegistry(const FToolCreationSettings& Settings);
    void GenerateDocumentation(const FToolCreationSettings& Settings);

    // Template Generation
    FString GenerateHeaderContent(const FToolCreationSettings& Settings);
    FString GenerateCppContent(const FToolCreationSettings& Settings);
    FString GetBaseClassForTemplate(EToolTemplateType TemplateType);

    // Progress Tracking
    void UpdateProgress(float Progress, const FString& Status);
    void CompleteCreation(bool bSuccess, const FString& Message);

private:
    float CurrentProgress = 0.0f;
    bool bIsCreating = false;
};