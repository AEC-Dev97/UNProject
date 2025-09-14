#include "TwinPluginFramework/Template/DataProcessorTemplate.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Core/Logs.h"

UDataProcessorTemplate::UDataProcessorTemplate()
{
    // Data processor defaults
    UpdateInterval = 0.1f;
    BatchSize = 100;
    bShowProgressUI = true;

    // Update tool metadata
    ToolMetadata.ToolName = TEXT("Data Processor");
    ToolMetadata.Description = TEXT("A data processing tool template");
    ToolMetadata.Category = EToolCategory::Analysis;
}

void UDataProcessorTemplate::StartProcessing()
{
    if (bIsProcessing)
    {
        UE_LOG(LogMode, Warning, TEXT("Processing already in progress for: %s"), *ToolMetadata.ToolName);
        return;
    }

    if (!bIsActive)
    {
        UE_LOG(LogMode, Error, TEXT("Cannot start processing - tool not active: %s"), *ToolMetadata.ToolName);
        return;
    }

    InternalStartProcessing();
}

void UDataProcessorTemplate::CancelProcessing()
{
    if (!bIsProcessing)
    {
        return;
    }

    UE_LOG(LogMode, Log, TEXT("Cancelling processing for: %s"), *ToolMetadata.ToolName);

    // Clear the timer
    if (GetWorld())
    {
        GetWorld()->GetTimerManager().ClearTimer(ProcessingTimer);
    }

    bIsProcessing = false;
    bIsPaused = false;
    CurrentProgress = 0.0f;
    CurrentBatch = 0;

    OnDataProcessingComplete.Broadcast(false, TEXT("Processing cancelled"));
}

void UDataProcessorTemplate::PauseProcessing()
{
    if (!bIsProcessing || bIsPaused)
    {
        return;
    }

    bIsPaused = true;

    if (GetWorld())
    {
        GetWorld()->GetTimerManager().PauseTimer(ProcessingTimer);
    }

    UE_LOG(LogMode, Log, TEXT("Paused processing for: %s"), *ToolMetadata.ToolName);
}

void UDataProcessorTemplate::ResumeProcessing()
{
    if (!bIsProcessing || !bIsPaused)
    {
        return;
    }

    bIsPaused = false;

    if (GetWorld())
    {
        GetWorld()->GetTimerManager().UnPauseTimer(ProcessingTimer);
    }

    UE_LOG(LogMode, Log, TEXT("Resumed processing for: %s"), *ToolMetadata.ToolName);
}

void UDataProcessorTemplate::UpdateProgress(float Progress, const FString& Status)
{
    CurrentProgress = FMath::Clamp(Progress, 0.0f, 1.0f);
    CurrentStatus = Status;

    OnDataProcessingProgress.Broadcast(CurrentProgress, CurrentStatus);
    BroadcastProgress(CurrentProgress, CurrentStatus);
}

void UDataProcessorTemplate::CompleteProcessing(bool bSuccess, const FString& Result)
{
    InternalCompleteProcessing(bSuccess, Result);
}

void UDataProcessorTemplate::InternalStartProcessing()
{
    bIsProcessing = true;
    bIsPaused = false;
    CurrentProgress = 0.0f;
    CurrentBatch = 0;

    // Get total number of batches from Blueprint implementation
    TotalBatches = GetTotalBatches();

    if (TotalBatches <= 0)
    {
        UE_LOG(LogMode, Warning, TEXT("No batches to process for: %s"), *ToolMetadata.ToolName);
        InternalCompleteProcessing(false, TEXT("No data to process"));
        return;
    }

    UE_LOG(LogMode, Log, TEXT("Starting processing for: %s - %d batches"),
        *ToolMetadata.ToolName, TotalBatches);

    // Initialize processing in Blueprint
    InitializeProcessing();

    // Start processing timer
    if (GetWorld())
    {
        GetWorld()->GetTimerManager().SetTimer(
            ProcessingTimer,
            this,
            &UDataProcessorTemplate::ProcessNextBatch,
            UpdateInterval,
            true
        );
    }

    UpdateProgress(0.0f, TEXT("Processing started"));
}

void UDataProcessorTemplate::ProcessNextBatch()
{
    if (!bIsProcessing || bIsPaused)
    {
        return;
    }

    if (CurrentBatch >= TotalBatches)
    {
        // Processing complete
        InternalCompleteProcessing(true, TEXT("Processing completed successfully"));
        return;
    }

    // Process current batch in Blueprint
    ProcessDataBatch(CurrentBatch);

    CurrentBatch++;

    // Update progress
    float Progress = static_cast<float>(CurrentBatch) / static_cast<float>(TotalBatches);
    FString Status = FString::Printf(TEXT("Processing batch %d/%d"), CurrentBatch, TotalBatches);
    UpdateProgress(Progress, Status);
}

void UDataProcessorTemplate::InternalCompleteProcessing(bool bSuccess, const FString& Result)
{
    if (!bIsProcessing)
    {
        return;
    }

    // Clear timer
    if (GetWorld())
    {
        GetWorld()->GetTimerManager().ClearTimer(ProcessingTimer);
    }

    bIsProcessing = false;
    bIsPaused = false;

    if (bSuccess)
    {
        CurrentProgress = 1.0f;
        // Finalize processing in Blueprint
        FinalizeProcessing();
    }

    UE_LOG(LogMode, Log, TEXT("Processing completed for: %s - Success: %s"),
        *ToolMetadata.ToolName, bSuccess ? TEXT("Yes") : TEXT("No"));

    OnDataProcessingComplete.Broadcast(bSuccess, Result);
    BroadcastProgress(CurrentProgress, bSuccess ? TEXT("Completed") : TEXT("Failed"));
}