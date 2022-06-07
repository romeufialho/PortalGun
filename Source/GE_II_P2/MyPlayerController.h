// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Portal_Manager.h"
#include "GameFramework/PlayerController.h"
#include "MyPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class GE_II_P2_API AMyPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	APortal_Manager* GetPortal_Manager();

	// Portal class
	UPROPERTY(EditDefaultsOnly, Category=Portal)
	TSubclassOf<class APortal> PortalClass;
	
	// PortalManager class
	UPROPERTY(EditDefaultsOnly, Category=Portal)
	TSubclassOf<class APortal_Manager> PortalManagerClass;

protected:
	APortal_Manager* Portal_Manager;

	virtual void BeginPlay() override;
};
