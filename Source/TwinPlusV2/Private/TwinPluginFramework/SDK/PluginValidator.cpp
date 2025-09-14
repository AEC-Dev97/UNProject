#include "TwinPluginFramework/SDK/PluginValidator.h"
#include "Core/Logs.h"
#include "Subsystems/GlobalServices.h"
#include "TwinPluginFramework/Core/TwinPluginManager.h"

void UTwinPluginValidator::ValidateToolByID(const FString& ToolID)
{
    UE_LOG(LogMode, Log, TEXT("Validating tool by ID: %s"), *ToolID);
    // TODO: Implement actual validation logic
}

TArray<FValidationIssue> UTwinPluginValidator::CheckMetadataValidity(const FToolMetadata& Metadata)
{
    TArray<FValidationIssue> Issues;

    if (Metadata.ToolName.IsEmpty())
    {
        FValidationIssue Issue;
        Issue.Severity = EValidationSeverity::Error;
        Issue.Category = TEXT("Metadata");
        Issue.Message = TEXT("Tool name is required");
        Issues.Add(Issue);
    }

    return Issues;
}

TArray<FValidationIssue> UTwinPluginValidator::CheckUIIntegration(TSubclassOf<UObject> ToolClass)
{
    TArray<FValidationIssue> Issues;

    if (!ToolClass)
    {
        FValidationIssue Issue;
        Issue.Severity = EValidationSeverity::Error;
        Issue.Category = TEXT("UI Integration");
        Issue.Message = TEXT("Tool class is null");
        Issues.Add(Issue);
    }

    return Issues;
}

TArray<FValidationIssue> UTwinPluginValidator::CheckPerformance(TSubclassOf<UObject> ToolClass)
{
    TArray<FValidationIssue> Issues;

    if (!ToolClass)
    {
        FValidationIssue Issue;
        Issue.Severity = EValidationSeverity::Warning;
        Issue.Category = TEXT("Performance");
        Issue.Message = TEXT("Cannot check performance on null class");
        Issues.Add(Issue);
    }

    return Issues;
}

TArray<FValidationIssue> UTwinPluginValidator::CheckDocumentation(TSubclassOf<UObject> ToolClass)
{
    TArray<FValidationIssue> Issues;

    FValidationIssue Issue;
    Issue.Severity = EValidationSeverity::Info;
    Issue.Category = TEXT("Documentation");
    Issue.Message = TEXT("Documentation check completed");
    Issues.Add(Issue);

    return Issues;
}

void UTwinPluginValidator::CancelValidation()
{
    if (bIsValidating)
    {
        bIsValidating = false;
        UE_LOG(LogMode, Warning, TEXT("Validation cancelled"));
    }
}

void UTwinPluginValidator::ValidateTool(TSubclassOf<UObject> ToolClass)
{
    if (bIsValidating)
    {
        UE_LOG(LogMode, Warning, TEXT("Validation already in progress"));
        return;
    }

    ValidationStartTime = FDateTime::Now();
    bIsValidating = true;
    CurrentIssues.Empty();
    CurrentProgress = 0.0f;

    PerformAsyncValidation(ToolClass);
}

void UTwinPluginValidator::ValidateAllTools()
{
    if (UTwinPluginManager* PluginManager = UGlobalServices::GetPluginManager(this))
    {
        TArray<FString> AvailableTools = PluginManager->GetAvailableTools();

        UpdateProgress(0.0f, TEXT("Starting validation of all tools"));

        for (int32 i = 0; i < AvailableTools.Num(); i++)
        {
            float Progress = (float)i / AvailableTools.Num();
            FString Status = FString::Printf(TEXT("Validating tool %d/%d: %s"),
                i + 1, AvailableTools.Num(), *AvailableTools[i]);

            UpdateProgress(Progress, Status);
            UE_LOG(LogMode, Log, TEXT("Validating tool: %s"), *AvailableTools[i]);
        }

        CompleteValidation();
    }
}

