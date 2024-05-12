// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseMobType.h"

#include "Components/AudioComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/PlayerController.h"
#include "Engine/DecalActor.h"
#include "Kismet/KismetMathLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "Sound/SoundCue.h"

ABaseMobType::ABaseMobType()
	: WeaponHitSoundComponent{CreateDefaultSubobject<UAudioComponent>(TEXT("WeaponHitSoundComponent"))}
{
	PrimaryActorTick.bCanEverTick = true;

	WeaponHitSoundComponent->SetupAttachment(RootComponent);
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

	if (WeaponHitSoundCue != nullptr) WeaponHitSoundComponent->SetSound(WeaponHitSoundCue);
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
	CurrentPlayerController->CurrentMouseCursor = EMouseCursor::Type::Crosshairs;
	SetTextureSampleMultiplier(TextureSampleMultiplierHover);
}

void ABaseMobType::OnMouseOverEnd(UPrimitiveComponent *TouchedComponent)
{
	CurrentPlayerController->CurrentMouseCursor = EMouseCursor::Type::Default;
	SetTextureSampleMultiplier(1.f);
}

void ABaseMobType::PlayWeaponHitSound()
{
	WeaponHitSoundComponent->Play();

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

void ABaseMobType::SetTextureSampleMultiplier(float Multiplier)
{
	for (size_t i = 0; i < MaterialInstances.Num(); ++i)
	{
		MaterialInstances[i]->SetScalarParameterValue("TextureSampleMultiplier", Multiplier);
	}
}