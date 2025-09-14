#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Core/AppState.h"
#include "Core/AppModes.h"
#include "TwinGameMode.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAppStateChanged, EAppState, NewState);

UCLASS()
class TWINPLUSV2_API ATwinGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    ATwinGameMode();

    virtual void BeginPlay() override;
    void ApplyAppState(EAppState NewState);

    EAppState GetCurrentAppState() const { return CurrentAppState; }

    UPROPERTY(BlueprintAssignable)
    FOnAppStateChanged OnAppStateChanged;

private:
    EAppState CurrentAppState = EAppState::Explore;
    //void RegisterExampleTools();
};
