#include "RoverPawn.h"

#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"

ARoverPawn::ARoverPawn()
{
    PrimaryActorTick.bCanEverTick = true;

    CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
    RootComponent = CollisionBox;

    CollisionBox->SetBoxExtent(FVector(70.0f, 50.0f, 30.0f));
    CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    CollisionBox->SetCollisionObjectType(ECC_Pawn);
    CollisionBox->SetGenerateOverlapEvents(true);

    CollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
    CollisionBox->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);

    RoverMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RoverMesh"));
    RoverMesh->SetupAttachment(CollisionBox);
    RoverMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    RoverMesh->SetSimulatePhysics(false);

    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArm->SetupAttachment(CollisionBox);
    SpringArm->TargetArmLength = 500.0f;
    SpringArm->SetRelativeLocation(FVector(0.0f, 0.0f, 80.0f));
    SpringArm->bUsePawnControlRotation = false;
    SpringArm->bEnableCameraLag = true;
    SpringArm->CameraLagSpeed = 8.0f;

    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(SpringArm);
    Camera->bUsePawnControlRotation = false;
    
    BoostLoopAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("BoostLoopAudio"));
    BoostLoopAudio->SetupAttachment(CollisionBox);
    BoostLoopAudio->bAutoActivate = false;

    BrakeLoopAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("BrakeLoopAudio"));
    BrakeLoopAudio->SetupAttachment(CollisionBox);
    BrakeLoopAudio->bAutoActivate = false;

    ChargingLoopAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("ChargingLoopAudio"));
    ChargingLoopAudio->SetupAttachment(CollisionBox);
    ChargingLoopAudio->bAutoActivate = false;

    AutoPossessPlayer = EAutoReceiveInput::Player0;
}

void ARoverPawn::BeginPlay()
{
    Super::BeginPlay();

    ApplyCameraRotation();

    APlayerController* PlayerController = Cast<APlayerController>(GetController());
    if (!PlayerController)
    {
        return;
    }

    ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();
    if (!LocalPlayer)
    {
        return;
    }

    UEnhancedInputLocalPlayerSubsystem* InputSubsystem =
        LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();

    if (InputSubsystem && RoverMappingContext)
    {
        InputSubsystem->AddMappingContext(RoverMappingContext, 0);
    }
}

void ARoverPawn::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    bIsGrounded = CheckGrounded();
    bIsUpright = CheckUpright();

    //KeepRoverLevel();

    MoveRover(DeltaTime);
    RotateRover(DeltaTime);
    UpdateBattery(DeltaTime);
}

void ARoverPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
    if (!EnhancedInputComponent)
    {
        return;
    }

    if (ThrottleAction)
    {
        EnhancedInputComponent->BindAction(ThrottleAction, ETriggerEvent::Triggered, this, &ARoverPawn::HandleThrottle);
        EnhancedInputComponent->BindAction(ThrottleAction, ETriggerEvent::Completed, this, &ARoverPawn::HandleThrottleCompleted);
    }

    if (SteerAction)
    {
        EnhancedInputComponent->BindAction(SteerAction, ETriggerEvent::Triggered, this, &ARoverPawn::HandleSteer);
        EnhancedInputComponent->BindAction(SteerAction, ETriggerEvent::Completed, this, &ARoverPawn::HandleSteerCompleted);
    }

    if (BoostAction)
    {
        EnhancedInputComponent->BindAction(BoostAction, ETriggerEvent::Started, this, &ARoverPawn::HandleBoostStarted);
        EnhancedInputComponent->BindAction(BoostAction, ETriggerEvent::Completed, this, &ARoverPawn::HandleBoostCompleted);
    }

    if (BrakeAction)
    {
        EnhancedInputComponent->BindAction(BrakeAction, ETriggerEvent::Started, this, &ARoverPawn::HandleBrakeStarted);
        EnhancedInputComponent->BindAction(BrakeAction, ETriggerEvent::Completed, this, &ARoverPawn::HandleBrakeCompleted);
    }

    if (LookXAction)
    {
        EnhancedInputComponent->BindAction(LookXAction, ETriggerEvent::Triggered, this, &ARoverPawn::HandleLookX);
    }

    if (LookYAction)
    {
        EnhancedInputComponent->BindAction(LookYAction, ETriggerEvent::Triggered, this, &ARoverPawn::HandleLookY);
    }

    if (InteractAction)
    {
        EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &ARoverPawn::HandleInteractStarted);
    }

    if (PauseAction)
    {
        EnhancedInputComponent->BindAction(PauseAction, ETriggerEvent::Started, this, &ARoverPawn::HandlePauseStarted);
    }

    if (CameraToggleAction)
    {
        EnhancedInputComponent->BindAction(CameraToggleAction, ETriggerEvent::Started, this, &ARoverPawn::HandleCameraToggleStarted);
    }
    
    if (TogglePhoneAction)
    {
        EnhancedInputComponent->BindAction(
            TogglePhoneAction,
            ETriggerEvent::Started,
            this,
            &ARoverPawn::HandleTogglePhoneStarted
        );
    }
    
    if (ResetRoverAction)
    {
        EnhancedInputComponent->BindAction(
            ResetRoverAction,
            ETriggerEvent::Started,
            this,
            &ARoverPawn::HandleResetRoverStarted
        );
    }
}

