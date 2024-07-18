// Fill out your copyright notice in the Description page of Project Settings.


#include "InventorySlotHover.h"

#include "Components/Image.h"

void UInventorySlotHover::SetValid(bool bValid)
{
    if (bIsValid == bValid) return;

    bIsValid = bValid;
    Highlight->SetColorAndOpacity(bIsValid ? ValidColor : InvalidColor);
}
