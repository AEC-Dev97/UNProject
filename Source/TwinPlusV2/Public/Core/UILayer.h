#pragma once

#include "CoreMinimal.h"
#include "UILayer.generated.h"


UENUM(BlueprintType)
enum class EUILayer : uint8
{
	Background UMETA(DisplayName="Background"),
	HUD        UMETA(DisplayName="HUD"),
	Menu       UMETA(DisplayName="Menu"),
	Popup      UMETA(DisplayName="Popup")
};
