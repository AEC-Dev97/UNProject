#include "Core/TwinGameMode.h"
#include "Controllers/TwinPlayerController.h"
#include "Subsystems/ModeManagerSubsystem.h"
#include "Pawns/TwinCameraPawn.h"
#include "Core/Logs.h"

ATwinGameMode::ATwinGameMode()
{
    PlayerControllerClass = ATwinPlayerController::StaticClass();
    //DefaultPawnClass = ATwinPawn::StaticClass();
}

void ATwinGameMode::BeginPlay()
{
    Super::BeginPlay();
    ApplyAppState(EAppState::Explore);

    //RegisterExampleTools();
}

void ATwinGameMode::ApplyAppState(EAppState NewState)
{
    if (CurrentAppState == NewState)
        return; 

    CurrentAppState = NewState;
    OnAppStateChanged.Broadcast(NewState);

    UE_LOG(LogMode, Log, TEXT("AppState Changed: %d"), static_cast<uint8>(NewState));

    if (UGameInstance* GI = GetGameInstance())
    {
        if (UModeManagerSubsystem* ModeMgr = GI->GetSubsystem<UModeManagerSubsystem>())
        {
            static bool bUpdatingMode = false;
            if (!bUpdatingMode)
            {
                bUpdatingMode = true;
                switch (NewState)
                {
                    case EAppState::Explore:
                        ModeMgr->SetMode(EAppMode::InGame);
                        break;

                    case EAppState::Simulation:
                        ModeMgr->SetMode(EAppMode::Runtime);
                        break;

                    case EAppState::Edit:
                        ModeMgr->SetMode(EAppMode::Editor);
                        break;

                    default:
                        break;
                }
                bUpdatingMode = false;
            }
        }
    }
}

//void ATwinGameMode::RegisterExampleTools()
//{
//    if (UGameInstance* GI = GetGameInstance())
//    {
//        if (UTwinPluginManager* PluginMgr = GI->GetSubsystem<UTwinPluginManager>())
//        {
//            // Example: Register a basic measurement tool
//            // This would be your custom tool classes
//            /*
//            PluginMgr->RegisterTool(TEXT("MeasurementTool"), UMeasurementTool::StaticClass());
//            PluginMgr->RegisterTool(TEXT("AnalysisTool"), UAnalysisTool::StaticClass());
//            PluginMgr->RegisterTool(TEXT("DataExportTool"), UDataExportTool::StaticClass());
//            */
//
//            UE_LOG(LogMode, Log, TEXT("Example tools registration completed"));
//        }
//    }
//}
