#include "Subsystems/UIManagerSubsystem.h"
#include "Core/UILayer.h"
#include "Blueprint/UserWidget.h"
#include "Engine/World.h"
#include "Core/Logs.h"
#include "UObject/ConstructorHelpers.h"

void UUIManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    FSoftObjectPath RegistryPath(TEXT("/Game/Config/UIRegistry.UIRegistry"));
    if (UObject* LoadedAsset = RegistryPath.TryLoad())
    {
        UIRegistry = Cast<UUIRegistry>(LoadedAsset);
    }

    if (!UIRegistry)
    {
        UE_LOG(LogUI, Error, TEXT("UIManagerSubsystem: UIRegistry not found! Creating empty fallback."));
        UIRegistry = NewObject<UUIRegistry>(this);
        ensure(UIRegistry);
    }

}

void UUIManagerSubsystem::PushUI(FName UIName, EUILayer Layer)
{
    if (!UIRegistry) return;

    const FUIEntry* Entry = UIRegistry->FindEntry(UIName);
    if (!Entry) return;

    if (UClass* WidgetClass = Entry->WidgetClass.LoadSynchronous())
    {
        if (UUserWidget** ExistingWidget = ActiveWidgets.Find(Layer))
        {
            if (*ExistingWidget) (*ExistingWidget)->RemoveFromParent();
            ActiveWidgets.Remove(Layer);
        }

        UUserWidget* Widget = CreateWidget<UUserWidget>(GetWorld(), WidgetClass);
        if (Widget)
        {
            Widget->AddToViewport(static_cast<int32>(Layer));
            ActiveWidgets.Add(Layer, Widget);
            OnUIPushed.Broadcast(UIName, Layer, Widget);
        }
    }
}

void UUIManagerSubsystem::PopUI(EUILayer Layer)
{
    if (UUserWidget** Found = ActiveWidgets.Find(Layer))
    {
        if (UUserWidget* Widget = *Found)
        {
            Widget->RemoveFromParent();
            OnUIPopped.Broadcast(Layer, Widget);
        }
        ActiveWidgets.Remove(Layer);
    }
}

void UUIManagerSubsystem::PopAllUI()
{
    for (auto& Pair : ActiveWidgets)
    {
        if (UUserWidget* Widget = Pair.Value)
        {
            EUILayer Layer = Pair.Key;
            Widget->RemoveFromParent();
            OnUIPopped.Broadcast(Layer, Widget); 
        }
    }
    ActiveWidgets.Empty();
}

void UUIManagerSubsystem::Deinitialize()
{
    for (auto& Pair : ActiveWidgets)
    {
        if (UUserWidget* Widget = Pair.Value)
        {
            Widget->RemoveFromParent();
        }
    }
    ActiveWidgets.Empty();

    Super::Deinitialize();
}