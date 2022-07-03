// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "WeaponRocketLauncher.generated.h"

/**
 * 
 */
UCLASS()
class GE_II_P2_API AWeaponRocketLauncher : public AWeapon
{
	GENERATED_BODY()

public:
	AWeaponRocketLauncher();

protected:
	virtual void BeginPlay() override;
	
};
