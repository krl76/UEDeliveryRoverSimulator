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
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rover|Input")
    UInputAction* ResetRoverAction;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rover|Movement")
    float MoveSpeed = 500.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rover|Movement")
    float BoostSpeed = 900.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rover|Movement")
    float BrakeMultiplier = 0.25f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rover|Movement")
    float TurnSpeed = 100.0f;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rover|Movement", meta = (AllowPrivateAccess = "true"))
    float MaxForwardSpeed = 850.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rover|Movement", meta = (AllowPrivateAccess = "true"))
    float MaxReverseSpeed = 420.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rover|Movement", meta = (AllowPrivateAccess = "true"))
    float BoostMaxSpeed = 1250.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rover|Movement", meta = (AllowPrivateAccess = "true"))
    float Acceleration = 520.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rover|Movement", meta = (AllowPrivateAccess = "true"))
    float Deceleration = 420.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rover|Movement", meta = (AllowPrivateAccess = "true"))
    float BrakeDeceleration = 1200.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rover|Movement", meta = (AllowPrivateAccess = "true"))
    float CurrentForwardSpeed = 0.0f;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rover|Steering", meta = (AllowPrivateAccess = "true"))
    float MaxTurnRate = 95.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rover|Steering", meta = (AllowPrivateAccess = "true"))
    float SteerInterpSpeed = 6.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rover|Steering", meta = (AllowPrivateAccess = "true"))
    float MinTurnMultiplier = 0.35f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rover|Steering", meta = (AllowPrivateAccess = "true"))
    float BoostTurnMultiplier = 0.75f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rover|Steering", meta = (AllowPrivateAccess = "true"))
    float CurrentSteerValue = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rover|Camera")
    float LookSensitivity = 0.7f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rover|Camera")
    float MinCameraPitch = -70.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rover|Camera")
    float MaxCameraPitch = -5.0f;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rover|Battery", meta = (AllowPrivateAccess = "true"))
    float BatteryPercent = 1.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rover|Battery", meta = (AllowPrivateAccess = "true"))
    float BatteryDrainRate = 0.015f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rover|Battery", meta = (AllowPrivateAccess = "true"))
    float BoostBatteryDrainMultiplier = 2.5f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rover|Battery", meta = (AllowPrivateAccess = "true"))
    float LowBatteryThreshold = 0.2f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rover|Battery", meta = (AllowPrivateAccess = "true"))
    float LowBatterySpeedMultiplier = 0.55f;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rover|Battery", meta = (AllowPrivateAccess = "true"))
    float EmptyBatterySpeedMultiplier = 0.25f;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rover|Ground", meta = (AllowPrivateAccess = "true"))
    float GroundTraceLength = 120.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rover|Ground", meta = (AllowPrivateAccess = "true"))
    float GroundCheckStartHeight = 40.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rover|Ground", meta = (AllowPrivateAccess = "true"))
    float MinUprightDot = 0.65f;

    UPROPERTY(BlueprintReadOnly, Category = "Rover|Ground", meta = (AllowPrivateAccess = "true"))
    bool bIsGrounded = false;

    UPROPERTY(BlueprintReadOnly, Category = "Rover|Ground", meta = (AllowPrivateAccess = "true"))
    bool bIsUpright = true;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rover|Collision", meta = (AllowPrivateAccess = "true"))
    float ObstacleTraceLength = 95.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rover|Collision", meta = (AllowPrivateAccess = "true"))
    float ObstacleTraceRadius = 28.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rover|Collision", meta = (AllowPrivateAccess = "true"))
    float ObstacleTraceHeight = 75.0f;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rover|Collision", meta = (AllowPrivateAccess = "true"))
    float BodyBlockTraceRadius = 42.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rover|Collision", meta = (AllowPrivateAccess = "true"))
    float BodyBlockTraceHeight = 65.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rover|Collision", meta = (AllowPrivateAccess = "true"))
    float WallNormalZLimit = 0.45f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rover|Collision", meta = (AllowPrivateAccess = "true"))
    float WallStopOffset = 5.0f;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rover|Battery", meta = (AllowPrivateAccess = "true"))
    float BatteryChargeRate = 0.25f;

    UPROPERTY(BlueprintReadOnly, Category = "Rover|Battery", meta = (AllowPrivateAccess = "true"))
    bool bIsCharging = false;

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
    
    void HandleResetRoverStarted();
    
    void UpdateBattery(float DeltaTime);
    
    bool CheckGrounded() const;
    bool CheckUpright() const;
    void KeepRoverLevel();
    void ResetRoverUpright();
    
    bool IsObstacleAhead(float DeltaTime) const;
    void MoveWithBlocking(const FVector& DeltaLocation);

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
    
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rover|Movement")
    float GetCurrentForwardSpeed() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rover|Battery")
    float GetBatteryPercent() const;
    
    UFUNCTION(BlueprintCallable, Category = "Rover|Battery")
    void StartCharging();

    UFUNCTION(BlueprintCallable, Category = "Rover|Battery")
    void StopCharging();

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rover|Battery")
    bool IsCharging() const;
};