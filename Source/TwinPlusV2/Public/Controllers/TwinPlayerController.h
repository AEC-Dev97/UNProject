
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Core/AppModes.h"
#include "TwinPlayerController.generated.h"

UCLASS()
class TWINPLUSV2_API ATwinPlayerController : public APlayerController
{
    GENERATED_BODY()

protected:
    virtual void BeginPlay() override;

    UFUNCTION()
    void HandleModeChanged(EAppMode NewMode);
};
