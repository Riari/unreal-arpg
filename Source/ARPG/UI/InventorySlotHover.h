// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Math/Color.h"
#include "InventorySlotHover.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class ARPG_API UInventorySlotHover : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetValid(bool bValid);

protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UImage> Highlight;

	UPROPERTY(BlueprintReadWrite)
	FLinearColor ValidColor;

	UPROPERTY(BlueprintReadWrite)
	FLinearColor InvalidColor;

	bool bIsValid{true};
};
