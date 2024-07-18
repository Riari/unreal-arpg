// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "ARPG/Data/Runtime/ItemInstance.h"

#include "InventoryItem.generated.h"

/**
 * 
 */
UCLASS()
class ARPG_API UInventoryItem : public UUserWidget
{
	GENERATED_BODY()

public:
	void Init(UItemInstance* InItem, float CellSize);

protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UBorder> Border;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UImage> Icon;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<class UItemInstance> Item;
};
