// Copyright Epic Games, Inc. All Rights Reserved.

#include "ARPGCharacter.h"

#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Materials/Material.h"
#include "Engine/World.h"

#include "MobType/BaseMobType.h"
#include "WeaponActor.h"

AARPGCharacter::AARPGCharacter()
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void AARPGCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsAttacking)
	{
		AttackTimer += DeltaTime;

		if (AttackTimer >= AttackDuration)
		{
			AttackTimer = 0.0f;
			bIsAttacking = false;
		}
	}
}

AWeaponActor* AARPGCharacter::GetCurrentWeapon()
{
	return CurrentWeapon;
}

void AARPGCharacter::SetForceAttackMode(bool bForceAttackModeEnabled)
{
	bIsInForceAttackMode = bForceAttackModeEnabled;
}

bool AARPGCharacter::TryGetTargetMobActors(TArray<ABaseMobType*>& OutTargetMobActors)
{
	float WeaponAttackRange = CurrentWeapon->GetAttackRange();

	FVector Start = GetActorLocation();
	FVector End = Start + GetActorForwardVector() * WeaponAttackRange;

	FCollisionShape Sphere = FCollisionShape::MakeSphere(5.f);
	TArray<FHitResult> HitResults;
	if (!GetWorld()->SweepMultiByChannel(HitResults, Start, End, FQuat::Identity, ECC_GameTraceChannel1, Sphere))
	{
		return false;
	}

	DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 3.f);

	for (FHitResult HitResult : HitResults)
	{
		AActor* HitActor = HitResult.GetActor();
		OutTargetMobActors.Add(Cast<ABaseMobType>(HitActor));
	}
	
	return true;
}

// TODO: Make this report more useful info beyond success/failure
bool AARPGCharacter::AttackTarget(ABaseMobType* TargetMobActor)
{
	bIsAttacking = true;

	if (AttackTimer == 0.f && CurrentWeapon)
	{
		CurrentWeapon->Swing();
		if (TargetMobActor != nullptr)
		{
			// TODO: Damage the target
		}
	}

	return true;
}

void AARPGCharacter::BeginPlay()
{
	Super::BeginPlay();

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	CurrentWeapon = GetWorld()->SpawnActor<AWeaponActor>(MainHandWeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
	if (CurrentWeapon)
	{
		CurrentWeapon->SetOwner(this);
		CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("RightHandSocket"));
	}
}
