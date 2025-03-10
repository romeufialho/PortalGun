// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GE_II_P2HUD.h"
#include "GE_II_P2Projectile.h"
#include "Weapon.h"
#include "GameFramework/Character.h"
#include "Templates/Casts.h"
#include "Math/Color.h"


#include "GE_II_P2Character.generated.h"


class UInputComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UCameraComponent;
class UMotionControllerComponent;
class UAnimMontage;
class USoundBase;

UCLASS(config=Game)
	class AGE_II_P2Character : public ACharacter
{
	GENERATED_BODY()
	
	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	USkeletalMeshComponent* Mesh1P;

	/** Gun mesh: 1st person view (seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* FP_Gun;

	/** Location on gun mesh where projectiles should spawn. */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USceneComponent* FP_MuzzleLocation;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

public:
	AGE_II_P2Character();

protected:
	virtual void BeginPlay();

	virtual void Tick(float DeltaSeconds) override;

public:
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	FVector GunOffset;


	///////////////////////////////////////////////////////////////////////////////////////////////////


	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Projectile)
		//TSubclassOf<AActor> ProjectileClass;
		TSubclassOf<AGE_II_P2Projectile> ProjectileClass;



	///////////////////////////////////////////////////////////////////////////////////////////////////


	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	USoundBase* FireSound;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UAnimMontage* FireAnimation;

	/** Whether to use motion controller location for aiming. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	uint8 bUsingMotionControllers : 1;


protected:
	
	/** Fires a projectile. */
	AGE_II_P2Projectile* OnFire() const;

	// Fires Blue Portal
	void OnFireLeft();

	//Fires Orange Portal
	void OnFireRight();

	/** Resets HMD orientation and position in VR. */
	void OnResetVR();

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	///////////////////////////////////

	void NextWeapon();

	void PreviousWeapon();

	void Reload();

	struct TouchData
	{
		TouchData() { bIsPressed = false;Location=FVector::ZeroVector;}
		bool bIsPressed;
		ETouchIndex::Type FingerIndex;
		FVector Location;
		bool bMoved;
	};
	void BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	void EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	void TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location);
	TouchData	TouchItem;
	
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

	/* 
	 * Configures input for touchscreen devices if there is a valid touch interface for doing so 
	 *
	 * @param	InputComponent	The input component pointer to bind controls to
	 * @returns true if touch controls were enabled.
	 */
	bool EnableTouchscreenMovement(UInputComponent* InputComponent);

	//horizontal radius of the portal
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PORTAL STUFF")
	float PortalRadius = 50.f;

	//vertical half height of the portal
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PORTAL STUFF")
	float PortalHalfHeight = 100.f;

	bool CanSpawnPortal;
	
	//reference to HUD object
	AGE_II_P2HUD* MyHUD;

	//reference to world
	UWorld* ThisWorld;



//////////////////////////////////////////////////////////////////////////////////////////////////////


	//function that checks if the portal can be placed or not
	bool HandlePortalPlacement();



public:
	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	int CurrentBulletType = 0;
	int AR_BulletType = 0;
	int Shotgun_BulletType = 1;
	int Rocket_BulletType = 2;
	int Portal_BulletType = 3;

	// 0 = AR
	// 1 = Shotgun
	// 2 = Rocket
	// 3 = Portal

	//virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);


	UPROPERTY(EditDefaultsOnly, Category=Projectile)
	float MaxHP = 100.f;

	float CurrentHP = 100.f;

	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TArray<float>Size_BulletTypesArray;

	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TArray<FLinearColor>Colors_BulletTypesArray;

	UPROPERTY(EditDefaultsOnly, Category = Projectile)
		TArray<float>Damage_BulletTypesArray = { 20.f, 5.f, 80.f, 0.f };

	UPROPERTY(EditDefaultsOnly, Category = Projectile)
		TArray<float>FireRate_BulletTypesArray = { 1 / 5.f, 2 / 1.f, 5.f, 0.f };
	
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
		TArray<float>Radius_BulletTypesArray = { 1.f, 20.f, 40.f, 0.f };

	UPROPERTY(EditAnywhere, Category="Weapon")
	TArray<AWeapon*>WeaponInventory;


	int GetPlayerCurrentBulletType();

	float GetCurrentPlayer_DamageBulletType();

	float GetCurrentPlayer_FireRateBulletType();

	float GetCurrentPlayer_RadiusBulletType();

	void SetGunColor(FLinearColor ColorIn);


	
};

