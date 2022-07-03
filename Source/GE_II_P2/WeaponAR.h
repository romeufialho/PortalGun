// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "WeaponAR.generated.h"

/**
 * 
 */
UCLASS()
class GE_II_P2_API AWeaponAR : public AWeapon
{
	GENERATED_BODY()

	// PROPERTIES

	// FUNCTIONS
public:
	AWeaponAR();
	
protected:
	
	virtual void BeginPlay() override;
	
};



