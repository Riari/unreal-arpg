// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Math/Range.h"
#include "WeaponActor.generated.h"

UCLASS(Blueprintable)
class ARPG_API AWeaponActor : public AActor
{
	GENERATED_BODY()
	
public:
	AWeaponActor();

	float GetAttackRange() const;
	FFloatRange GetBaseDamage() const;

	void PlaySwingSound();
	float RollDamage();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(Category = "Weapon Properties", EditAnywhere)
	float AttackRange{150.f};

	UPROPERTY(Category = "Weapon Properties", EditAnywhere)
	FFloatRange BaseDamage{10.f, 25.f};

	UPROPERTY(Category = "Components", VisibleAnywhere, BlueprintReadWrite)
	class USkeletalMeshComponent* Mesh;

	UPROPERTY(Category = "Effects", EditAnywhere)
	class USoundCue* SwingSoundCue;

	class UAudioComponent* SwingSoundComponent;

};
