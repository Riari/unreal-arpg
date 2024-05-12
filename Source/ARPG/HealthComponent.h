// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ARPG_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UHealthComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/**
	 * Applies the given damage amount and returns a bool indicating whether it was a killing blow.
	 */
	bool ApplyDamage(float Damage);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxHealth{100.f};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float Health{0.f};

private:
	class AARPGGameMode* GameMode;
};
