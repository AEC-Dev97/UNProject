#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Blueprint/UserWidget.h"
#include "UIRegistry.generated.h"
USTRUCT(BlueprintType)
struct FUIEntry
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName UIName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSoftClassPtr<UUserWidget> WidgetClass;
};

UCLASS(BlueprintType)
class TWINPLUSV2_API UUIRegistry : public UDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TArray<FUIEntry> UIEntries;

    const FUIEntry* FindEntry(FName Name) const
    {
        for (const FUIEntry& Entry : UIEntries)
        {
            if (Entry.UIName == Name)
                return &Entry;
        }
        return nullptr;
    }
};
