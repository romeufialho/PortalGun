// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "WeaponSpawner.generated.h"

UCLASS()
class GE_II_P2_API AWeaponSpawner : public AActor
{
	GENERATED_BODY()

	// ////////////////////// PROPERTIES & COMPONENTS //////////////////////////////////
public:	
	// Sets default values for this actor's properties
	AWeaponSpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Mesh
	UPROPERTY(VisibleAnywhere, Category = "WeaponSpawner")
	UStaticMeshComponent* StaticMesh ;

	//Weapon to Spawn
	UPROPERTY(VisibleAnywhere, Category = "WeaponSpawner")
	USkeletalMeshComponent* Weapon;

	//Trigger
	UPROPERTY(VisibleAnywhere, Category= "WeaponSpawner")
	UBoxComponent* BoxComponent;

private:

	
	// /////////////////////// FUNCTIONS /////////////////////////////////////
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	static void SpawnWeapon(USkeletalMeshComponent* Weapon);

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

protected:

	
private:
	
	

};
