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

	void SetForceAttackMode(bool bForceAttackModeEnabled);
	void Attack();

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

