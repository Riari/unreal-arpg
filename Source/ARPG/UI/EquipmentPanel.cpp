// Fill out your copyright notice in the Description page of Project Settings.


#include "EquipmentPanel.h"

#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/GridPanel.h"
#include "Components/Image.h"
#include "Math/UnrealMathUtility.h"

#include "../Data/ItemDataAsset.h"
#include "Item.h"

void UEquipmentPanel::AddItemToInventory(UItemDataAsset* ItemData)
{
    if (InventoryItemsCanvas == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("UEquipmentPanel: InventoryItemsCanvas is null! Ensure there is a UCanvasPanel widget of this name exposed as a variable."));
        return;
    }

    if (ItemWidgetClass == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("UEquipmentPanel: ItemWidgetClass is null!"));
        return;
    }

    UItem* ItemWidget = CreateWidget<UItem>(GetWorld(), ItemWidgetClass);
    ItemWidget->Init(ItemData);
    UCanvasPanelSlot* CanvasPanelSlot = InventoryItemsCanvas->AddChildToCanvas(ItemWidget);
}

void UEquipmentPanel::OnDraggedItemEnter(UItemDataAsset* ItemData)
{
    bIsDragging = true;
    float CellSize = GetInventoryGridCellSize();
    SlotHoverWidget->SetDesiredSizeInViewport(FVector2D(CellSize * ItemData->GetInventoryWidth(), CellSize * ItemData->GetInventoryHeight()));
    SlotHoverWidget->AddToViewport();
    UE_LOG(LogTemp, Warning, TEXT("OnDraggedItemEnter"));
    UE_LOG(LogTemp, Warning, TEXT("Item is %fx%f"), ItemData->GetInventoryWidth() * CellSize, ItemData->GetInventoryHeight() * CellSize);
}

void UEquipmentPanel::OnDraggingCancelled()
{
    UE_LOG(LogTemp, Warning, TEXT("OnDraggingCancelled"));

    SlotHoverWidget->RemoveFromParent();
    DraggedItemData = nullptr;
    bIsDragging = false;
}

float UEquipmentPanel::GetInventoryGridCellSize() const
{
    FGeometry GridGeometry = InventoryGrid->GetCachedGeometry();
    FVector2D GridSize = GridGeometry.GetLocalSize();
    return GridSize.X / InventorySlotsPerRow;
}

void UEquipmentPanel::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
    Super::NativeTick(MyGeometry, DeltaTime);

    if (!bIsDragging) return;

    FGeometry InventoryGeometry = InventoryGrid->GetCachedGeometry();
    FVector2D AbsolutePosition = InventoryGeometry.GetAbsolutePosition();
    float CellSize = GetInventoryGridCellSize();
    float GridX = AbsolutePosition.X + (FMath::Floor(InventoryGridDragPosition.X / CellSize) * CellSize);
    float GridY = AbsolutePosition.Y + (FMath::Floor(InventoryGridDragPosition.Y / CellSize) * CellSize);

    SlotHoverWidget->SetPositionInViewport(FVector2D(GridX, GridY));
}

void UEquipmentPanel::NativeConstruct()
{
    Super::NativeConstruct();

    SlotHoverWidget = CreateWidget<UUserWidget>(GetWorld(), SlotHoverWidgetClass);
    CreateSlots();
}

void UEquipmentPanel::CreateSlots()
{
    if (SlotWidgetClass == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("UEquipmentPanel: SlotWidgetClass is null!"));
        return;
    }

    if (InventoryGrid == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("UEquipmentPanel: InventoryGrid is null! Ensure there is a UGridPanel widget of this name exposed as a variable."));
        return;
    }

    for (int i = 0; i < InventorySlotCount; ++i)
    {
        int x{i % InventorySlotsPerRow};
        int y{i / InventorySlotsPerRow};
        UUserWidget* SlotWidget = CreateWidget<UUserWidget>(GetWorld(), SlotWidgetClass);
        InventoryGrid->AddChildToGrid(SlotWidget, y, x);
    }
}