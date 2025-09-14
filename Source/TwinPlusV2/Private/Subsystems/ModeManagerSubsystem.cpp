#include "Subsystems/ModeManagerSubsystem.h"
#include "Core/Logs.h"
#include "UObject/ConstructorHelpers.h"
void UModeManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    FSoftObjectPath RegistryPath(TEXT("/Game/Config/AppModeRegistry.AppModeRegistry"));
    if (UObject* LoadedAsset = RegistryPath.TryLoad())
    {
        ModeRegistry = Cast<UAppModeRegistry>(LoadedAsset);
    }

    if (!ModeRegistry)
    {
        UE_LOG(LogMode, Error, TEXT("ModeManagerSubsystem: Failed to load AppModeRegistry. Creating fallback."));
        ModeRegistry = NewObject<UAppModeRegistry>(this);
        ensure(ModeRegistry);
    }


    AllowedTransitions.Add(EAppMode::MainMenu, {EAppMode::InGame, EAppMode::Editor});
    AllowedTransitions.Add(EAppMode::InGame, {EAppMode::MainMenu, EAppMode::Runtime});
    AllowedTransitions.Add(EAppMode::Editor, {EAppMode::MainMenu});
    AllowedTransitions.Add(EAppMode::Runtime, {EAppMode::InGame});
}

bool UModeManagerSubsystem::CanTransition(EAppMode NewMode) const
{
    if (CurrentMode == NewMode) return false;
    if (AllowedTransitions.Contains(CurrentMode)) return AllowedTransitions[CurrentMode].Contains(NewMode);
    return true;
}

void UModeManagerSubsystem::SetMode(EAppMode Mode)
{
    if (!CanTransition(Mode)) { UE_LOG(LogMode, Warning, TEXT("Cannot transition from %d to %d"), (int32)CurrentMode, (int32)Mode); return; }
    if (ModeRegistry && !ModeRegistry->FindMode(Mode)) { UE_LOG(LogMode, Warning, TEXT("Mode %d not found"), (int32)Mode); return; }

    CurrentMode = Mode;
    OnModeChanged.Broadcast(Mode);
    UE_LOG(LogMode, Log, TEXT("ModeManagerSubsystem: Mode set to %d"), (int32)Mode);
}

const FAppModeEntry* UModeManagerSubsystem::GetCurrentModeEntry() const
{
    return ModeRegistry ? ModeRegistry->FindMode(CurrentMode) : nullptr;
}
