// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryItem.h"

#include "Components/CanvasPanelSlot.h"
#include "Components/Border.h"
#include "Components/Image.h"

#include "ARPG/Data/Definitions/ItemDataAsset.h"

void UInventoryItem::Init(UItemInstance* InItem, float CellSize)
{
    FVector2D Size = FVector2D(InItem->GetItemData()->GetInventoryWidth() * CellSize, InItem->GetItemData()->GetInventoryHeight() * CellSize);

    UCanvasPanelSlot* BorderCanvasSlot = Cast<UCanvasPanelSlot>(Border->Slot);
    BorderCanvasSlot->SetSize(Size);
    UCanvasPanelSlot* IconCanvasSlot = Cast<UCanvasPanelSlot>(Icon->Slot);
    IconCanvasSlot->SetSize(Size);

    Icon->SetBrushFromTexture(InItem->GetItemData()->GetIconTexture(), true);
    Item = InItem;
}