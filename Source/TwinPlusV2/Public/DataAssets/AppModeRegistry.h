#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Core/AppModeEntry.h"
#include "AppModeRegistry.generated.h"

UCLASS(BlueprintType)
class TWINPLUSV2_API UAppModeRegistry : public UDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Modes")
    TArray<FAppModeEntry> Modes;
    const FAppModeEntry* FindMode(EAppMode Mode) const
    {
        for (const FAppModeEntry& Entry : Modes)
        {
            if (Entry.Mode == Mode)
            {
                return &Entry;
            }
        }
        return nullptr;
    }
};
