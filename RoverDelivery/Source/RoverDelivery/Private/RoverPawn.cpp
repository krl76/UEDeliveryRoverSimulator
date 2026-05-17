#include "RoverPawn.h"

#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"

ARoverPawn::ARoverPawn()
{
    PrimaryActorTick.bCanEverTick = true;

    CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
    RootComponent = CollisionBox;

    CollisionBox->SetBoxExtent(FVector(80.0f, 55.0f, 35.0f));
    CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    CollisionBox->SetCollisionObjectType(ECC_Pawn);
    CollisionBox->SetCollisionResponseToAllChannels(ECR_Block);

    RoverMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RoverMesh"));
    RoverMesh->SetupAttachment(CollisionBox);
    RoverMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

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

    MoveRover(DeltaTime);
    RotateRover(DeltaTime);
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
    bBoosting = true;
}

void ARoverPawn::HandleBoostCompleted()
{
    bBoosting = false;
}

void ARoverPawn::HandleBrakeStarted()
{
    bBraking = true;
}

void ARoverPawn::HandleBrakeCompleted()
{
    bBraking = false;
}

void ARoverPawn::HandleLookX(const FInputActionValue& Value)
{
    const float LookAmount = Value.Get<float>();

    CameraYaw += LookAmount * LookSensitivity;
    ApplyCameraRotation();
}

void ARoverPawn::HandleLookY(const FInputActionValue& Value)
{
    const float LookAmount = Value.Get<float>();

    CameraPitch += LookAmount * LookSensitivity;
    CameraPitch = FMath::Clamp(CameraPitch, MinCameraPitch, MaxCameraPitch);

    ApplyCameraRotation();
}

void ARoverPawn::HandleInteractStarted()
{
    OnInteractPressed();
}

void ARoverPawn::HandlePauseStarted()
{
    OnPausePressed();
}

void ARoverPawn::HandleCameraToggleStarted()
{
    OnCameraTogglePressed();
}

void ARoverPawn::MoveRover(float DeltaTime)
{
    if (FMath::IsNearlyZero(ThrottleValue))
    {
        return;
    }

    float CurrentSpeed = MoveSpeed;

    if (bBoosting)
    {
        CurrentSpeed = BoostSpeed;
    }

    if (bBraking)
    {
        CurrentSpeed *= BrakeMultiplier;
    }

    const FVector ForwardDirection = GetActorForwardVector();
    const FVector DeltaLocation = ForwardDirection * ThrottleValue * CurrentSpeed * DeltaTime;

    AddActorWorldOffset(DeltaLocation, true);
}

void ARoverPawn::RotateRover(float DeltaTime)
{
    if (FMath::IsNearlyZero(SteerValue))
    {
        return;
    }

    const float DeltaYaw = SteerValue * TurnSpeed * DeltaTime;
    const FRotator DeltaRotation = FRotator(0.0f, DeltaYaw, 0.0f);

    AddActorLocalRotation(DeltaRotation, true);
}

void ARoverPawn::ApplyCameraRotation()
{
    if (SpringArm)
    {
        SpringArm->SetRelativeRotation(FRotator(CameraPitch, CameraYaw, 0.0f));
    }
}