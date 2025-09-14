#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GlobalServices.generated.h"

class UUIManagerSubsystem;
class UModeManagerSubsystem;
class USceneManagerSubsystem;
class UInputManagerSubsystem;
class UTwinPluginManager;

UCLASS()
class TWINPLUSV2_API UGlobalServices : public UObject
{
    GENERATED_BODY()

public:
    static UUIManagerSubsystem* GetUIManager(const UObject* WorldContext);
    static UModeManagerSubsystem* GetModeManager(const UObject* WorldContext);
    static USceneManagerSubsystem* GetSceneManager(const UObject* WorldContext);
    static UInputManagerSubsystem* GetInputManager(const UObject* WorldContext);
    static UTwinPluginManager* GetPluginManager(const UObject* WorldContext);
};
