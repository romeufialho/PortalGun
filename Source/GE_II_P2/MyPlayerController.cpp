// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerController.h"

APortal_Manager* AMyPlayerController::GetPortal_Manager()
{
	return Portal_Manager;
}

void AMyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Place Portal Manager on the Scene ( this doesnt let you choose the Portal_BP blueprint to spawn )
	Portal_Manager = GetWorld()->SpawnActor<APortal_Manager>(PortalManagerClass, FVector::ZeroVector, FRotator::ZeroRotator);
	Portal_Manager->AttachToActor(this, FAttachmentTransformRules::SnapToTargetIncludingScale);
	Portal_Manager->SetPortalClass(PortalClass);
}


