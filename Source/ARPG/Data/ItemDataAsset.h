// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ItemDataAsset.generated.h"

/**
 * 
 */
UCLASS(abstract, Blueprintable)
class ARPG_API UItemDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

	FPrimaryAssetId GetPrimaryAssetId() const override { return FPrimaryAssetId("ItemDataAssets", GetFName()); } 

public:
	UFUNCTION(BlueprintCallable)
	FName GetName() const;

	UFUNCTION(BlueprintCallable)
	int GetInventoryWidth() const;

	UFUNCTION(BlueprintCallable)
	int GetInventoryHeight() const;

	UFUNCTION(BlueprintCallable)
	UTexture2D* GetIconTexture() const;

	UFUNCTION(BlueprintCallable)
	UStaticMesh* GetMesh() const;

protected:
	UPROPERTY(EditAnywhere, Category = Details)
	FName Name;

	UPROPERTY(EditAnywhere, Category = Details)
	int InventoryWidth;

	UPROPERTY(EditAnywhere, Category = Details)
	int InventoryHeight;

	UPROPERTY(EditAnywhere, Category = Visuals)
	UTexture2D* IconTexture;

	UPROPERTY(EditAnywhere, Category = Visuals)
	UStaticMesh* Mesh;
};