void ARoverPawn::HandleThrottle(const FInputActionValue& Value)
{
    ThrottleValue = Value.Get<float>();
}

void ARoverPawn::HandleThrottleCompleted(const FInputActionValue& Value)
{
    ThrottleValue = 0.0f;
}

void ARoverPawn::HandleSteer(const FInputActionValue& Value)
{
    SteerValue = Value.Get<float>();
}

void ARoverPawn::HandleSteerCompleted(const FInputActionValue& Value)
{
    SteerValue = 0.0f;
}

void ARoverPawn::HandleBoostStarted()
{
    if (bRoverInputBlocked || bIsCharging)
    {
        return;
    }

    bBoosting = true;

    const bool bCanBoost =
        BatteryPercent > 0.05f &&
        ThrottleValue > 0.0f &&
        FMath::Abs(CurrentForwardSpeed) > 50.0f;

    if (bCanBoost)
    {
        StartLoopSound(BoostLoopAudio, BoostLoopSound, BoostLoopVolume);
    }
}

void ARoverPawn::HandleBoostCompleted()
{
    bBoosting = false;
    StopLoopSound(BoostLoopAudio);
}

void ARoverPawn::HandleBrakeStarted()
{
    if (bRoverInputBlocked || bIsCharging)
    {
        return;
    }

    const bool bWasMoving = FMath::Abs(CurrentForwardSpeed) >= BrakeSoundMinSpeed;

    bBraking = true;

    if (bWasMoving)
    {
        StartLoopSound(BrakeLoopAudio, BrakeLoopSound, BrakeLoopVolume);
    }
}

void ARoverPawn::HandleBrakeCompleted()
{
    bBraking = false;
    StopLoopSound(BrakeLoopAudio);
}

void ARoverPawn::HandleLookX(const FInputActionValue& Value)
{
    if (bRoverInputBlocked)
    {
        return;
    }
    
    const float LookAmount = Value.Get<float>();

    CameraYaw += LookAmount * LookSensitivity;
    ApplyCameraRotation();
}

void ARoverPawn::HandleLookY(const FInputActionValue& Value)
{
    if (bRoverInputBlocked)
    {
        return;
    }
    
    const float LookAmount = Value.Get<float>();

    CameraPitch += LookAmount * LookSensitivity;
    CameraPitch = FMath::Clamp(CameraPitch, MinCameraPitch, MaxCameraPitch);

    ApplyCameraRotation();
}

void ARoverPawn::SetLookSensitivity(float NewSensitivity)
{
    LookSensitivity = FMath::Clamp(NewSensitivity, 0.3f, 2.5f);
}

void ARoverPawn::HandleInteractStarted()
{
    OnInteractPressed();
}

void ARoverPawn::HandlePauseStarted()
{
    UE_LOG(LogTemp, Warning, TEXT("Pause input received"));

    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(
            -1,
            2.0f,
            FColor::Yellow,
            TEXT("Pause input received")
        );
    }

    OnPausePressed();
}

void ARoverPawn::HandleCameraToggleStarted()
{
    OnCameraTogglePressed();
}

