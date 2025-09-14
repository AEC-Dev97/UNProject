#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "InputManagerSubsystem.generated.h"

class UInputMappingContext;
class APawn;

UCLASS()
class TWINPLUSV2_API UInputManagerSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Input")
    void ApplyContext(UInputMappingContext* Context, int32 Priority = 0);

    UFUNCTION(BlueprintCallable, Category="Input")
    void ClearContexts();

    UFUNCTION(BlueprintCallable, Category="Input")
    void BindPawnInputs(APawn* Pawn);

    UFUNCTION(BlueprintCallable, Category="Input")
    void BindAllPlayerPawns();
};
