// Copyright Epic Games, Inc. All Rights Reserved.

#include "ARPGPlayerController.h"

#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/Pawn.h"
#include "GenericPlatform/ICursor.h"
#include "InputActionValue.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"

#include "ARPGCharacter.h"
#include "MobType/BaseMobType.h"
#include "WeaponActor.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

AARPGPlayerController::AARPGPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	CachedDestination = FVector::ZeroVector;
	FollowTime = 0.f;
}

void AARPGPlayerController::BeginPlay()
{
	Super::BeginPlay();

	ControlledPawn = Cast<AARPGCharacter>(GetPawn());
}

void AARPGPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	FHitResult Hit;
	bool bHitSuccessful = false;
	if (bIsTouch)
	{
		bHitSuccessful = GetHitResultUnderFinger(ETouchIndex::Touch1, ECollisionChannel::ECC_Visibility, true, Hit);
	}
	else
	{
		bHitSuccessful = GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit);
	}

	if (bHitSuccessful)
	{
		CachedDestination = Hit.Location;
		AActor* HitActor = Hit.GetActor();
		if (PrevTargetActor != HitActor)
		{
			PrevTargetActor = CurrentTargetActor;
			CurrentTargetActor = HitActor;
			CurrentTargetMobActor = Cast<ABaseMobType>(HitActor);
		}
	}
	else
	{
		PrevTargetActor = CurrentTargetActor;
		CurrentTargetActor = nullptr;
		CurrentTargetMobActor = nullptr;
	}
}

void AARPGPlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	// Add Input Mapping Context
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		// Setup mouse input events
		EnhancedInputComponent->BindAction(PrimaryClickAction, ETriggerEvent::Started, this, &AARPGPlayerController::OnInputStarted);
		EnhancedInputComponent->BindAction(PrimaryClickAction, ETriggerEvent::Triggered, this, &AARPGPlayerController::OnPrimaryActionTriggered);
		EnhancedInputComponent->BindAction(PrimaryClickAction, ETriggerEvent::Completed, this, &AARPGPlayerController::OnPrimaryActionReleased);
		EnhancedInputComponent->BindAction(PrimaryClickAction, ETriggerEvent::Canceled, this, &AARPGPlayerController::OnPrimaryActionReleased);

		// Setup touch input events
		EnhancedInputComponent->BindAction(PrimaryTouchAction, ETriggerEvent::Started, this, &AARPGPlayerController::OnInputStarted);
		EnhancedInputComponent->BindAction(PrimaryTouchAction, ETriggerEvent::Triggered, this, &AARPGPlayerController::OnTouchTriggered);
		EnhancedInputComponent->BindAction(PrimaryTouchAction, ETriggerEvent::Completed, this, &AARPGPlayerController::OnTouchReleased);
		EnhancedInputComponent->BindAction(PrimaryTouchAction, ETriggerEvent::Canceled, this, &AARPGPlayerController::OnTouchReleased);

		EnhancedInputComponent->BindAction(ForceAttackAction, ETriggerEvent::Started, this, &AARPGPlayerController::OnForceAttackStarted);
		EnhancedInputComponent->BindAction(ForceAttackAction, ETriggerEvent::Completed, this, &AARPGPlayerController::OnForceAttackEnded);
		EnhancedInputComponent->BindAction(ForceAttackAction, ETriggerEvent::Canceled, this, &AARPGPlayerController::OnForceAttackEnded);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AARPGPlayerController::OnInputStarted()
{
	StopMovement();
}

// Triggered every frame when the input is held down
void AARPGPlayerController::OnPrimaryActionTriggered()
{
	FollowTime += GetWorld()->GetDeltaSeconds();

	if (ControlledPawn == nullptr) return;

	FVector WorldDirection = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
	FRotator Rotation = FRotator(0.f, WorldDirection.Rotation().Yaw, 0.f);
	ControlledPawn->SetActorRotation(Rotation);

	if (ControlledPawn->TryAttack(CurrentTargetMobActor))
	{
		StopMovement();
		return;
	}

	if (bIsInForceAttackMode)
	{
		// TODO: Force attack
		return;
	}

	ControlledPawn->AddMovementInput(WorldDirection, 1.0, false);
}

void AARPGPlayerController::OnPrimaryActionReleased()
{
	if (bIsInForceAttackMode) return;

	// If it was a short press
	if (FollowTime <= ShortPressThreshold)
	{
		// We move there and spawn some particles
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, CachedDestination);
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, FXCursor, CachedDestination, FRotator::ZeroRotator, FVector(1.f, 1.f, 1.f), true, true, ENCPoolMethod::None, true);
	}

	FollowTime = 0.f;
}

// Triggered every frame when the input is held down
void AARPGPlayerController::OnTouchTriggered()
{
	bIsTouch = true;
	OnPrimaryActionTriggered();
}

void AARPGPlayerController::OnTouchReleased()
{
	bIsTouch = false;
	OnPrimaryActionReleased();
}

void AARPGPlayerController::OnForceAttackStarted()
{
	bIsInForceAttackMode = true;

	if (ControlledPawn != nullptr)
	{
		ControlledPawn->SetForceAttackMode(true);
	}
}

void AARPGPlayerController::OnForceAttackEnded()
{
	bIsInForceAttackMode = false;

	if (ControlledPawn != nullptr)
	{
		ControlledPawn->SetForceAttackMode(false);
	}
}
