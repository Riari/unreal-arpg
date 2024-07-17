// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/SizeBox.h"
#include "Containers/Array.h"
#include "Math/IntRect.h"

#include "InventorySlotHover.h"
#include "Item.h"

#include "InventoryGrid.generated.h"

USTRUCT(BlueprintType)
struct FInventoryItem
{
    GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Inventory")
	TObjectPtr<UItemDataAsset> ItemData;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Inventory")
	TObjectPtr<UItem> Widget;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Inventory")
	TObjectPtr<UCanvasPanelSlot> CanvasPanelSlot;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Inventory")
    int X; // Top-left X coordinate in the grid

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Inventory")
    int Y; // Top-left Y coordinate in the grid
};

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
	float GetSlotLocalSize() const;

	UFUNCTION(BlueprintCallable)
	float GetSlotDrawSize() const;

	virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int GridWidth{12};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int GridHeight{6};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UUserWidget> SlotWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UInventorySlotHover> SlotHoverWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UUserWidget> ItemWidgetClass;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UUniformGridPanel> SlotGrid;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UCanvasPanel> InventoryItemsCanvas;

	UPROPERTY(BlueprintReadWrite)
	FVector2D InventoryGridDragPosition;

	TObjectPtr<UInventorySlotHover> SlotHoverWidget;
	TObjectPtr<UCanvasPanelSlot> SlotHoverWidgetCanvasSlot;
	TObjectPtr<UItemDataAsset> DraggedItemData;
	FBox2f DraggedItemDropArea;
	bool bIsDragging{false};

	TArray<TArray<TSharedPtr<FInventoryItem>>> Grid;

	void NativeOnInitialized() override;

	void InitializeGrid();
	void CreateSlots();

	bool IsSlotAvailable(int X, int Y) const;
	bool CanItemBePlacedAt(int X, int Y, UItemDataAsset* ItemData) const;
};
