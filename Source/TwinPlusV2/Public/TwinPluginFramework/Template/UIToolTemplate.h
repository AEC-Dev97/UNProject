#pragma once
#include "CoreMinimal.h"
#include "TwinPluginFramework/Template/BasicToolTemplate.h"
#include "Core/UILayer.h"
#include "UIToolTemplate.generated.h"

/**
 * UI Tool Template - For tools that primarily interact through UI
 *
 * Features:
 * - Automatic UI management
 * - Layer-based widget organization
 * - Input handling integration
 * - Mode-aware UI updates
 */
UCLASS(BlueprintType, Blueprintable, Abstract)
class TWINPLUSV2_API UUIToolTemplate : public UBasicToolTemplate
{
    GENERATED_BODY()

public:
    UUIToolTemplate();

    // Override Base Implementation
    virtual void ActivateTool_Implementation() override;
    virtual void DeactivateTool_Implementation() override;
    virtual UUserWidget* CreateToolWidget_Implementation() override;
    virtual void OnModeChanged_Implementation(EAppMode NewMode) override;

protected:
    // UI Configuration
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI Config")
    EUILayer TargetUILayer = EUILayer::Menu;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI Config")
    bool bAutoShowUI = true;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI Config")
    bool bHideOnModeChange = true;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI Config")
    TMap<EAppMode, TSoftClassPtr<UUserWidget>> ModeSpecificWidgets;

    // UI Events
    UFUNCTION(BlueprintImplementableEvent, Category = "UI Events")
    void OnUIShown();

    UFUNCTION(BlueprintImplementableEvent, Category = "UI Events")
    void OnUIHidden();

    UFUNCTION(BlueprintImplementableEvent, Category = "UI Events")
    void OnUIWidgetCreated(UUserWidget* Widget);

    // UI Management
    UFUNCTION(BlueprintCallable, Category = "UI Management")
    void ShowToolUI();

    UFUNCTION(BlueprintCallable, Category = "UI Management")
    void HideToolUI();

    UFUNCTION(BlueprintCallable, Category = "UI Management")
    void RefreshUI();

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "UI Management")
    bool IsUIVisible() const;

private:
    bool bUIVisible = false;
    void UpdateUIForMode(EAppMode Mode);
};