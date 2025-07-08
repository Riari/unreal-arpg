// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseMobType.h"

#include "Components/AudioComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/PlayerController.h"
#include "Engine/DecalActor.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Math/UnrealMathUtility.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "Sound/SoundCue.h"

#include "../ARPGCharacter.h"
#include "../HealthComponent.h"
#include "../UI/MobUI.h"

ABaseMobType::ABaseMobType()
	: HealthComponent{CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"))}
	, ReceiveMeleeWeaponHitSoundComponent{CreateDefaultSubobject<UAudioComponent>(TEXT("ReceiveMeleeWeaponHitSoundComponent"))}
{
	PrimaryActorTick.bCanEverTick = true;

	ReceiveMeleeWeaponHitSoundComponent->SetupAttachment(RootComponent);
}

void ABaseMobType::BeginPlay()
{
	Super::BeginPlay();
	
	USkeletalMeshComponent* ActorMesh = GetMesh();

	for (size_t i = 0; i < Materials.Num(); ++i)
	{
		UMaterialInstanceDynamic* DynMaterial = UMaterialInstanceDynamic::Create(Materials[i], this);
		ActorMesh->SetMaterial(i, DynMaterial);
		MaterialInstances.Add(DynMaterial);
	}

	ActorMesh->OnBeginCursorOver.AddDynamic(this, &ABaseMobType::OnMouseOverBegin);
	ActorMesh->OnEndCursorOver.AddDynamic(this, &ABaseMobType::OnMouseOverEnd);
	
	CurrentPlayerController = GetWorld()->GetFirstPlayerController();

	if (ReceiveMeleeWeaponHitSoundCue != nullptr) ReceiveMeleeWeaponHitSoundComponent->SetSound(ReceiveMeleeWeaponHitSoundCue);

	MobUI = Cast<AMobUI>(UGameplayStatics::GetActorOfClass(GetWorld(), AMobUI::StaticClass()));
}

float ABaseMobType::RollMeleeDamage()
{
	return FMath::RandRange(BaseMeleeDamage.GetLowerBoundValue(), BaseMeleeDamage.GetUpperBoundValue());
}

void ABaseMobType::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABaseMobType::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ABaseMobType::OnMouseOverBegin(UPrimitiveComponent* TouchedComponent)
{
	if (!bIsAlive) return;

	CurrentPlayerController->CurrentMouseCursor = EMouseCursor::Type::Crosshairs;
	SetTextureSampleMultiplier(TextureSampleMultiplierHover);

	if (MobUI != nullptr) MobUI->OnMouseEnterMob(this);
}

void ABaseMobType::OnMouseOverEnd(UPrimitiveComponent *TouchedComponent)
{
	if (!bIsAlive) return;

	CurrentPlayerController->CurrentMouseCursor = EMouseCursor::Type::Default;
	SetTextureSampleMultiplier(1.f);

	if (MobUI != nullptr) MobUI->OnMouseExitMob(this);
}

void ABaseMobType::PlayReceiveMeleeWeaponHitSound()
{
	if (!bIsAlive) return;

	ReceiveMeleeWeaponHitSoundComponent->Play();

	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), BloodSplashParticleSystem, GetActorLocation());

	for (size_t i = 0; i < 2; ++i)
	{
		FVector SpawnLocation = GetActorLocation() + UKismetMathLibrary::RandomUnitVector() * 100.f;
		SpawnLocation.Z = 0.f;

		FRotator SpawnRotation{};
		SpawnRotation.Yaw = UKismetMathLibrary::RandomFloatInRange(0.f, 360.f);

		GetWorld()->SpawnActor<ADecalActor>(BloodSplatterDecalActorClass, SpawnLocation, SpawnRotation);
	}
}

void ABaseMobType::InflictWeaponDamageOnTarget()
{
	if (CurrentTarget == nullptr || !bIsAlive) return;

	AARPGCharacter* TargetCharacter = Cast<AARPGCharacter>(CurrentTarget);
	if (TargetCharacter) TargetCharacter->PlayReceiveMeleeHitSound();

	UHealthComponent* TargetHealthComponent = CurrentTarget->FindComponentByClass<UHealthComponent>();
	if (TargetHealthComponent) TargetHealthComponent->ApplyDamage(RollMeleeDamage());

	{
		TRACE_CPUPROFILER_EVENT_SCOPE_STR("EnemyActor: Expensive work");
		FPlatformProcess::Sleep(0.03f);
	}
}

void ABaseMobType::Die()
{
	bIsAlive = false;
	bIsAttacking = false;
	SetTextureSampleMultiplier(1.f);
	CurrentPlayerController->CurrentMouseCursor = EMouseCursor::Type::Default;
	SetActorEnableCollision(false);
	if (MobUI != nullptr) MobUI->OnMobDied(this);
}

bool ABaseMobType::IsDead_Implementation() const
{
    return !bIsAlive;
}

void ABaseMobType::SetTextureSampleMultiplier(float Multiplier)
{
	for (size_t i = 0; i < MaterialInstances.Num(); ++i)
	{
		MaterialInstances[i]->SetScalarParameterValue("TextureSampleMultiplier", Multiplier);
	}
}