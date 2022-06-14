// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GE_II_P2Projectile.h"
#include "MyProjectileClass.generated.h"

/**
 * 
 */
UCLASS()
class GE_II_P2_API AMyProjectileClass : public AGE_II_P2Projectile
{
	GENERATED_BODY()

public:

		virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;


protected:
	UPROPERTY(EditAnywhere, Category = "Projectile")
		float BulletDamageAmmount;

	UPROPERTY(EditAnywhere, Category = "Projectile")
		float BulletHeadShotMultiplier;

	UPROPERTY(EditAnywhere, Category = "Projectile")
		int BulletType;

	
};
