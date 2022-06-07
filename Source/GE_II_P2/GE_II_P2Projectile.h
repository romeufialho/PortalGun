// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GE_II_P2Projectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;

UCLASS(config=Game)
class AGE_II_P2Projectile : public AActor
{
	GENERATED_BODY()

	/** Sphere collision component */
	UPROPERTY(VisibleDefaultsOnly, Category=Projectile)
	USphereComponent* CollisionComp;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* ProjectileMovement;

	// Mesh
	UPROPERTY(EditAnywhere, Category = "Projectile")
	UStaticMeshComponent* StaticMesh ;

public:
	AGE_II_P2Projectile();

	/** called when projectile hits something */
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	/** Returns CollisionComp subobject **/
	USphereComponent* GetCollisionComp() const { return CollisionComp; }
	/** Returns ProjectileMovement subobject **/
	UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }

	// Colour boolean
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Projectile")
	bool bIsBlue;

	// Set bIsBlueProjectile
	void SetIsBlueProjectile(bool bValue);

	// Material properties of projectile
	UPROPERTY(EditAnywhere, Category = "Projectile")
	UMaterialInterface* BlueMaterial;

	UPROPERTY(EditAnywhere, Category = "Projectile")
	UMaterialInterface* OrangeMaterial;

	void SetProjectileMaterial();
	
};