void ARoverPawn::MoveRover(float DeltaTime)
{
    if (bIsCharging)
    {
        CurrentForwardSpeed = 0.0f;
        return;
    }
    
    if (bRoverInputBlocked)
    {
        CurrentForwardSpeed = 0.0f;
        return;
    }
    
    if (!bIsGrounded || !bIsUpright)
    {
        CurrentForwardSpeed = FMath::FInterpConstantTo(
            CurrentForwardSpeed,
            0.0f,
            DeltaTime,
            BrakeDeceleration
        );
        return;
    }

    float TargetSpeed = 0.0f;

    const bool bHasThrottleInput = !FMath::IsNearlyZero(ThrottleValue);
    const bool bCanBoost = bBoosting && BatteryPercent > 0.05f && ThrottleValue > 0.0f;

    if (bHasThrottleInput)
    {
        if (ThrottleValue > 0.0f)
        {
            TargetSpeed = bCanBoost ? BoostMaxSpeed : MaxForwardSpeed;
            TargetSpeed *= ThrottleValue;
        }
        else
        {
            TargetSpeed = MaxReverseSpeed * ThrottleValue;
        }
    }

    if (bBraking)
    {
        TargetSpeed = 0.0f;
    }

    if (BatteryPercent <= 0.0f)
    {
        TargetSpeed *= EmptyBatterySpeedMultiplier;
    }
    else if (BatteryPercent <= LowBatteryThreshold)
    {
        TargetSpeed *= LowBatterySpeedMultiplier;
    }

    float SpeedChangeRate = Acceleration;

    if (bBraking)
    {
        SpeedChangeRate = BrakeDeceleration;
    }
    else if (!bHasThrottleInput)
    {
        SpeedChangeRate = Deceleration;
    }

    CurrentForwardSpeed = FMath::FInterpConstantTo(
        CurrentForwardSpeed,
        TargetSpeed,
        DeltaTime,
        SpeedChangeRate
    );
    
    if (!bHasThrottleInput && FMath::Abs(CurrentForwardSpeed) < 25.0f)
    {
        CurrentForwardSpeed = 0.0f;
        return;
    }
    
    if (FMath::IsNearlyZero(CurrentForwardSpeed, 2.0f))
    {
        CurrentForwardSpeed = 0.0f;
        return;
    }

    FVector ForwardDirection = GetActorForwardVector();
    ForwardDirection.Z = 0.0f;
    ForwardDirection.Normalize();

    const FVector DeltaLocation = ForwardDirection * CurrentForwardSpeed * DeltaTime;
    
    MoveWithBlocking(DeltaLocation);
}

void ARoverPawn::RotateRover(float DeltaTime)
{
    if (bIsCharging)
    {
        CurrentSteerValue = 0.0f;
        return;
    }
    
    if (bRoverInputBlocked)
    {
        CurrentSteerValue = 0.0f;
        return;
    }
    
    if (!bIsGrounded || !bIsUpright)
    {
        CurrentSteerValue = 0.0f;
        return;
    }

    CurrentSteerValue = FMath::FInterpTo(
        CurrentSteerValue,
        SteerValue,
        DeltaTime,
        SteerInterpSpeed
    );

    if (FMath::IsNearlyZero(CurrentSteerValue, 0.01f))
    {
        CurrentSteerValue = 0.0f;
        return;
    }

    const float SpeedAlpha = FMath::Clamp(
        FMath::Abs(CurrentForwardSpeed) / MaxForwardSpeed,
        0.0f,
        1.0f
    );

    float TurnMultiplier = FMath::Lerp(MinTurnMultiplier, 1.0f, SpeedAlpha);

    if (bBoosting)
    {
        TurnMultiplier *= BoostTurnMultiplier;
    }

    const float DirectionMultiplier = CurrentForwardSpeed < -5.0f ? -1.0f : 1.0f;

    const float DeltaYaw =
        CurrentSteerValue *
        MaxTurnRate *
        TurnMultiplier *
        DirectionMultiplier *
        DeltaTime;

    AddActorLocalRotation(FRotator(0.0f, DeltaYaw, 0.0f), false);
}

bool ARoverPawn::CheckGrounded() const
{
    const FVector Start = GetActorLocation() + FVector(0.0f, 0.0f, GroundCheckStartHeight);
    const FVector End = Start - FVector(0.0f, 0.0f, GroundTraceLength);

    FHitResult HitResult;

    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);

    const bool bHit = GetWorld()->LineTraceSingleByChannel(
        HitResult,
        Start,
        End,
        ECC_Visibility,
        Params
    );

    return bHit;
}

