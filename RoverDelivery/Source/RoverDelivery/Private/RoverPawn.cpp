// Fill out your copyright notice in the Description page of Project Settings.


#include "RoverDelivery/Public/RoverPawn.h"


// Sets default values
ARoverPawn::ARoverPawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ARoverPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARoverPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ARoverPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

