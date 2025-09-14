#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TwinPluginFramework/Core/TwinToolInterface.h"


#include "DocumentationGenerator.generated.h"

UENUM(BlueprintType)
enum class EDocumentationType : uint8
{
    API             UMETA(DisplayName = "API Documentation"),
    UserGuide       UMETA(DisplayName = "User Guide"),
    TechnicalRef    UMETA(DisplayName = "Technical Reference"),
    Tutorial        UMETA(DisplayName = "Tutorial"),
    Examples        UMETA(DisplayName = "Code Examples")
};

USTRUCT(BlueprintType)
struct TWINPLUSV2_API FTwinDocumentationSettings
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Documentation")
    TArray<EDocumentationType> DocumentationTypes;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Documentation")
    FString OutputDirectory = TEXT("Documentation/");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Documentation")
    bool bGenerateHTML = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Documentation")
    bool bGenerateMarkdown = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Documentation")
    bool bGeneratePDF = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Documentation")
    bool bIncludeScreenshots = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Documentation")
    bool bIncludeCodeExamples = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Documentation")
    FString ProjectName = TEXT("Twin Framework");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Documentation")
    FString Version = TEXT("1.0.0");
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDocumentationProgress, float, Progress, const FString&, CurrentDocument);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDocumentationComplete, bool, bSuccess, const FString&, OutputPath);

UCLASS(BlueprintType)
class TWINPLUSV2_API UTwinDocumentationGenerator : public UObject
{
    GENERATED_BODY()

public:
    // Events
    UPROPERTY(BlueprintAssignable, Category = "Documentation")
    FOnDocumentationProgress OnProgress;

    UPROPERTY(BlueprintAssignable, Category = "Documentation")
    FOnDocumentationComplete OnComplete;

    // Main Functions
    UFUNCTION(BlueprintCallable, Category = "Documentation")
    void GenerateDocumentation(const FTwinDocumentationSettings& Settings);

    UFUNCTION(BlueprintCallable, Category = "Documentation")
    void GenerateToolDocumentation(const FString& ToolID, const FTwinDocumentationSettings& Settings);

    UFUNCTION(BlueprintCallable, Category = "Documentation")
    void GenerateAllToolsDocumentation(const FTwinDocumentationSettings& Settings);

    // Specific Document Generation
    UFUNCTION(BlueprintCallable, Category = "Documentation")
    FString GenerateAPIReference(const FString& ToolID);

    UFUNCTION(BlueprintCallable, Category = "Documentation")
    FString GenerateUserGuide(const FString& ToolID);

    UFUNCTION(BlueprintCallable, Category = "Documentation")
    FString GenerateTutorial(const FString& ToolID);

    // Utility Functions
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Documentation")
    bool IsGenerationInProgress() const { return bIsGenerating; }

    UFUNCTION(BlueprintCallable, Category = "Documentation")
    void CancelGeneration();

    UFUNCTION(BlueprintCallable, Category = "Documentation")
    FTwinDocumentationSettings GetDefaultSettings() const;

protected:
    // Internal Generation Logic
    void GenerateDocumentationType(EDocumentationType Type, const FString& ToolID, const FTwinDocumentationSettings& Settings);
    FString CreateHTMLTemplate(const FString& Title, const FString& Content);
    FString CreateMarkdownTemplate(const FString& Title, const FString& Content);
    void SaveDocument(const FString& Content, const FString& FilePath, const FString& Format);
    void UpdateProgress(float Progress, const FString& CurrentDocument);
    void CompleteGeneration(bool bSuccess, const FString& OutputPath);

private:
    bool bIsGenerating = false;
    float CurrentProgress = 0.0f;
    FString CurrentOutputPath;
};
