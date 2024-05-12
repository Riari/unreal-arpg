// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ARPGCharacter.generated.h"

UCLASS(Blueprintable)
class AARPGCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AARPGCharacter();

	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	virtual void Tick(float DeltaTime) override;

	class AWeaponActor* GetCurrentWeapon();

	void SetForceAttackMode(bool bForceAttackModeEnabled);

	/**
	 * Writes out the mob actors that would be hit, if any, by attacking from the current location with the current rotation and weapon.
	 * @return true if any mob actors would be hit, false otherwise
	 */
	bool TryGetTargetMobActors(TArray<class ABaseMobType*>& OutTargetMobActors);

	/**
	 * Attacks the target mob actor.
	 * @return true if the attack was successful, false otherwise
	 */
	bool AttackTarget(ABaseMobType* TargetMobActor);

protected:
	UPROPERTY(Category = "Combat", EditDefaultsOnly)
	TSubclassOf<class AWeaponActor> MainHandWeaponClass;

	UPROPERTY(Category = "Combat", VisibleAnywhere, BlueprintReadOnly)
	bool bIsAttacking{false};

	bool bIsInForceAttackMode{false};

	virtual void BeginPlay() override;

private:
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	class AWeaponActor* CurrentWeapon;

	float AttackDuration{0.7f};
	float AttackTimer{0.0f};
};

