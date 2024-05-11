// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseMobType.generated.h"

UCLASS()
class ARPG_API ABaseMobType : public ACharacter
{
	GENERATED_BODY()

public:
	ABaseMobType();

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	void OnMouseOverBegin(UPrimitiveComponent* TouchedComponent);

	UFUNCTION()
	void OnMouseOverEnd(UPrimitiveComponent* TouchedComponent);

protected:
	UPROPERTY(Category = "Appearance", EditAnywhere)
	float TextureSampleMultiplierHover{2.0f};

	UPROPERTY(Category = "Appearance", EditAnywhere)
	TArray<class UMaterialInterface*> Materials;

	TArray<class UMaterialInstanceDynamic*> MaterialInstances;

	virtual void BeginPlay() override;

private:
	class APlayerController* CurrentPlayerController;

	void SetTextureSampleMultiplier(float Multiplier);

};
