// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryGrid.h"

#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Components/UniformGridPanel.h"
#include "Math/UnrealMathUtility.h"
#include "Templates/UniquePtr.h"

#include "../Data/ItemDataAsset.h"

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

    int ItemWidth = ItemData->GetInventoryWidth();
    int ItemHeight = ItemData->GetInventoryHeight();
    float SlotSize = GetSlotLocalSize();
    UE_LOG(LogTemp, Log, TEXT("Slot size is %f"), SlotSize);

    for (int Y = 0; Y < GridHeight; ++Y)
    {
        for (int X = 0; X < GridWidth; ++X)
        {
            if (!CanItemBePlacedAt(X, Y, ItemData)) continue;

            UItem* ItemWidget = CreateWidget<UItem>(GetWorld(), ItemWidgetClass);
            UCanvasPanelSlot* CanvasPanelSlot = InventoryItemsCanvas->AddChildToCanvas(ItemWidget);
            CanvasPanelSlot->SetPosition(FVector2D(X * SlotSize, Y * SlotSize));
            CanvasPanelSlot->SetSize(FVector2D(ItemWidth * SlotSize, ItemHeight * SlotSize));
            ItemWidget->Init(ItemData, SlotSize);

            TSharedPtr<FInventoryItem> InventoryItem = MakeShared<FInventoryItem>();
            InventoryItem->ItemData = ItemData;
            InventoryItem->Widget = ItemWidget;
            InventoryItem->CanvasPanelSlot = CanvasPanelSlot;
            InventoryItem->X = X;
            InventoryItem->Y = Y;

            for (int ItemX = 0; ItemX < ItemWidth; ++ItemX)
            {
                for (int ItemY = 0; ItemY < ItemHeight; ++ItemY)
                {
                    Grid[X + ItemX][Y + ItemY] = InventoryItem;
                }
            }

            UE_LOG(LogTemp, Log, TEXT("UInventoryGrid: Item placed at %d, %d"), X, Y);
            return;
        }
    }
}

void UInventoryGrid::OnDraggedItemEnter(UItemDataAsset* ItemData)
{
    bIsDragging = true;
    DraggedItemData = ItemData;

    float SlotSize = GetSlotLocalSize();
    SlotHoverWidgetCanvasSlot->SetSize(FVector2D(SlotSize * ItemData->GetInventoryWidth(), SlotSize * ItemData->GetInventoryHeight()));
    SlotHoverWidget->SetVisibility(ESlateVisibility::Visible);
}

void UInventoryGrid::OnDraggingCancelled()
{
    SlotHoverWidget->SetVisibility(ESlateVisibility::Collapsed);
    DraggedItemData = nullptr;
    bIsDragging = false;
}

float UInventoryGrid::GetSlotLocalSize() const
{
    FGeometry SlotGeometry = SlotGrid->GetCachedGeometry();
    float SlotSize = SlotGeometry.GetLocalSize().X;
    return SlotSize / GridWidth;
}

float UInventoryGrid::GetSlotDrawSize() const
{
    FGeometry SlotGeometry = SlotGrid->GetCachedGeometry();
    float SlotSize = SlotGeometry.GetDrawSize().X;
    return SlotSize / GridWidth;
}

void UInventoryGrid::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
    Super::NativeTick(MyGeometry, DeltaTime);

    if (!bIsDragging) return;

    int ItemSlotWidth = DraggedItemData->GetInventoryWidth();
    int ItemSlotHeight = DraggedItemData->GetInventoryHeight();

    float SlotSize = GetSlotLocalSize();
    float GridMinX = FMath::Floor(InventoryGridDragPosition.X / SlotSize) * SlotSize;
    float GridMinY = FMath::Floor(InventoryGridDragPosition.Y / SlotSize) * SlotSize;
    float GridMaxX = GridMinX + (ItemSlotWidth * SlotSize);
    float GridMaxY = GridMinY + (ItemSlotHeight * SlotSize);

    if (ItemSlotWidth % 2 != 0)
    {
        GridMinX -= (ItemSlotWidth / 2) * SlotSize;
        GridMaxX -= (ItemSlotWidth / 2) * SlotSize;
    }

    if (ItemSlotHeight % 2 != 0)
    {
        GridMinY -= (ItemSlotHeight / 2) * SlotSize;
        GridMaxY -= (ItemSlotHeight / 2) * SlotSize;
    }

    DraggedItemDropArea.Min = {GridMinX, GridMinY};
    DraggedItemDropArea.Max = {GridMaxX, GridMaxY};
    SlotHoverWidgetCanvasSlot->SetPosition(FVector2D(GridMinX, GridMinY));
}

void UInventoryGrid::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    InitializeGrid();
    CreateSlots();

    SlotHoverWidget = CreateWidget<UInventorySlotHover>(GetWorld(), SlotHoverWidgetClass);
    SlotHoverWidgetCanvasSlot = InventoryItemsCanvas->AddChildToCanvas(SlotHoverWidget);
    SlotHoverWidget->SetVisibility(ESlateVisibility::Collapsed);
}

void UInventoryGrid::InitializeGrid()
{
    Grid.SetNum(GridWidth);
    for (int X = 0; X < GridWidth; ++X)
    {
        Grid[X].SetNum(GridHeight);
        for (int Y = 0; Y < GridHeight; ++Y)
        {
            Grid[X][Y] = nullptr;
        }
    }
}

void UInventoryGrid::CreateSlots()
{
    if (SlotWidgetClass == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("UInventoryGrid: SlotWidgetClass is null!"));
        return;
    }

    if (SlotGrid == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("UInventoryGrid: SlotGrid is null! Ensure there is a UUniformGridPanel widget of this name exposed as a variable."));
        return;
    }

    for (int X = 0; X < GridWidth; ++X)
    {
        for (int Y = 0; Y < GridHeight; ++Y)
        {
            UUserWidget* SlotWidget = CreateWidget<UUserWidget>(GetWorld(), SlotWidgetClass);
            SlotGrid->AddChildToUniformGrid(SlotWidget, Y, X);
        }
    }

    SlotGrid->SynchronizeProperties();
    SlotGrid->ForceLayoutPrepass();

    UE_LOG(LogTemp, Log, TEXT("UInventoryGrid: Slots created!"));
}

bool UInventoryGrid::IsSlotAvailable(int X, int Y) const
{
    return Grid[X][Y] == nullptr;
}

bool UInventoryGrid::CanItemBePlacedAt(int X, int Y, UItemDataAsset *ItemData) const
{
    int ItemWidth = ItemData->GetInventoryWidth();
    int ItemHeight = ItemData->GetInventoryHeight();

    if (X + ItemWidth > GridWidth || Y + ItemHeight > GridHeight) return false;

    for (int i = X; i < X + ItemWidth; ++i)
    {
        for (int j = Y; j < Y + ItemHeight; ++j)
        {
            if (!IsSlotAvailable(i, j)) return false;
        }
    }

    return true;
}
