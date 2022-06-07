// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Portal_Manager.generated.h"

class APortal;

UCLASS()
class GE_II_P2_API APortal_Manager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APortal_Manager();

	// Spawn Portal
	APortal* SpawnPortal(FHitResult HitResult, APortal* Target);
	APortal* SpawnOrangePortal(FHitResult HitResult);
	APortal* SpawnBluePortal(FHitResult HitResult);

	void SetPortalClass(TSubclassOf<class APortal> Class);

	// Portal class to default
	UPROPERTY(EditAnywhere, Category=Portal, meta = (AllowPrivateAccess = true))
	TSubclassOf<class APortal> PortalClass;

	// types of portals
	APortal* BluePortal = nullptr;
	APortal* OrangePortal = nullptr;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
