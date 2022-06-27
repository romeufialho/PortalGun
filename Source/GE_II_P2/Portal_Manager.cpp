// Fill out your copyright notice in the Description page of Project Settings.

#include "Portal_Manager.h"
#include "Portal.h"

// Sets default values
APortal_Manager::APortal_Manager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

APortal* APortal_Manager::SpawnPortal(FHitResult HitResult, APortal* Target)
{
	APortal* Portal = nullptr;

	UWorld* const World = GetWorld();
	
	FActorSpawnParameters ActorSpawnParameters;
	ActorSpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	FVector ImpactPoint = HitResult.Location + (HitResult.ImpactNormal * 1.f);
	FRotator Rotation =  HitResult.ImpactNormal.Rotation();
	
	Portal = World->SpawnActor<APortal>(PortalClass, ImpactPoint, Rotation, ActorSpawnParameters);

	if (Target != nullptr)
	{
		Portal->OtherPortal = Target;
		Target->OtherPortal = Portal;
	}
	
	return Portal;
}

APortal* APortal_Manager::SpawnOrangePortal(FHitResult HitResult)
{
	if (OrangePortal != nullptr)
	{
		OrangePortal->Destroy();
	}
	return OrangePortal = SpawnPortal(HitResult, BluePortal);
}

APortal* APortal_Manager::SpawnBluePortal(FHitResult HitResult)
{
	if (BluePortal != nullptr)
	{
		BluePortal->Destroy();
	}
	return BluePortal = SpawnPortal(HitResult, OrangePortal);
}

void APortal_Manager::SetPortalClass(TSubclassOf<APortal> Class)
{
	PortalClass = Class;
}

// Called when the game starts or when spawned
void APortal_Manager::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void APortal_Manager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}




