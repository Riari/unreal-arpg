// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponActor.generated.h"

UCLASS(Blueprintable)
class ARPG_API AWeaponActor : public AActor
{
	GENERATED_BODY()
	
public:
	AWeaponActor();

	void Swing();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(Category = "Components", VisibleAnywhere, BlueprintReadWrite)
	class USkeletalMeshComponent* Mesh;

	UPROPERTY(Category = "Effects", EditAnywhere)
	TArray<class USoundBase*> SwingSounds;

};
