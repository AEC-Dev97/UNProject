#include "Controllers/TwinPlayerController.h"
#include "Subsystems/ModeManagerSubsystem.h"
#include "Subsystems/UIManagerSubsystem.h"
#include "Core/AppModes.h"
#include "Subsystems/GlobalServices.h"
#include "Subsystems/SceneManagerSubsystem.h"
#include "Kismet/GameplayStatics.h"
void ATwinPlayerController::BeginPlay()
{
    Super::BeginPlay();

    if (UModeManagerSubsystem* ModeMgr = UGlobalServices::GetModeManager(this))
    {
        ModeMgr->OnModeChanged.AddDynamic(this, &ATwinPlayerController::HandleModeChanged);
    }
}

void ATwinPlayerController::HandleModeChanged(EAppMode NewMode)
{
    switch (NewMode)
    {
        case EAppMode::MainMenu:
            bShowMouseCursor = true;
            SetInputMode(FInputModeUIOnly());
            break;
        case EAppMode::InGame:
            bShowMouseCursor = false;
            SetInputMode(FInputModeGameOnly());
            break;
        case EAppMode::Editor:
            bShowMouseCursor = true;
            SetInputMode(FInputModeGameAndUI());
            break;
        case EAppMode::Runtime:
            bShowMouseCursor = false;
            SetInputMode(FInputModeGameOnly());
            break;
        default: break;
    }
}
