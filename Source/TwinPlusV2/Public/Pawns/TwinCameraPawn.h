// TwinCameraPawn.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InputActionValue.h"
#include "TwinCameraPawn.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UInputMappingContext;
class UInputAction;
class UFloatingPawnMovement;

UCLASS()
class TWINPLUSV2_API ATwinCameraPawn : public APawn
{
    GENERATED_BODY()

public:
    ATwinCameraPawn();

protected:
    virtual void BeginPlay() override;
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

    void MoveCamera(const FInputActionValue& Value);
    void ZoomCamera(const FInputActionValue& Value);

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
    USpringArmComponent* SpringArm;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
    UCameraComponent* Camera;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
    UFloatingPawnMovement* MovementComp;

    UPROPERTY(EditDefaultsOnly, Category="Input")
    UInputMappingContext* CameraMappingContext;

    UPROPERTY(EditDefaultsOnly, Category="Input")
    UInputAction* MoveCameraAction;

    UPROPERTY(EditDefaultsOnly, Category="Input")
    UInputAction* ZoomCameraAction;
};
