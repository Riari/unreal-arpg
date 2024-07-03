// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Math/IntPoint.h"
#include "Templates/UniquePtr.h"

#include "Blueprint/UserWidget.h"
#include "EquipmentPanel.generated.h"

UCLASS(Blueprintable)
class ARPG_API UEquipmentPanel : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void CreateSlots(class UGridPanel* InventoryGrid);

	UFUNCTION(BlueprintCallable)
	void AddItemToInventory(class UItemDataAsset* ItemData);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int InventorySlotCount{96};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int InventorySlotsPerRow{16};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FIntPoint, class UItemDataAsset*> Inventory;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UUserWidget> SlotWidgetClass;
};
