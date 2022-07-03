// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "WeaponShotgun.generated.h"

/**
 * 
 */
UCLASS()
class GE_II_P2_API AWeaponShotgun : public AWeapon
{
	GENERATED_BODY()
public:
	AWeaponShotgun();

protected:
	virtual void BeginPlay() override;
};
