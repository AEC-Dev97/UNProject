#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "DataAssets/SceneRegistry.h"
#include "SceneManagerSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSceneEvent, FName, SceneName);

UCLASS()
class TWINPLUSV2_API USceneManagerSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    UFUNCTION(BlueprintCallable, Category="Scene")
    void LoadScene(FName SceneName, bool bMakeVisible = true);

    UFUNCTION(BlueprintCallable, Category="Scene")
    void UnloadScene(FName SceneName);

    UFUNCTION(BlueprintCallable, Category="Scene")
    void SwitchScene(FName SceneName);

    FName GetCurrentScene() const { return CurrentScene; }

    UPROPERTY(BlueprintAssignable, Category="Scene")
    FOnSceneEvent OnSceneLoaded;

    UPROPERTY(BlueprintAssignable, Category="Scene")
    FOnSceneEvent OnSceneUnloaded;

private:
    UPROPERTY()
    TObjectPtr<USceneRegistry> SceneRegistry;

    UPROPERTY()
    FName CurrentScene = NAME_None;

    TQueue<FName> SceneQueue;

    void ProcessNextSceneInQueue();
    const FSceneEntry* FindSceneEntry(FName SceneName) const;

    UFUNCTION()
    void OnPreviousSceneUnloaded(FName UnloadedScene);
};
