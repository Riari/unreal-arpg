// Fill out your copyright notice in the Description page of Project Settings.


#include "EquipmentPanel.h"

#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/GridPanel.h"
#include "Components/Image.h"

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
    UTexture2D* ItemTexture = ItemData->GetIconTexture();
    ItemWidget->SetIconTexture(ItemTexture);
    UCanvasPanelSlot* CanvasPanelSlot = InventoryItemsCanvas->AddChildToCanvas(ItemWidget);
}

void UEquipmentPanel::NativeConstruct()
{
    Super::NativeConstruct();

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