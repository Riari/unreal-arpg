// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Item.generated.h"

/**
 * 
 */
UCLASS()
class ARPG_API UItem : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetIconTexture(TObjectPtr<UTexture2D> texture);

protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UButton> BackgroundButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UImage> IconImage;
};
