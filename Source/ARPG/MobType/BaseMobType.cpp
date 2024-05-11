// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseMobType.h"

#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/PlayerController.h"
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

	ActorMesh->OnBeginCursorOver.AddDynamic(this, &ABaseMobType::OnMouseOverBegin);
	ActorMesh->OnEndCursorOver.AddDynamic(this, &ABaseMobType::OnMouseOverEnd);
	
	CurrentPlayerController = GetWorld()->GetFirstPlayerController();
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

void ABaseMobType::SetTextureSampleMultiplier(float Multiplier)
{
	for (size_t i = 0; i < MaterialInstances.Num(); ++i)
	{
		MaterialInstances[i]->SetScalarParameterValue("TextureSampleMultiplier", Multiplier);
	}
}