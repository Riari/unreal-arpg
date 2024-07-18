// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryGrid.h"

#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Components/UniformGridPanel.h"
#include "Math/UnrealMathUtility.h"

#include "ARPG/Data/Definitions/ItemDataAsset.h"

void UInventoryGrid::CreateItemInInventory(UItemDataAsset * ItemData, int Quantity)
{
    if (ItemData == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("UInventoryGrid: ItemData is null!"));
        return;
    }

    for (int i = 0; i < Quantity; ++i)
    {
        UItemInstance* Item = NewObject<UItemInstance>();
        Item->Init(ItemData);

        AddItemToInventory(Item);
    }
}

void UInventoryGrid::AddItemToInventory(UItemInstance* Item)
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

    int ItemWidth = Item->GetItemData()->GetInventoryWidth();
    int ItemHeight = Item->GetItemData()->GetInventoryHeight();
    float SlotSize = GetSlotLocalSize();
    UE_LOG(LogTemp, Log, TEXT("Slot size is %f"), SlotSize);

    for (int Y = 0; Y < GridHeight; ++Y)
    {
        for (int X = 0; X < GridWidth; ++X)
        {
            if (!CanItemBePlacedAt(X, Y, Item)) continue;

            UInventoryItem* ItemWidget = CreateWidget<UInventoryItem>(GetWorld(), ItemWidgetClass);
            UCanvasPanelSlot* CanvasPanelSlot = InventoryItemsCanvas->AddChildToCanvas(ItemWidget);
            CanvasPanelSlot->SetPosition(FVector2D(X * SlotSize, Y * SlotSize));
            CanvasPanelSlot->SetSize(FVector2D(ItemWidth * SlotSize, ItemHeight * SlotSize));
            ItemWidget->Init(Item, SlotSize);

            TSharedPtr<FInventoryItemInstance> InventoryItem = MakeShared<FInventoryItemInstance>();
            InventoryItem->Item = Item;
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

void UInventoryGrid::OnDraggedItemEnter(UItemInstance* Item)
{
    bIsDragging = true;
    DraggedItem = Item;

    float SlotSize = GetSlotLocalSize();
    SlotHoverWidgetCanvasSlot->SetSize(FVector2D(SlotSize * Item->GetItemData()->GetInventoryWidth(), SlotSize * Item->GetItemData()->GetInventoryHeight()));
    SlotHoverWidget->SetVisibility(ESlateVisibility::Visible);
}

void UInventoryGrid::OnDraggedItemDrop()
{
    if (!bIsDragging) return;

    SlotHoverWidget->SetVisibility(ESlateVisibility::Collapsed);
    bIsDragging = false;

    DraggedItem = nullptr;
}

void UInventoryGrid::OnDraggingCancelled()
{
    SlotHoverWidget->SetVisibility(ESlateVisibility::Collapsed);
    DraggedItem = nullptr;
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

    int ItemSlotWidth = DraggedItem->GetItemData()->GetInventoryWidth();
    int ItemSlotHeight = DraggedItem->GetItemData()->GetInventoryHeight();

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

    DraggedItemDropCoordinates = FIntRect(
        FMath::FloorToInt(GridMinX / SlotSize),
        FMath::FloorToInt(GridMinY / SlotSize),
        FMath::FloorToInt(GridMaxX / SlotSize),
        FMath::FloorToInt(GridMaxY / SlotSize)
    );

    TSet<FGuid> OverlappedItemIDs;
    for (int X = DraggedItemDropCoordinates.Min.X; X <= DraggedItemDropCoordinates.Max.X; ++X)
    {
        for (int Y = DraggedItemDropCoordinates.Min.Y; Y <= DraggedItemDropCoordinates.Max.Y; ++Y)
        {
            if (!IsSlotValid(X, Y) || IsSlotAvailable(X, Y)) continue;

            OverlappedItemIDs.Add(Grid[X][Y]->Item->GetID());
        }
    }

    SlotHoverWidget->SetValid(OverlappedItemIDs.Num() <= 1);
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

bool UInventoryGrid::IsSlotValid(int X, int Y) const
{
    return X >= 0 && Y >= 0 && Grid.Num() > X && Grid[X].Num() > Y;
}

bool UInventoryGrid::IsSlotAvailable(int X, int Y) const
{
    return Grid[X][Y] == nullptr;
}

bool UInventoryGrid::CanItemBePlacedAt(int X, int Y, UItemInstance* Item) const
{
    int ItemWidth = Item->GetItemData()->GetInventoryWidth();
    int ItemHeight = Item->GetItemData()->GetInventoryHeight();

    if (X + ItemWidth > GridWidth || Y + ItemHeight > GridHeight) return false;

    for (int i = X; i < X + ItemWidth; ++i)
    {
        for (int j = Y; j < Y + ItemHeight; ++j)
        {
            if (!IsSlotValid(i, j) || !IsSlotAvailable(i, j)) return false;
        }
    }

    return true;
}
