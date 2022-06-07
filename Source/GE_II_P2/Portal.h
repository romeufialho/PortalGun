// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GE_II_P2Character.h"
#include "Components/BoxComponent.h"
#include "Engine/TextureRenderTarget2D.h"
#include "GameFramework/Actor.h"
#include "Portal.generated.h"

UCLASS()
class GE_II_P2_API APortal : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APortal();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Portal camera
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Portal")
	USceneCaptureComponent2D* SceneCapture;

	// Portal meshes
	UPROPERTY(VisibleAnywhere, Category = "Portal")
	UStaticMeshComponent* PortalScreenMesh;
	
	UPROPERTY(VisibleAnywhere, Category = "Portal")
	UStaticMeshComponent* PortalBorderMesh;
	
	// Portal Collision
	UPROPERTY(VisibleAnywhere,Category = "Portal")
	UBoxComponent* BoxComponent;
	
	FVector CharacterLocation;
	FRotator CharacterRotation;
	FVector SelfLocation;
	FRotator SelfRotation;
	FVector CameraLocation;
	FRotator CameraRotation;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Materials
	UPROPERTY(EditAnywhere, Category="Portal")
	UMaterialInterface* BlueMaterial;

	UPROPERTY(EditAnywhere, Category="Portal")
	UMaterialInterface* OrangeMaterial;

	UPROPERTY(EditAnywhere, Category = "Portal")
	UMaterialInterface* PortalVisionMaterial_1;

	UPROPERTY(EditAnywhere, Category = "Portal")
	UMaterialInterface* PortalVisionMaterial_2;

	// Texture Target
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Portal")
	UTextureRenderTarget2D* PortalVision_1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Portal")
	UTextureRenderTarget2D* PortalVision_2;

	// Portal type boolean
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Portal")
	bool IsBluePortal;
	
	bool bCanEnterPortal = true;
	
	UPROPERTY()
	APortal* OtherPortal;
	
	// Function to change IsBluePortal boolean
	void SetIsBluePortal(bool bValue);

	void SetPortalMaterial() const;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);
};