bool ARoverPawn::CheckUpright() const
{
    const float UprightDot = FVector::DotProduct(GetActorUpVector(), FVector::UpVector);
    return UprightDot >= MinUprightDot;
}

void ARoverPawn::KeepRoverLevel()
{
    FRotator CurrentRotation = GetActorRotation();

    CurrentRotation.Pitch = 0.0f;
    CurrentRotation.Roll = 0.0f;

    SetActorRotation(CurrentRotation);
}

void ARoverPawn::HandleResetRoverStarted()
{
    ResetRoverUpright();
}

void ARoverPawn::ResetRoverUpright()
{
    FVector NewLocation = GetActorLocation();

    const FVector Start = NewLocation + FVector(0.0f, 0.0f, 300.0f);
    const FVector End = NewLocation - FVector(0.0f, 0.0f, 800.0f);

    FHitResult HitResult;

    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);

    const bool bHit = GetWorld()->LineTraceSingleByChannel(
        HitResult,
        Start,
        End,
        ECC_Visibility,
        Params
    );

    if (bHit)
    {
        NewLocation = HitResult.Location + FVector(0.0f, 0.0f, 70.0f);
    }
    else
    {
        NewLocation += FVector(0.0f, 0.0f, 120.0f);
    }

    FRotator NewRotation = GetActorRotation();
    NewRotation.Pitch = 0.0f;
    NewRotation.Roll = 0.0f;

    SetActorLocationAndRotation(NewLocation, NewRotation, false, nullptr, ETeleportType::TeleportPhysics);

    CurrentForwardSpeed = 0.0f;
    CurrentSteerValue = 0.0f;
    ThrottleValue = 0.0f;
    SteerValue = 0.0f;
    bBoosting = false;
    bBraking = false;
}

void ARoverPawn::ApplyCameraRotation()
{
    if (SpringArm)
    {
        SpringArm->SetRelativeRotation(FRotator(CameraPitch, CameraYaw, 0.0f));
    }
}

void ARoverPawn::HandleTogglePhoneStarted()
{
    UE_LOG(LogTemp, Warning, TEXT("Phone toggle input received"));
    OnPhoneTogglePressed();
}

void ARoverPawn::SetRoverInputBlocked(bool bBlocked)
{
    bRoverInputBlocked = bBlocked;

    ThrottleValue = 0.0f;
    SteerValue = 0.0f;
    CurrentSteerValue = 0.0f;
    CurrentForwardSpeed = 0.0f;
    bBoosting = false;
    bBraking = false;

    StopLoopSound(BoostLoopAudio);
    StopLoopSound(BrakeLoopAudio);
}

float ARoverPawn::GetBatteryPercent() const
{
    return BatteryPercent;
}

float ARoverPawn::GetCurrentForwardSpeed() const
{
    return CurrentForwardSpeed;
}

void ARoverPawn::UpdateBattery(float DeltaTime)
{
    if (bIsCharging)
    {
        BatteryPercent += BatteryChargeRate * DeltaTime;
        BatteryPercent = FMath::Clamp(BatteryPercent, 0.0f, 1.0f);
        
        if (BatteryPercent >= LowBatteryWarningResetThreshold)
        {
            bLowBatteryWarningPlayed = false;
        }
        
        if (BatteryPercent >= 1.0f)
        {
            BatteryPercent = 1.0f;

            StopCharging();
            PlayOneShotSound(ChargeCompleteSound, ChargingLoopVolume);
        }

        return;
    }
    
    if (bRoverInputBlocked)
    {
        return;
    }

    if (BatteryPercent <= 0.0f)
    {
        BatteryPercent = 0.0f;
        return;
    }

    const bool bMotorIsWorking =
        FMath::Abs(ThrottleValue) > 0.05f ||
        FMath::Abs(CurrentForwardSpeed) > 50.0f;

    if (!bMotorIsWorking)
    {
        return;
    }

    float CurrentDrainRate = BatteryDrainRate;

    if (bBoosting && ThrottleValue > 0.05f)
    {
        CurrentDrainRate *= BoostBatteryDrainMultiplier;
    }

    BatteryPercent -= CurrentDrainRate * DeltaTime;
    BatteryPercent = FMath::Clamp(BatteryPercent, 0.0f, 1.0f);
    
    if (BatteryPercent <= LowBatteryThreshold && !bLowBatteryWarningPlayed)
    {
        PlayOneShotSound(LowBatterySound, LowBatterySoundVolume);
        bLowBatteryWarningPlayed = true;
    }
}

