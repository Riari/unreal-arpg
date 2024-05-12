// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthComponent.h"

#include "Kismet/GameplayStatics.h"

#include "ARPGGameMode.h"

UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

bool UHealthComponent::ApplyDamage(float Damage)
{
	if (Damage <= 0) return false;

	Health -= Damage;

	if (Health <= 0)
	{
		if (GameMode != nullptr) GameMode->ActorDied(GetOwner());
		return true;
	}

	return false;
}

void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	
	Health = MaxHealth;

	GameMode = Cast<AARPGGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
}
