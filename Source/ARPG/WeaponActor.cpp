// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponActor.h"

#include "Kismet/GameplayStatics.h"

AWeaponActor::AWeaponActor()
	: Mesh{CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"))}
{
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = Mesh;
}

void AWeaponActor::Swing()
{
	if (SwingSounds.Num() > 0)
	{
		UGameplayStatics::PlaySoundAtLocation(this, SwingSounds[FMath::RandRange(0, SwingSounds.Num() - 1)], GetActorLocation());
	}
}

void AWeaponActor::BeginPlay()
{
	Super::BeginPlay();
}

