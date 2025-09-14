#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Core/AppModes.h"
#include "AppModeEntry.generated.h"

USTRUCT(BlueprintType)
struct FAppModeEntry : public FTableRowBase
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="App Mode")
    EAppMode Mode = EAppMode::None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="App Mode")
    FText DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="App Mode")
    FText Description;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="App Mode")
    TSoftObjectPtr<UWorld> Level;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="App Mode")
    TSoftObjectPtr<UTexture2D> Icon;
};
