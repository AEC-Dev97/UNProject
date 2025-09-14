#pragma once
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "TwinPluginFramework/Core/TwinToolInterface.h"
#include "TwinPluginRegistry.generated.h"

USTRUCT(BlueprintType)
struct TWINPLUSV2_API FRegisteredTool
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString ToolID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSoftClassPtr<UObject> ToolClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FToolMetadata Metadata;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bEnabled = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bAutoLoad = false;
};

UCLASS(BlueprintType)
class TWINPLUSV2_API UTwinPluginRegistry : public UDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tools")
    TArray<FRegisteredTool> RegisteredTools;

    UFUNCTION(BlueprintCallable, Category = "Plugin Registry")
    FRegisteredTool FindTool(const FString& ToolID) const;


    UFUNCTION(BlueprintCallable, Category = "Plugin Registry")
    TArray<FRegisteredTool> GetToolsByCategory(EToolCategory Category) const;

    UFUNCTION(BlueprintCallable, Category = "Plugin Registry")
    TArray<FRegisteredTool> GetToolsForMode(EAppMode Mode) const;

    UFUNCTION(BlueprintCallable, Category = "Plugin Registry")
    void RegisterTool(const FRegisteredTool& Tool);

    UFUNCTION(BlueprintCallable, Category = "Plugin Registry")
    bool UnregisterTool(const FString& ToolID);
};