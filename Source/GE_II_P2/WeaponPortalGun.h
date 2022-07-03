// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "WeaponPortalGun.generated.h"

/**
 * 
 */
UCLASS()
class GE_II_P2_API AWeaponPortalGun : public AWeapon
{
	GENERATED_BODY()

public:
AWeaponPortalGun();
	
protected:
	virtual void BeginPlay() override;
	
};
