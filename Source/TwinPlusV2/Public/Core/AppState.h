#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EAppState : uint8
{
    None        UMETA(DisplayName="None"),
    Explore     UMETA(DisplayName="Explore"),      
    Simulation  UMETA(DisplayName="Simulation"),   
    Edit        UMETA(DisplayName="Edit"),        
    Loading     UMETA(DisplayName="Loading"),      
    Paused      UMETA(DisplayName="Paused")       
};
