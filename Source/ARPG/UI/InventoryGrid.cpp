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

    for (int X = 0; X < GridWidth; ++X)
    {
        for (int Y = 0; Y < GridHeight; ++Y)
        {
            if (!CanItemBePlacedAt(X, Y, ItemData)) continue;

            int ItemWidth = ItemData->GetInventoryWidth();
            int ItemHeight = ItemData->GetInventoryHeight();
            float CellSize = GetCellLocalSize();

            UItem* ItemWidget = CreateWidget<UItem>(GetWorld(), ItemWidgetClass);
            UCanvasPanelSlot* CanvasPanelSlot = InventoryItemsCanvas->AddChildToCanvas(ItemWidget);
            CanvasPanelSlot->SetPosition(FVector2D(X * CellSize, Y * CellSize));
            CanvasPanelSlot->SetSize(FVector2D(ItemWidth * CellSize, ItemHeight * CellSize));
            ItemWidget->Init(ItemData, CellSize);

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

            break;
        }
    }
}

void UInventoryGrid::OnDraggedItemEnter(UItemDataAsset* ItemData)
{
    bIsDragging = true;
    DraggedItemData = ItemData;

    float CellSize = GetCellLocalSize();
    SlotHoverWidgetCanvasSlot->SetSize(FVector2D(CellSize * ItemData->GetInventoryWidth(), CellSize * ItemData->GetInventoryHeight()));
    SlotHoverWidget->SetVisibility(ESlateVisibility::Visible);
}

void UInventoryGrid::OnDraggingCancelled()
{
    SlotHoverWidget->SetVisibility(ESlateVisibility::Collapsed);
    DraggedItemData = nullptr;
    bIsDragging = false;
}

float UInventoryGrid::GetCellLocalSize() const
{
    FGeometry GridGeometry = SlotGrid->GetCachedGeometry();
    FVector2D GridSize = GridGeometry.GetLocalSize();
    return GridSize.X / GridWidth;
}

void UInventoryGrid::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
    Super::NativeTick(MyGeometry, DeltaTime);

    if (!bIsDragging) return;

    int ItemSlotWidth = DraggedItemData->GetInventoryWidth();
    int ItemSlotHeight = DraggedItemData->GetInventoryHeight();

    FGeometry InventoryGeometry = SlotGrid->GetCachedGeometry();
    float CellSize = GetCellLocalSize();
    float GridMinX = FMath::Floor(InventoryGridDragPosition.X / CellSize) * CellSize;
    float GridMinY = FMath::Floor(InventoryGridDragPosition.Y / CellSize) * CellSize;
    float GridMaxX = GridMinX + (ItemSlotWidth * CellSize);
    float GridMaxY = GridMinY + (ItemSlotHeight * CellSize);

    if (ItemSlotWidth % 2 == 0)
    {
        GridMinX -= CellSize / 2.f;
        GridMaxX -= CellSize / 2.f;
    }

    if (ItemSlotHeight % 2 == 0)
    {
        GridMinY -= CellSize / 2.f;
        GridMaxY -= CellSize / 2.f;
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
    UE_LOG(LogTemp, Log, TEXT("UInventoryGrid: Initializing grid with dimensions %d x %d."), GridWidth, GridHeight);

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
