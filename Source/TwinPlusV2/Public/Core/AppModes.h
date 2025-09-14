#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EAppMode : uint8
{
     None      UMETA(DisplayName="None"),
    MainMenu  UMETA(DisplayName="Main Menu"),   
    InGame    UMETA(DisplayName="In Game"),    
    Editor    UMETA(DisplayName="Editor"),      
    Runtime   UMETA(DisplayName="Runtime")      
};