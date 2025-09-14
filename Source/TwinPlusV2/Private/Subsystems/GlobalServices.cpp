#include "Subsystems/GlobalServices.h"
#include "TwinPluginFramework/Core/TwinPluginManager.h"
#include "Subsystems/UIManagerSubsystem.h"
#include "Subsystems/ModeManagerSubsystem.h"
#include "Subsystems/SceneManagerSubsystem.h"
#include "Subsystems/InputManagerSubsystem.h"

#include "Engine/World.h"
#include "Engine/GameInstance.h"


UUIManagerSubsystem* UGlobalServices::GetUIManager(const UObject* WorldContext)
{
    if (!WorldContext || !WorldContext->GetWorld()) return nullptr;
    if (UGameInstance* GI = WorldContext->GetWorld()->GetGameInstance())
        return GI->GetSubsystem<UUIManagerSubsystem>();
    return nullptr;
}

UModeManagerSubsystem* UGlobalServices::GetModeManager(const UObject* WorldContext)
{
    if (!WorldContext || !WorldContext->GetWorld()) return nullptr;
    if (UGameInstance* GI = WorldContext->GetWorld()->GetGameInstance())
        return GI->GetSubsystem<UModeManagerSubsystem>();
    return nullptr;
}

USceneManagerSubsystem* UGlobalServices::GetSceneManager(const UObject* WorldContext)
{
    if (!WorldContext || !WorldContext->GetWorld()) return nullptr;
    if (UGameInstance* GI = WorldContext->GetWorld()->GetGameInstance())
        return GI->GetSubsystem<USceneManagerSubsystem>();
    return nullptr;
}

UInputManagerSubsystem* UGlobalServices::GetInputManager(const UObject* WorldContext)
{
    if (!WorldContext || !WorldContext->GetWorld()) return nullptr;
    if (UGameInstance* GI = WorldContext->GetWorld()->GetGameInstance())
        return GI->GetSubsystem<UInputManagerSubsystem>();
    return nullptr;
}

UTwinPluginManager* UGlobalServices::GetPluginManager(const UObject* WorldContext)
{
    if (!WorldContext || !WorldContext->GetWorld()) return nullptr;
    if (UGameInstance* GI = WorldContext->GetWorld()->GetGameInstance())
        return GI->GetSubsystem<UTwinPluginManager>();
    return nullptr;
}
