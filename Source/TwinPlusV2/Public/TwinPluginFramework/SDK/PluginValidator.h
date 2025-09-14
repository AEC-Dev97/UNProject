#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TwinPluginFramework/Core/TwinToolInterface.h"


#include "PluginValidator.generated.h"

UENUM(BlueprintType)
enum class EValidationSeverity : uint8
{
    Info        UMETA(DisplayName = "Information"),
    Warning     UMETA(DisplayName = "Warning"),
    Error       UMETA(DisplayName = "Error"),
    Critical    UMETA(DisplayName = "Critical")
};

USTRUCT(BlueprintType)
struct TWINPLUSV2_API FValidationIssue
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Validation")
    EValidationSeverity Severity = EValidationSeverity::Info;

    UPROPERTY(BlueprintReadOnly, Category = "Validation")
    FString Category;

    UPROPERTY(BlueprintReadOnly, Category = "Validation")
    FString Message;

    UPROPERTY(BlueprintReadOnly, Category = "Validation")
    FString SourceFile;

    UPROPERTY(BlueprintReadOnly, Category = "Validation")
    int32 LineNumber = 0;

    UPROPERTY(BlueprintReadOnly, Category = "Validation")
    FString Suggestion;
};

USTRUCT(BlueprintType)
struct TWINPLUSV2_API FValidationResult
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Validation")
    bool bIsValid = true;

    UPROPERTY(BlueprintReadOnly, Category = "Validation")
    int32 ErrorCount = 0;

    UPROPERTY(BlueprintReadOnly, Category = "Validation")
    int32 WarningCount = 0;

    UPROPERTY(BlueprintReadOnly, Category = "Validation")
    TArray<FValidationIssue> Issues;

    UPROPERTY(BlueprintReadOnly, Category = "Validation")
    float ValidationTime = 0.0f;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnValidationProgress, float, Progress, const FString&, CurrentCheck);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnValidationComplete, const FValidationResult&, Result);

UCLASS(BlueprintType)
class TWINPLUSV2_API UTwinPluginValidator : public UObject
{
    GENERATED_BODY()

public:
    // Events
    UPROPERTY(BlueprintAssignable, Category = "Validation")
    FOnValidationProgress OnValidationProgress;

    UPROPERTY(BlueprintAssignable, Category = "Validation")
    FOnValidationComplete OnValidationComplete;

    // Main Validation Functions
    UFUNCTION(BlueprintCallable, Category = "Validation")
    void ValidateTool(TSubclassOf<UObject> ToolClass);

    UFUNCTION(BlueprintCallable, Category = "Validation")
    void ValidateToolByID(const FString& ToolID);

    UFUNCTION(BlueprintCallable, Category = "Validation")
    void ValidateAllTools();

    UFUNCTION(BlueprintCallable, Category = "Validation")
    FValidationResult ValidateToolSync(TSubclassOf<UObject> ToolClass);

    // Specific Validation Checks
    UFUNCTION(BlueprintCallable, Category = "Validation")
    TArray<FValidationIssue> CheckInterfaceImplementation(TSubclassOf<UObject> ToolClass);

    UFUNCTION(BlueprintCallable, Category = "Validation")
    TArray<FValidationIssue> CheckMetadataValidity(const FToolMetadata& Metadata);

    UFUNCTION(BlueprintCallable, Category = "Validation")
    TArray<FValidationIssue> CheckUIIntegration(TSubclassOf<UObject> ToolClass);

    UFUNCTION(BlueprintCallable, Category = "Validation")
    TArray<FValidationIssue> CheckPerformance(TSubclassOf<UObject> ToolClass);

    UFUNCTION(BlueprintCallable, Category = "Validation")
    TArray<FValidationIssue> CheckDocumentation(TSubclassOf<UObject> ToolClass);

    // Utility Functions
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Validation")
    bool IsValidationInProgress() const { return bIsValidating; }

    UFUNCTION(BlueprintCallable, Category = "Validation")
    void CancelValidation();

protected:
    // Internal Validation Logic
    void PerformAsyncValidation(TSubclassOf<UObject> ToolClass);
    void AddIssue(EValidationSeverity Severity, const FString& Category, const FString& Message, const FString& Suggestion = TEXT(""));
    void UpdateProgress(float Progress, const FString& CurrentCheck);
    void CompleteValidation();

private:
    bool bIsValidating = false;
    TArray<FValidationIssue> CurrentIssues;
    float CurrentProgress = 0.0f;
    FDateTime ValidationStartTime;
};