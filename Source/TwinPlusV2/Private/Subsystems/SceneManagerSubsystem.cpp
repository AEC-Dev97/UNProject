#include "Subsystems/SceneManagerSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Core/Logs.h"
void USceneManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    FSoftObjectPath RegistryPath(TEXT("/Game/Config/SceneRegistry.SceneRegistry"));
    if (UObject* LoadedAsset = RegistryPath.TryLoad())
    {
        SceneRegistry = Cast<USceneRegistry>(LoadedAsset);
    }

    if (!SceneRegistry)
    {
        UE_LOG(LogScene, Error, TEXT("SceneManagerSubsystem: SceneRegistry not found! Creating fallback."));
        SceneRegistry = NewObject<USceneRegistry>(this);
        ensure(SceneRegistry);
    }
}

const FSceneEntry* USceneManagerSubsystem::FindSceneEntry(FName SceneName) const
{
    if (!SceneRegistry) return nullptr;
    for (const FSceneEntry& Entry : SceneRegistry->Scenes)
        if (Entry.SceneName == SceneName) return &Entry;
    return nullptr;
}

void USceneManagerSubsystem::LoadScene(FName SceneName, bool bMakeVisible)
{
    if (const FSceneEntry* Entry = FindSceneEntry(SceneName))
    {
        FLatentActionInfo LatentInfo;
        LatentInfo.CallbackTarget = this;
        UGameplayStatics::LoadStreamLevel(this, FName(*Entry->LevelPath.GetAssetName()), bMakeVisible, false, LatentInfo);

        CurrentScene = SceneName;
        OnSceneLoaded.Broadcast(SceneName);
        UE_LOG(LogScene, Log, TEXT("SceneManagerSubsystem: Loaded scene %s"), *SceneName.ToString());
    }
    else UE_LOG(LogScene, Warning, TEXT("Scene %s not found!"), *SceneName.ToString());
}

void USceneManagerSubsystem::UnloadScene(FName SceneName)
{
    if (const FSceneEntry* Entry = FindSceneEntry(SceneName))
    {
        FLatentActionInfo LatentInfo;
        LatentInfo.CallbackTarget = this;
        UGameplayStatics::UnloadStreamLevel(this, FName(*Entry->LevelPath.GetAssetName()), LatentInfo, false);

        if (CurrentScene == SceneName) CurrentScene = NAME_None;
        OnSceneUnloaded.Broadcast(SceneName);
        UE_LOG(LogScene, Log, TEXT("SceneManagerSubsystem: Unloaded scene %s"), *SceneName.ToString());
    }
}

void USceneManagerSubsystem::SwitchScene(FName SceneName)
{
    SceneQueue.Enqueue(SceneName);

    if (CurrentScene != NAME_None)
    {
        OnSceneUnloaded.AddDynamic(this, &USceneManagerSubsystem::OnPreviousSceneUnloaded);
        UnloadScene(CurrentScene);
    }
    else ProcessNextSceneInQueue();
}

void USceneManagerSubsystem::OnPreviousSceneUnloaded(FName UnloadedScene)
{
    OnSceneUnloaded.RemoveDynamic(this, &USceneManagerSubsystem::OnPreviousSceneUnloaded);
    ProcessNextSceneInQueue();
}

void USceneManagerSubsystem::ProcessNextSceneInQueue()
{
    FName NextScene;
    if (SceneQueue.Dequeue(NextScene)) LoadScene(NextScene);
}
