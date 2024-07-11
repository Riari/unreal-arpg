// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryGrid.h"

#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Components/UniformGridPanel.h"
#include "Math/UnrealMathUtility.h"

#include "../Data/ItemDataAsset.h"
#include "Item.h"

void UInventoryGrid::AddItemToInventory(UItemDataAsset* ItemData)
{
    if (InventoryItemsCanvas == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("UInventoryGrid: InventoryItemsCanvas is null! Ensure there is a UCanvasPanel widget of this name exposed as a variable."));
        return;
    }

    if (ItemWidgetClass == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("UInventoryGrid: ItemWidgetClass is null!"));
        return;
    }

    UItem* ItemWidget = CreateWidget<UItem>(GetWorld(), ItemWidgetClass);
    ItemWidget->Init(ItemData);
    UCanvasPanelSlot* CanvasPanelSlot = InventoryItemsCanvas->AddChildToCanvas(ItemWidget);
}

void UInventoryGrid::OnDraggedItemEnter(UItemDataAsset* ItemData)
{
    bIsDragging = true;
    float CellSize = GetCellLocalSize();
    SlotHoverWidgetCanvasSlot->SetSize(FVector2D(CellSize * ItemData->GetInventoryWidth(), CellSize * ItemData->GetInventoryHeight()));
    SlotHoverWidget->SetVisibility(ESlateVisibility::Visible);
    UE_LOG(LogTemp, Warning, TEXT("OnDraggedItemEnter"));
    UE_LOG(LogTemp, Warning, TEXT("Item is %fx%f"), ItemData->GetInventoryWidth() * CellSize, ItemData->GetInventoryHeight() * CellSize);
}

void UInventoryGrid::OnDraggingCancelled()
{
    UE_LOG(LogTemp, Warning, TEXT("OnDraggingCancelled"));

    SlotHoverWidget->SetVisibility(ESlateVisibility::Collapsed);
    DraggedItemData = nullptr;
    bIsDragging = false;
}

float UInventoryGrid::GetCellLocalSize() const
{
    FGeometry GridGeometry = InventoryGrid->GetCachedGeometry();
    FVector2D GridSize = GridGeometry.GetLocalSize();
    return GridSize.X / InventorySlotsPerRow;
}

void UInventoryGrid::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
    Super::NativeTick(MyGeometry, DeltaTime);

    if (!bIsDragging) return;

    FGeometry InventoryGeometry = InventoryGrid->GetCachedGeometry();
    float CellSize = GetCellLocalSize();
    float GridX = FMath::Floor(InventoryGridDragPosition.X / CellSize) * CellSize;
    float GridY = FMath::Floor(InventoryGridDragPosition.Y / CellSize) * CellSize;

    SlotHoverWidgetCanvasSlot->SetPosition(FVector2D(GridX, GridY));
}

void UInventoryGrid::NativeConstruct()
{
    Super::NativeConstruct();

    CreateSlots();

    SlotHoverWidget = CreateWidget<UInventorySlotHover>(GetWorld(), SlotHoverWidgetClass);
    SlotHoverWidgetCanvasSlot = InventoryItemsCanvas->AddChildToCanvas(SlotHoverWidget);
    SlotHoverWidget->SetVisibility(ESlateVisibility::Collapsed);
}

void UInventoryGrid::CreateSlots()
{
    if (SlotWidgetClass == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("UInventoryGrid: SlotWidgetClass is null!"));
        return;
    }

    if (InventoryGrid == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("UInventoryGrid: InventoryGrid is null! Ensure there is a UGridPanel widget of this name exposed as a variable."));
        return;
    }

    for (int i = 0; i < InventorySlotCount; ++i)
    {
        int x{i % InventorySlotsPerRow};
        int y{i / InventorySlotsPerRow};
        UUserWidget* SlotWidget = CreateWidget<UUserWidget>(GetWorld(), SlotWidgetClass);
        InventoryGrid->AddChildToUniformGrid(SlotWidget, y, x);
    }
}