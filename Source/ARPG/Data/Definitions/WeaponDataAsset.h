// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Math/Range.h"

#include "ItemDataAsset.h"
#include "WeaponDataAsset.generated.h"

/**
 * 
 */
UCLASS(abstract, Blueprintable)
class ARPG_API UWeaponDataAsset : public UItemDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = Details)
	float AttackRange;

	UPROPERTY(EditAnywhere, Category = Details)
	FFloatRange BaseDamage;
};
