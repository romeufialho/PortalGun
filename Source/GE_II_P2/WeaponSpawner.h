// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
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

	//Weapon to Spawn
	UPROPERTY(EditAnywhere, Category = "WeaponSpawner", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AWeapon> WeaponToSpawn;

	UPROPERTY()
	AWeapon* SpawnedWeapon;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Mesh
	UPROPERTY(VisibleAnywhere, Category = "WeaponSpawner")
	UStaticMeshComponent* StaticMesh ;

	//Location to Spawn Weapon
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "WeaponSpawner")
	USceneComponent* WeaponLocation;

	//Trigger
	UPROPERTY(VisibleAnywhere, Category= "WeaponSpawner")
	UBoxComponent* BoxComponent;

private:
	FTimerHandle _loopTimeHandle;

	float RespawnTime = 5.f;

	
	// /////////////////////// FUNCTIONS /////////////////////////////////////
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SpawnWeapon();

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

	UFUNCTION()
	void StartTimer();

	UFUNCTION()
	void OnTimerEnd();

protected:

	
private:
	
	

};
