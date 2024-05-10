// Copyright Epic Games, Inc. All Rights Reserved.

#include "ARPGPlayerController.h"
#include "GameFramework/Pawn.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "ARPGCharacter.h"
#include "Interfaces/Hoverable.h"
#include "Engine/World.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"

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
		if (HitActor)
		{
			if (HitActor != HoveredActor)
			{
				if (HitActor->GetClass()->ImplementsInterface(UHoverable::StaticClass()))
				{
					HoveredActor = HitActor;
					HoveredActorHoverable = Cast<IHoverable>(HoveredActor);
				}
				else
				{
					HoveredActor = nullptr;
					HoveredActorHoverable = nullptr;
				}
			}

			if (HoveredActorHoverable)
			{
				HoveredActorHoverable->OnHover();
			}
		}
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
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Started, this, &AARPGPlayerController::OnInputStarted);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Triggered, this, &AARPGPlayerController::OnSetDestinationTriggered);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Completed, this, &AARPGPlayerController::OnSetDestinationReleased);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Canceled, this, &AARPGPlayerController::OnSetDestinationReleased);

		// Setup touch input events
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Started, this, &AARPGPlayerController::OnInputStarted);
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Triggered, this, &AARPGPlayerController::OnTouchTriggered);
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Completed, this, &AARPGPlayerController::OnTouchReleased);
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Canceled, this, &AARPGPlayerController::OnTouchReleased);

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
void AARPGPlayerController::OnSetDestinationTriggered()
{
	FollowTime += GetWorld()->GetDeltaSeconds();

	AARPGCharacter* ControlledPawn = Cast<AARPGCharacter>(GetPawn());
	if (ControlledPawn != nullptr)
	{
		FVector WorldDirection = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();

		if (bIsInForceAttackMode)
		{
			FRotator Rotation = FRotator(0.f, WorldDirection.Rotation().Yaw, 0.f);
			// Rotate the pawn
			ControlledPawn->SetActorRotation(Rotation);
			ControlledPawn->Attack();
			return;
		}

		ControlledPawn->AddMovementInput(WorldDirection, 1.0, false);
	}
}

void AARPGPlayerController::OnSetDestinationReleased()
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
	OnSetDestinationTriggered();
}

void AARPGPlayerController::OnTouchReleased()
{
	bIsTouch = false;
	OnSetDestinationReleased();
}

void AARPGPlayerController::OnForceAttackStarted()
{
	bIsInForceAttackMode = true;

	AARPGCharacter* ControlledPawn = Cast<AARPGCharacter>(GetPawn());
	if (ControlledPawn != nullptr)
	{
		ControlledPawn->SetForceAttackMode(true);
	}
}

void AARPGPlayerController::OnForceAttackEnded()
{
	bIsInForceAttackMode = false;

	AARPGCharacter* ControlledPawn = Cast<AARPGCharacter>(GetPawn());
	if (ControlledPawn != nullptr)
	{
		ControlledPawn->SetForceAttackMode(false);
	}
}
