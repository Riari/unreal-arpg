// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemDataAsset.h"

FName UItemDataAsset::GetName() const
{
    return Name;
}

int UItemDataAsset::GetInventoryWidth() const
{
    return InventoryWidth;
}

int UItemDataAsset::GetInventoryHeight() const
{
    return InventoryHeight;
}

UTexture2D* UItemDataAsset::GetIconTexture() const
{
    return IconTexture;
}

UStaticMesh* UItemDataAsset::GetMesh() const
{
    return Mesh;
}
