// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"

#include "Components/CanvasPanelSlot.h"
#include "Components/Border.h"
#include "Components/Image.h"

#include "../Data/ItemDataAsset.h"

void UItem::Init(TObjectPtr<UItemDataAsset> Data, float CellSize)
{
    FVector2D Size = FVector2D(Data->GetInventoryWidth() * CellSize, Data->GetInventoryHeight() * CellSize);

    UCanvasPanelSlot* BorderCanvasSlot = Cast<UCanvasPanelSlot>(Border->Slot);
    BorderCanvasSlot->SetSize(Size);
    UCanvasPanelSlot* IconCanvasSlot = Cast<UCanvasPanelSlot>(Icon->Slot);
    IconCanvasSlot->SetSize(Size);

    Icon->SetBrushFromTexture(Data->GetIconTexture(), true);
    ItemData = Data;
}