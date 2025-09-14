#pragma once
#include "CoreMinimal.h"
#include "TwinPluginFramework/Template/BasicToolTemplate.h"
#include "Engine/LatentActionManager.h"
#include "DataProcessorTemplate.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDataProcessingComplete, bool, bSuccess, const FString&, Result);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDataProcessingProgress, float, Progress, const FString&, Status);

/**
 * Data Processor Template - For tools that process data asynchronously
 *
 * Features:
 * - Async data processing
 * - Progress tracking
 * - Cancellation support
 * - Batch processing capabilities
 */
UCLASS(BlueprintType, Blueprintable, Abstract)
class TWINPLUSV2_API UDataProcessorTemplate : public UBasicToolTemplate
{
    GENERATED_BODY()

public:
    UDataProcessorTemplate();

    // Data Processing Events
    UPROPERTY(BlueprintAssignable, Category = "Data Processing")
    FOnDataProcessingComplete OnDataProcessingComplete;

    UPROPERTY(BlueprintAssignable, Category = "Data Processing")
    FOnDataProcessingProgress OnDataProcessingProgress;

    // Processing Control
    UFUNCTION(BlueprintCallable, Category = "Data Processing")
    void StartProcessing();

    UFUNCTION(BlueprintCallable, Category = "Data Processing")
    void CancelProcessing();

    UFUNCTION(BlueprintCallable, Category = "Data Processing")
    void PauseProcessing();

    UFUNCTION(BlueprintCallable, Category = "Data Processing")
    void ResumeProcessing();

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Data Processing")
    bool IsProcessing() const { return bIsProcessing; }

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Data Processing")
    bool IsPaused() const { return bIsPaused; }

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Data Processing")
    float GetProgress() const { return CurrentProgress; }

protected:
    // Processing State
    UPROPERTY(BlueprintReadOnly, Category = "Processing State")
    bool bIsProcessing = false;

    UPROPERTY(BlueprintReadOnly, Category = "Processing State")
    bool bIsPaused = false;

    UPROPERTY(BlueprintReadOnly, Category = "Processing State")
    float CurrentProgress = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Processing State")
    FString CurrentStatus;

    // Configuration
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Processing Config")
    float UpdateInterval = 0.1f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Processing Config")
    int32 BatchSize = 100;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Processing Config")
    bool bShowProgressUI = true;

    // Virtual Processing Functions
    UFUNCTION(BlueprintImplementableEvent, Category = "Data Processing")
    void InitializeProcessing();

    UFUNCTION(BlueprintImplementableEvent, Category = "Data Processing")
    void ProcessDataBatch(int32 BatchIndex);

    UFUNCTION(BlueprintImplementableEvent, Category = "Data Processing")
    void FinalizeProcessing();

    UFUNCTION(BlueprintImplementableEvent, Category = "Data Processing")
    int32 GetTotalBatches() const;

    // Progress Updates
    UFUNCTION(BlueprintCallable, Category = "Data Processing")
    void UpdateProgress(float Progress, const FString& Status = TEXT(""));

    UFUNCTION(BlueprintCallable, Category = "Data Processing")
    void CompleteProcessing(bool bSuccess, const FString& Result = TEXT(""));

private:
    FTimerHandle ProcessingTimer;
    int32 CurrentBatch = 0;
    int32 TotalBatches = 0;

    void ProcessNextBatch();
    void InternalStartProcessing();
    void InternalCompleteProcessing(bool bSuccess, const FString& Result);
};