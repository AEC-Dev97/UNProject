#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Engine/PrimaryAssetLabel.h"
#include "SceneRegistry.generated.h"

USTRUCT(BlueprintType)
struct FSceneEntry
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName SceneName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSoftObjectPtr<UWorld> LevelPath;
};

UCLASS(BlueprintType)
class TWINPLUSV2_API USceneRegistry : public UDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TArray<FSceneEntry> Scenes;
};
