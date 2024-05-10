// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseMobType.h"

#include "Components/SkeletalMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

ABaseMobType::ABaseMobType()
{
	PrimaryActorTick.bCanEverTick = true;
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
}

void ABaseMobType::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HoverTimer <= 0.f) return;

	HoverTimer -= DeltaTime;

	if (HoverTimer <= 0.f) SetTextureSampleMultiplier(1.f);
}

void ABaseMobType::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ABaseMobType::OnHover()
{
	HoverTimer = HoverTime;
	SetTextureSampleMultiplier(TextureSampleMultiplierHover);
}

void ABaseMobType::SetTextureSampleMultiplier(float Multiplier)
{
	for (size_t i = 0; i < MaterialInstances.Num(); ++i)
	{
		MaterialInstances[i]->SetScalarParameterValue("TextureSampleMultiplier", Multiplier);
	}
}