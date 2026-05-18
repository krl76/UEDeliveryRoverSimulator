#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InputActionValue.h"
#include "RoverPawn.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;

UCLASS()
class ROVERDELIVERY_API ARoverPawn : public APawn
{
    GENERATED_BODY()

public:
    ARoverPawn();

protected:
    virtual void BeginPlay() override;
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

public:
    virtual void Tick(float DeltaTime) override;

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rover|Components")
    UBoxComponent* CollisionBox;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rover|Components")
    UStaticMeshComponent* RoverMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rover|Components")
    USpringArmComponent* SpringArm;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rover|Components")
    UCameraComponent* Camera;

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rover|Input")
    UInputMappingContext* RoverMappingContext;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rover|Input")
    UInputAction* ThrottleAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rover|Input")
    UInputAction* SteerAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rover|Input")
    UInputAction* BoostAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rover|Input")
    UInputAction* BrakeAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rover|Input")
    UInputAction* InteractAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rover|Input")
    UInputAction* PauseAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rover|Input")
    UInputAction* CameraToggleAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rover|Input")
    UInputAction* LookXAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rover|Input")
    UInputAction* LookYAction;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rover|Input")
    UInputAction* TogglePhoneAction;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rover|Movement")
    float MoveSpeed = 500.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rover|Movement")
    float BoostSpeed = 900.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rover|Movement")
    float BrakeMultiplier = 0.25f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rover|Movement")
    float TurnSpeed = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rover|Camera")
    float LookSensitivity = 0.7f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rover|Camera")
    float MinCameraPitch = -70.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rover|Camera")
    float MaxCameraPitch = -5.0f;

private:
    float ThrottleValue = 0.0f;
    float SteerValue = 0.0f;

    bool bBoosting = false;
    bool bBraking = false;

    float CameraYaw = 0.0f;
    float CameraPitch = -25.0f;
    
    bool bRoverInputBlocked = false;

private:
    void HandleThrottle(const FInputActionValue& Value);
    void HandleThrottleCompleted(const FInputActionValue& Value);

    void HandleSteer(const FInputActionValue& Value);
    void HandleSteerCompleted(const FInputActionValue& Value);

    void HandleBoostStarted();
    void HandleBoostCompleted();

    void HandleBrakeStarted();
    void HandleBrakeCompleted();

    void HandleLookX(const FInputActionValue& Value);
    void HandleLookY(const FInputActionValue& Value);

    void HandleInteractStarted();
    void HandlePauseStarted();
    void HandleCameraToggleStarted();

    void MoveRover(float DeltaTime);
    void RotateRover(float DeltaTime);
    void ApplyCameraRotation();
    
    void HandleTogglePhoneStarted();

protected:
    UFUNCTION(BlueprintImplementableEvent, Category = "Rover|Events")
    void OnInteractPressed();

    UFUNCTION(BlueprintImplementableEvent, Category = "Rover|Events")
    void OnPausePressed();

    UFUNCTION(BlueprintImplementableEvent, Category = "Rover|Events")
    void OnCameraTogglePressed();
    
    UFUNCTION(BlueprintImplementableEvent, Category = "Rover|Events")
    void OnPhoneTogglePressed();
    
    UFUNCTION(BlueprintCallable, Category = "Rover|Input")
    void SetRoverInputBlocked(bool bBlocked);
};