bool ARoverPawn::IsObstacleAhead(float DeltaTime) const
{
    if (FMath::Abs(CurrentForwardSpeed) < 20.0f)
    {
        return false;
    }

    FVector Direction = GetActorForwardVector();
    Direction.Z = 0.0f;
    Direction.Normalize();

    if (CurrentForwardSpeed < 0.0f)
    {
        Direction *= -1.0f;
    }

    const FVector Start = GetActorLocation() + FVector(0.0f, 0.0f, ObstacleTraceHeight);
    const FVector End = Start + Direction * (ObstacleTraceLength + FMath::Abs(CurrentForwardSpeed) * DeltaTime);

    FHitResult HitResult;

    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);

    const bool bHit = GetWorld()->SweepSingleByChannel(
        HitResult,
        Start,
        End,
        FQuat::Identity,
        ECC_Visibility,
        FCollisionShape::MakeSphere(ObstacleTraceRadius),
        Params
    );

    if (!bHit)
    {
        return false;
    }
    
    if (HitResult.ImpactNormal.Z > 0.45f)
    {
        return false;
    }

    return true;
}

void ARoverPawn::MoveWithBlocking(const FVector& DeltaLocation)
{
    if (DeltaLocation.IsNearlyZero())
    {
        return;
    }

    const FVector Start = GetActorLocation() + FVector(0.0f, 0.0f, BodyBlockTraceHeight);
    const FVector End = Start + DeltaLocation;

    FHitResult HitResult;

    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);
    Params.bTraceComplex = true;

    const bool bHit = GetWorld()->SweepSingleByChannel(
        HitResult,
        Start,
        End,
        FQuat::Identity,
        ECC_Visibility,
        FCollisionShape::MakeSphere(BodyBlockTraceRadius),
        Params
    );

    if (!bHit)
    {
        AddActorWorldOffset(DeltaLocation, false);
        return;
    }
    
    if (HitResult.ImpactNormal.Z > WallNormalZLimit)
    {
        AddActorWorldOffset(DeltaLocation, false);
        return;
    }

    const FVector MoveDirection = DeltaLocation.GetSafeNormal();
    const float SafeDistance = FMath::Max(0.0f, HitResult.Distance - WallStopOffset);

    if (SafeDistance > 1.0f)
    {
        AddActorWorldOffset(MoveDirection * SafeDistance, false);
    }

    CurrentForwardSpeed = 0.0f;
}

void ARoverPawn::StartCharging()
{
    if (bIsCharging)
    {
        return;
    }

    bIsCharging = true;

    CurrentForwardSpeed = 0.0f;
    CurrentSteerValue = 0.0f;
    ThrottleValue = 0.0f;
    SteerValue = 0.0f;
    bBoosting = false;
    bBraking = false;

    StopLoopSound(BoostLoopAudio);
    StopLoopSound(BrakeLoopAudio);

    PlayOneShotSound(StartChargingSound, ChargingLoopVolume);
    StartLoopSound(ChargingLoopAudio, ChargingLoopSound, ChargingLoopVolume);
}

void ARoverPawn::StopCharging()
{
    if (!bIsCharging)
    {
        return;
    }

    bIsCharging = false;
    StopLoopSound(ChargingLoopAudio);
}

bool ARoverPawn::IsCharging() const
{
    return bIsCharging;
}

void ARoverPawn::StartLoopSound(UAudioComponent* AudioComponent, USoundBase* Sound, float Volume)
{
    if (!AudioComponent || !Sound)
    {
        return;
    }

    if (AudioComponent->IsPlaying())
    {
        return;
    }

    AudioComponent->SetSound(Sound);
    AudioComponent->SetVolumeMultiplier(Volume);
    AudioComponent->FadeIn(AudioFadeTime, Volume);
}

void ARoverPawn::StopLoopSound(UAudioComponent* AudioComponent)
{
    if (!AudioComponent || !AudioComponent->IsPlaying())
    {
        return;
    }

    AudioComponent->FadeOut(AudioFadeTime, 0.0f);
}

void ARoverPawn::PlayOneShotSound(USoundBase* Sound, float Volume) const
{
    if (!Sound)
    {
        return;
    }

    UGameplayStatics::PlaySoundAtLocation(
        this,
        Sound,
        GetActorLocation(),
        Volume
    );
}