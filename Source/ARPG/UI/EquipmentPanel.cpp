// Fill out your copyright notice in the Description page of Project Settings.


#include "EquipmentPanel.h"

#include "../Data/ItemDataAsset.h"
#include "Components/CanvasPanel.h"
#include "Components/GridPanel.h"
#include "Components/Image.h"

void UEquipmentPanel::CreateSlots(UGridPanel* InventoryGrid)
{
    if (SlotWidgetClass == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("UEquipmentPanel: SlotWidgetClass is null!"));
        return;
    }

    if (InventoryGrid == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("UEquipmentPanel: InventoryGrid is null! Ensure there is a GridPanel widget of this name exposed as a variable."));
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

void UEquipmentPanel::AddItemToInventory(UItemDataAsset* ItemData)
{
    if (InventoryItemsContainer == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("UEquipmentPanel: InventoryItemsContainer is null! Ensure there is a CanvasPanel widget of this name exposed as a variable."));
        return;
    }

    UImage* ItemIcon = ConstructObject<UImage>(UImage::StaticClass());
    ItemIcon->SetBrushFromTexture(ItemData->GetIconTexture(), true);
    InventoryItemsContainer->AddChildToCanvas(ItemIcon);
}
