#include "ItemInstance.h"

UItemInstance::UItemInstance()
    : UniqueID{FGuid::NewGuid()}
{
}

void UItemInstance::Init(UItemDataAsset* InItemData)
{
    ItemData = InItemData;
}

FGuid UItemInstance::GetID() const
{
    return UniqueID;
}

UItemDataAsset *UItemInstance::GetItemData() const
{
    return ItemData;
}
