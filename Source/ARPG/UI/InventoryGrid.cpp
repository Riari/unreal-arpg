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
    float CellLocalSize = GetCellLocalSize();
    SlotHoverWidgetCanvasSlot->SetSize(FVector2D(CellLocalSize * ItemData->GetInventoryWidth(), CellLocalSize * ItemData->GetInventoryHeight()));
    SlotHoverWidget->SetVisibility(ESlateVisibility::Visible);
    UE_LOG(LogTemp, Warning, TEXT("OnDraggedItemEnter"));
    UE_LOG(LogTemp, Warning, TEXT("Item is %fx%f"), ItemData->GetInventoryWidth() * CellLocalSize, ItemData->GetInventoryHeight() * CellLocalSize);
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

float UInventoryGrid::GetCellDrawSize() const
{
    FGeometry GridGeometry = InventoryGrid->GetCachedGeometry();
    FVector2D GridSize = GridGeometry.GetDrawSize();
    return GridSize.X / InventorySlotsPerRow;
}

void UInventoryGrid::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
    Super::NativeTick(MyGeometry, DeltaTime);

    if (!bIsDragging) return;

    FGeometry InventoryGeometry = InventoryGrid->GetCachedGeometry();
    FVector2D AbsolutePosition = InventoryGeometry.GetAbsolutePosition();
    float CellDrawSize = GetCellDrawSize();
    float GridX = AbsolutePosition.X + (FMath::Floor(InventoryGridDragPosition.X / CellDrawSize) * CellDrawSize);
    float GridY = AbsolutePosition.Y + (FMath::Floor(InventoryGridDragPosition.Y / CellDrawSize) * CellDrawSize);

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