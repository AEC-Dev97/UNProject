#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Core/AppModes.h"
#include "DataAssets/AppModeRegistry.h"
#include "ModeManagerSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnModeChanged, EAppMode, NewMode);

UCLASS()
class TWINPLUSV2_API UModeManagerSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    UFUNCTION(BlueprintCallable, Category="Mode")
    void SetMode(EAppMode Mode);

    const FAppModeEntry* GetCurrentModeEntry() const;

    UPROPERTY(BlueprintAssignable)
    FOnModeChanged OnModeChanged;

private:
    UPROPERTY()
    TObjectPtr<UAppModeRegistry> ModeRegistry;

    EAppMode CurrentMode = EAppMode::None;
    TMap<EAppMode, TArray<EAppMode>> AllowedTransitions;

    bool CanTransition(EAppMode NewMode) const;
};
