// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "MobUI.generated.h"

UCLASS(Blueprintable)
class ARPG_API AMobUI : public AInfo
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
	void OnMouseEnterMob(class ABaseMobType* mob);

	UFUNCTION(BlueprintImplementableEvent)
	void OnMouseExitMob(class ABaseMobType* mob);

	UFUNCTION(BlueprintImplementableEvent)
	void OnMobDied(class ABaseMobType* mob);
};
