// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Layout/Geometry.h"
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
	void AddItemToInventory(class UItemDataAsset* ItemData);

	UFUNCTION(BlueprintCallable)
	void OnDraggedItemEnter(class UItemDataAsset* ItemData);

	UFUNCTION(BlueprintCallable)
	void OnDraggingCancelled();

	UFUNCTION(BlueprintCallable)
	float GetInventoryGridCellSize() const;

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
	TSubclassOf<UUserWidget> SlotHoverWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UUserWidget> ItemWidgetClass;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UGridPanel> InventoryGrid;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UCanvasPanel> InventoryItemsCanvas;

	UPROPERTY(BlueprintReadWrite)
	FVector2D InventoryGridDragPosition;

	TObjectPtr<UUserWidget> SlotHoverWidget;
	TObjectPtr<UItemDataAsset> DraggedItemData;
	bool bIsDragging{false};

	void NativeConstruct() override;

	void CreateSlots();
};
