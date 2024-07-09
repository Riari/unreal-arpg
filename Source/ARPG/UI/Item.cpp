// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"

#include "Components/Image.h"

#include "../Data/ItemDataAsset.h"

void UItem::Init(TObjectPtr<UItemDataAsset> Data)
{
    ItemData = Data;
    IconImage->SetBrushFromTexture(Data->GetIconTexture(), true);
}