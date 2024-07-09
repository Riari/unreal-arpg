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
	void Init(TObjectPtr<class UItemDataAsset> Data);

protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UBorder> IconContainer;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UImage> IconImage;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<class UItemDataAsset> ItemData;
};
