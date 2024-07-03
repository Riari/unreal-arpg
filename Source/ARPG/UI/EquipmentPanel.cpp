// Fill out your copyright notice in the Description page of Project Settings.


#include "EquipmentPanel.h"

#include "../Data/ItemDataAsset.h"
#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/GridPanel.h"
#include "Components/Image.h"

void UEquipmentPanel::AddItemToInventory(UItemDataAsset* ItemData)
{
    if (InventoryItemsCanvas == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("UEquipmentPanel: InventoryItemsCanvas is null! Ensure there is a UCanvasPanel widget of this name exposed as a variable."));
        return;
    }

    UImage* ItemIcon = NewObject<UImage>(this, UImage::StaticClass());
    UTexture2D* ItemTexture = ItemData->GetIconTexture();
    ItemIcon->SetBrushFromTexture(ItemTexture, true);
    UCanvasPanelSlot* CanvasPanelSlot = InventoryItemsCanvas->AddChildToCanvas(ItemIcon);
    CanvasPanelSlot->SetSize(FVector2D(ItemTexture->GetSurfaceWidth(), ItemTexture->GetSurfaceHeight()));
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