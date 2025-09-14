#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Core/UILayer.h"
#include "DataAssets/UIRegistry.h"
#include "UIManagerSubsystem.generated.h"

class UUserWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnUIPushed, FName, UIName, EUILayer, Layer, UUserWidget*, Widget);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUIPopped, EUILayer, Layer, UUserWidget*, Widget);

UCLASS()
class TWINPLUSV2_API UUIManagerSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

virtual void Deinitialize() override;   

    UFUNCTION(BlueprintCallable, Category = "UI")
    void PushUI(FName UIName, EUILayer Layer);

    UFUNCTION(BlueprintCallable, Category = "UI")
    void PopUI(EUILayer Layer);

    UFUNCTION(BlueprintCallable, Category = "UI")
    void PopAllUI();

    UPROPERTY(BlueprintAssignable, Category = "UI")
    FOnUIPushed OnUIPushed;

    UPROPERTY(BlueprintAssignable, Category = "UI")
    FOnUIPopped OnUIPopped;

private:
    UPROPERTY()
    UUIRegistry* UIRegistry;
    UPROPERTY()
    TMap<EUILayer, UUserWidget*> ActiveWidgets;
};
