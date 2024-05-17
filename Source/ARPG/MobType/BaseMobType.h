// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Internationalization/Text.h"

#include "BaseMobType.generated.h"

UCLASS()
class ARPG_API ABaseMobType : public ACharacter
{
	GENERATED_BODY()

public:
	ABaseMobType();

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	void OnMouseOverBegin(UPrimitiveComponent* TouchedComponent);

	UFUNCTION()
	void OnMouseOverEnd(UPrimitiveComponent* TouchedComponent);

	void PlayWeaponHitSound();

	void ApplyDamage(float DamageAmount);

protected:
	UPROPERTY(Category = "Identity", EditAnywhere, BlueprintReadWrite)
	FText MobName;

	UPROPERTY(Category = "Components", VisibleAnywhere, BlueprintReadWrite)
	class UHealthComponent* HealthComponent;

	UPROPERTY(Category = "Appearance", EditAnywhere)
	float TextureSampleMultiplierHover{2.0f};

	UPROPERTY(Category = "Appearance", EditAnywhere)
	TArray<class UMaterialInterface*> Materials;

	UPROPERTY(Category = "Effects", EditAnywhere)
	class USoundCue* WeaponHitSoundCue;

	UPROPERTY(Category = "Effects", EditAnywhere)
	class UNiagaraSystem* BloodSplashParticleSystem;

	UPROPERTY(Category = "Effects", EditAnywhere)
	TSubclassOf<class ADecalActor> BloodSplatterDecalActorClass;

	UPROPERTY(Category = "UI", VisibleAnywhere, BlueprintReadOnly)
	class AMobUI* MobUI;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsAlive{true};

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bIsAttacking{false};

	class UAudioComponent* WeaponHitSoundComponent;

	TArray<class UMaterialInstanceDynamic*> MaterialInstances;

	virtual void BeginPlay() override;

private:
	class APlayerController* CurrentPlayerController;

	void SetTextureSampleMultiplier(float Multiplier);

};
