// Copyright Epic Games, Inc. All Rights Reserved.

#include "ARPGCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/AudioComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/DecalComponent.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Materials/Material.h"
#include "Sound/SoundCue.h"
#include "UObject/ConstructorHelpers.h"

#include "HealthComponent.h"
#include "MobType/BaseMobType.h"
#include "WeaponActor.h"

AARPGCharacter::AARPGCharacter()
	: HealthComponent{CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"))}
	, ReceiveMeleeHitSoundComponent{CreateDefaultSubobject<UAudioComponent>(TEXT("ReceiveMeleeHitSoundComponent"))}
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

	ReceiveMeleeHitSoundComponent->SetupAttachment(RootComponent);
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
			CurrentTarget = nullptr;
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
	if (!bIsAlive || !CurrentWeapon) return false;

	float WeaponAttackRange = CurrentWeapon->GetAttackRange();

	FVector Start = GetActorLocation();
	FVector End = Start + GetActorForwardVector() * WeaponAttackRange;

	FCollisionShape Sphere = FCollisionShape::MakeSphere(5.f);
	TArray<FHitResult> HitResults;
	if (!GetWorld()->SweepMultiByChannel(HitResults, Start, End, FQuat::Identity, ECC_GameTraceChannel1, Sphere))
	{
		return false;
	}

	// DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 3.f);

	for (const FHitResult& HitResult : HitResults)
	{
		AActor* HitActor = HitResult.GetActor();
		OutTargetMobActors.Add(Cast<ABaseMobType>(HitActor));
	}
	
	return true;
}

// TODO: Make this report more useful info beyond success/failure
bool AARPGCharacter::AttackTarget(ABaseMobType* TargetMobActor)
{
	if (!bIsAlive) return false;

	bIsAttacking = true;
	CurrentTarget = TargetMobActor;
	return true;
}

void AARPGCharacter::PlayWeaponSwingSound()
{
	if (CurrentWeapon == nullptr) return;

	CurrentWeapon->PlaySwingSound();
}

void AARPGCharacter::PlayReceiveMeleeHitSound()
{
	ReceiveMeleeHitSoundComponent->Play();
}

void AARPGCharacter::InflictWeaponDamageOnTarget()
{
	if (CurrentTarget == nullptr || !bIsAlive) return;

	CurrentTarget->PlayReceiveMeleeWeaponHitSound();

	UHealthComponent* TargetHealthComponent = CurrentTarget->FindComponentByClass<UHealthComponent>();
	if (TargetHealthComponent) TargetHealthComponent->ApplyDamage(CurrentWeapon->RollDamage());
}

void AARPGCharacter::Die()
{
	bIsAlive = false;
	bIsAttacking = false;
}

bool AARPGCharacter::IsDead_Implementation() const
{
	return !bIsAlive;
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

	if (ReceiveMeleeHitSoundCue != nullptr) ReceiveMeleeHitSoundComponent->SetSound(ReceiveMeleeHitSoundCue);
}
