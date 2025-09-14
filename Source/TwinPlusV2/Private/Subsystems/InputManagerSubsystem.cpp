
#include "Subsystems/InputManagerSubsystem.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/PlayerController.h"
#include "InputBindable.h"

void UInputManagerSubsystem::ApplyContext(UInputMappingContext* Context, int32 Priority)
{
    if (!GetWorld()) return;

    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        if (APlayerController* PC = It->Get())
        {
            if (ULocalPlayer* LP = PC->GetLocalPlayer())
            {
                if (UEnhancedInputLocalPlayerSubsystem* Subsys =
                    ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LP))
                {
                    Subsys->ClearAllMappings();
                    if (Context)
                    {
                        Subsys->AddMappingContext(Context, Priority);
                    }
                }
            }
        }
    }
}

void UInputManagerSubsystem::ClearContexts()
{
    ApplyContext(nullptr);
}

void UInputManagerSubsystem::BindPawnInputs(APawn* Pawn)
{
    if (!Pawn) return;

    if (Pawn->GetClass()->ImplementsInterface(UInputBindable::StaticClass()))
    {
        if (IInputBindable* Bindable = Cast<IInputBindable>(Pawn))
        {
            Bindable->BindInputs(this);
        }
        else
        {
            IInputBindable::Execute_BindInputs(Pawn, this); 
        }
    }
}

void UInputManagerSubsystem::BindAllPlayerPawns()
{
    if (!GetWorld()) return;

    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        if (APlayerController* PC = It->Get())
        {
            if (APawn* Pawn = PC->GetPawn())
            {
                BindPawnInputs(Pawn);
            }
        }
    }
}
