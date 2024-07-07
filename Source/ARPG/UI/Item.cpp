// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"

#include "Components/Image.h"

void UItem::SetIconTexture(TObjectPtr<UTexture2D> texture)
{
    IconImage->SetBrushFromTexture(texture, true);
}