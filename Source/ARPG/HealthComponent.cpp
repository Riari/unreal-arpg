// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthComponent.h"

#include "Kismet/GameplayStatics.h"

#include "ARPGGameMode.h"
#include "MortalInterface.h"

UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UHealthComponent::ApplyDamage(float Damage)
{
	if (Damage <= 0) return;

	Health -= Damage;

	OnHealthChangedEvent.Broadcast();

	if (Health <= 0)
	{
		AActor* OwningActor = GetOwner();
		if (GameMode != nullptr) GameMode->ActorDied(OwningActor);

		IMortalInterface* Mortal = Cast<IMortalInterface>(OwningActor);
		Mortal->Die();
	}
}

void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	
	Health = MaxHealth;
	GameMode = Cast<AARPGGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
}
