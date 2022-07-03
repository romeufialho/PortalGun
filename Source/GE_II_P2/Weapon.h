// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

UCLASS()
class GE_II_P2_API AWeapon : public AActor
{
	GENERATED_BODY()

	// ////////////////////// PROPERTIES & COMPONENTS //////////////////////////////////
public:	
	// Sets default values for this actor's properties
	AWeapon();

	// Mesh
	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	USkeletalMeshComponent* WeaponMesh;

	// Weapon ID
	UPROPERTY()
	int WeaponID;
	// 0 = AR
	// 1 = Shotgun
	// 2 = Rocket
	// 3 = Portal

protected:

	// /////////////////////// FUNCTIONS ////////////////////////////////////
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
