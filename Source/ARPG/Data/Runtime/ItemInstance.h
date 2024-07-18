#pragma once

#include "CoreMinimal.h"
#include "../Definitions/ItemDataAsset.h"
#include "ItemInstance.generated.h"

UCLASS(Blueprintable)
class ARPG_API UItemInstance : public UObject
{
    GENERATED_BODY()

public:
    UItemInstance();

    void Init(UItemDataAsset* InItemData);

    FGuid GetID() const;
    UItemDataAsset* GetItemData() const;

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item")
    FGuid UniqueID;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item")
    UItemDataAsset* ItemData;
};