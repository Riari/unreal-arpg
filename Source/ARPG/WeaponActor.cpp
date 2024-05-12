// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponActor.h"

#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Math/UnrealMathUtility.h"
#include "Sound/SoundCue.h"

AWeaponActor::AWeaponActor()
	: Mesh{CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"))}
	, SwingSoundComponent{CreateDefaultSubobject<UAudioComponent>(TEXT("SwingSoundComponent"))}
{
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = Mesh;

	SwingSoundComponent->SetupAttachment(RootComponent);
}

float AWeaponActor::GetAttackRange() const
{
    return AttackRange;
}

FFloatRange AWeaponActor::GetBaseDamage() const
{
	return BaseDamage;
}

void AWeaponActor::PlaySwingSound()
{
	SwingSoundComponent->Play();
}

float AWeaponActor::RollDamage()
{
	return FMath::RandRange(BaseDamage.GetLowerBoundValue(), BaseDamage.GetUpperBoundValue());
}

void AWeaponActor::BeginPlay()
{
	Super::BeginPlay();

	if (SwingSoundCue != nullptr) SwingSoundComponent->SetSound(SwingSoundCue);
}

