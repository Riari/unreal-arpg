// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/SizeBox.h"
#include "Components/CanvasPanelSlot.h"
#include "InventorySlotHover.h"
#include "InventoryGrid.generated.h"

/**
 * 
 */
UCLASS()
class ARPG_API UInventoryGrid : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void AddItemToInventory(class UItemDataAsset* ItemData);

	UFUNCTION(BlueprintCallable)
	void OnDraggedItemEnter(class UItemDataAsset* ItemData);

	UFUNCTION(BlueprintCallable)
	void OnDraggingCancelled();

	UFUNCTION(BlueprintCallable)
	float GetCellLocalSize() const;

	virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int InventorySlotCount{72};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int InventorySlotsPerRow{12};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FIntPoint, class UItemDataAsset*> Inventory;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UUserWidget> SlotWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UInventorySlotHover> SlotHoverWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UUserWidget> ItemWidgetClass;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UUniformGridPanel> InventoryGrid;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UCanvasPanel> InventoryItemsCanvas;

	UPROPERTY(BlueprintReadWrite)
	FVector2D InventoryGridDragPosition;

	TObjectPtr<UInventorySlotHover> SlotHoverWidget;
	TObjectPtr<UCanvasPanelSlot> SlotHoverWidgetCanvasSlot;
	TObjectPtr<UItemDataAsset> DraggedItemData;
	bool bIsDragging{false};

	void NativeConstruct() override;

	void CreateSlots();
};
