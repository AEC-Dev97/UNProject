#include "Pawns/TwinCameraPawn.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"

ATwinCameraPawn::ATwinCameraPawn()
{
    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    RootComponent = SpringArm;
    SpringArm->TargetArmLength = 800.f;
    SpringArm->bDoCollisionTest = false;

    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(SpringArm);

    MovementComp = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("MovementComp"));
    MovementComp->MaxSpeed = 1200.f;

    AutoPossessPlayer = EAutoReceiveInput::Player0; 
}

void ATwinCameraPawn::BeginPlay()
{
    Super::BeginPlay();

    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
            ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
        {
            if (CameraMappingContext)
            {
                Subsystem->AddMappingContext(CameraMappingContext, 0);
            }
        }
    }
}

void ATwinCameraPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        if (MoveCameraAction)
            EIC->BindAction(MoveCameraAction, ETriggerEvent::Triggered, this, &ATwinCameraPawn::MoveCamera);

        if (ZoomCameraAction)
            EIC->BindAction(ZoomCameraAction, ETriggerEvent::Triggered, this, &ATwinCameraPawn::ZoomCamera);
    }
}

void ATwinCameraPawn::MoveCamera(const FInputActionValue& Value)
{
    FVector2D Movement = Value.Get<FVector2D>();
    AddMovementInput(FVector(Movement.Y, Movement.X, 0)); 
}

void ATwinCameraPawn::ZoomCamera(const FInputActionValue& Value)
{
    float Zoom = Value.Get<float>();
    SpringArm->TargetArmLength = FMath::Clamp(SpringArm->TargetArmLength - Zoom * 50.f, 300.f, 2000.f);
}