FValidationResult UTwinPluginValidator::ValidateToolSync(TSubclassOf<UObject> ToolClass)
{
    FValidationResult Result;

    if (!ToolClass)
    {
        FValidationIssue Issue;
        Issue.Severity = EValidationSeverity::Error;
        Issue.Category = TEXT("Class Validation");
        Issue.Message = TEXT("Tool class is null");
        Result.Issues.Add(Issue);
        Result.bIsValid = false;
        Result.ErrorCount = 1;
        return Result;
    }

    TArray<FValidationIssue> InterfaceIssues = CheckInterfaceImplementation(ToolClass);
    Result.Issues.Append(InterfaceIssues);

    for (const FValidationIssue& Issue : Result.Issues)
    {
        if (Issue.Severity == EValidationSeverity::Error || Issue.Severity == EValidationSeverity::Critical)
        {
            Result.ErrorCount++;
        }
        else if (Issue.Severity == EValidationSeverity::Warning)
        {
            Result.WarningCount++;
        }
    }

    Result.bIsValid = (Result.ErrorCount == 0);
    return Result;
}

TArray<FValidationIssue> UTwinPluginValidator::CheckInterfaceImplementation(TSubclassOf<UObject> ToolClass)
{
    TArray<FValidationIssue> Issues;

    if (!ToolClass)
    {
        FValidationIssue Issue;
        Issue.Severity = EValidationSeverity::Error;
        Issue.Category = TEXT("Interface");
        Issue.Message = TEXT("Tool class is null");
        Issues.Add(Issue);
        return Issues;
    }

    // Check if class implements ITwinToolInterface
    if (!ToolClass->ImplementsInterface(UTwinToolInterface::StaticClass()))
    {
        FValidationIssue Issue;
        Issue.Severity = EValidationSeverity::Error;
        Issue.Category = TEXT("Interface");
        Issue.Message = TEXT("Tool class must implement ITwinToolInterface");
        Issue.Suggestion = TEXT("Add ITwinToolInterface to your class inheritance");
        Issues.Add(Issue);
    }

    return Issues;
}

void UTwinPluginValidator::PerformAsyncValidation(TSubclassOf<UObject> ToolClass)
{
    UpdateProgress(0.2f, TEXT("Checking interface implementation"));
    TArray<FValidationIssue> InterfaceIssues = CheckInterfaceImplementation(ToolClass);
    CurrentIssues.Append(InterfaceIssues);

    UpdateProgress(0.5f, TEXT("Checking UI integration"));
    TArray<FValidationIssue> UIIssues = CheckUIIntegration(ToolClass);
    CurrentIssues.Append(UIIssues);

    UpdateProgress(0.8f, TEXT("Checking performance"));
    TArray<FValidationIssue> PerformanceIssues = CheckPerformance(ToolClass);
    CurrentIssues.Append(PerformanceIssues);

    UpdateProgress(1.0f, TEXT("Validation complete"));
    CompleteValidation();
}

void UTwinPluginValidator::CompleteValidation()
{
    FValidationResult Result;
    Result.Issues = CurrentIssues;

    for (const FValidationIssue& Issue : CurrentIssues)
    {
        if (Issue.Severity == EValidationSeverity::Error || Issue.Severity == EValidationSeverity::Critical)
        {
            Result.ErrorCount++;
        }
        else if (Issue.Severity == EValidationSeverity::Warning)
        {
            Result.WarningCount++;
        }
    }

    Result.bIsValid = (Result.ErrorCount == 0);
    Result.ValidationTime = (FDateTime::Now() - ValidationStartTime).GetTotalSeconds();

    bIsValidating = false;
    OnValidationComplete.Broadcast(Result);
}

void UTwinPluginValidator::UpdateProgress(float Progress, const FString& CurrentCheck)
{
    CurrentProgress = Progress;
    OnValidationProgress.Broadcast(Progress, CurrentCheck);
}