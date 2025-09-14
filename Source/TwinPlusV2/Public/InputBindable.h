#pragma once
#include "UObject/Interface.h"
#include "InputBindable.generated.h"

class UInputManagerSubsystem;

UINTERFACE(MinimalAPI)
class UInputBindable : public UInterface
{
    GENERATED_BODY()
};

class IInputBindable
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Input")
    void BindInputs(UInputManagerSubsystem* InputMgr);
};
