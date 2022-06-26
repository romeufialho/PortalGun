// Copyright Epic Games, Inc. All Rights Reserved.

#include "GE_II_P2Character.h"
#include "GE_II_P2Projectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "MotionControllerComponent.h"
#include "DrawDebugHelpers.h"
#include "Templates/Casts.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// AGE_II_P2Character

AGE_II_P2Character::AGE_II_P2Character()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetOwnerNoSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

	// Change mesh component that will be viewed only by other that are not the player
	// (this is the default mesh component that is already present on the character)
	GetMesh()->SetOnlyOwnerSee(false);
	GetMesh()->SetOwnerNoSee(true);

	// Create a gun mesh component
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->SetOnlyOwnerSee(true);			// there is a duplicated gun (TP_Gun that is seen by others)
	FP_Gun->SetOwnerNoSee(false);
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;
	// FP_Gun->SetupAttachment(Mesh1P, TEXT("GripPoint"));
	FP_Gun->SetupAttachment(RootComponent);

	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetupAttachment(FP_Gun);
	FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 0.0f, 10.0f);

	// Note: The ProjectileClass and the skeletal mesh/anim blueprints for Mesh1P, FP_Gun, and VR_Gun 
	// are set in the derived blueprint asset named MyCharacter to avoid direct content references in C++.
	
	// Uncomment the following line to turn motion controllers on by default:
	//bUsingMotionControllers = true;

	GetCapsuleComponent()->SetGenerateOverlapEvents(true);


	//////////////////////////////////////////////////








}

void AGE_II_P2Character::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
	FP_Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));

	ThisWorld = GetWorld();

	MyHUD = Cast<AGE_II_P2HUD>(UGameplayStatics::GetPlayerController(this->GetOwner(), 0)->GetHUD()); 


	///////////////////////////////////////////////////////////////////////////

	RuntimeProjectileClass = Cast<AGE_II_P2Projectile>(ProjectileClass->GetClass());
	RuntimePortalProjectileClass = Cast<AGE_II_P2Projectile>(PortalProjectileClass->GetClass());
	RuntimeARProjectileClass = Cast<AGE_II_P2Projectile>(ARProjectileClass);
	RuntimeShotgunProjectileClass = Cast<AGE_II_P2Projectile>(ShotgunProjectileClass);
	RuntimeRocketProjectileClass = Cast<AGE_II_P2Projectile>(RocketProjectileClass);
	

}

void AGE_II_P2Character::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	


	//Gun Types

	//if (Cast<AGE_II_P2Projectile>(ProjectileClass)->GetBulletType() == 4) {
	HandlePortalPlacement();
	//}
	
}

//////////////////////////////////////////////////////////////////////////
// Input

void AGE_II_P2Character::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind fire event
	//Mouse Left Click
	PlayerInputComponent->BindAction("FireLeft", IE_Pressed, this, &AGE_II_P2Character::OnFireLeft);
	//Mouse Right Click
	PlayerInputComponent->BindAction("FireRight", IE_Pressed, this, &AGE_II_P2Character::OnFireRight);
	
	// Enable touchscreen input
	EnableTouchscreenMovement(PlayerInputComponent);

	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AGE_II_P2Character::OnResetVR);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &AGE_II_P2Character::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AGE_II_P2Character::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AGE_II_P2Character::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AGE_II_P2Character::LookUpAtRate);
}

void AGE_II_P2Character::OnFireLeft()
{
	if (RuntimeProjectileClass && RuntimeProjectileClass->GetBulletType() == RuntimePortalProjectileClass->GetBulletType()) {
		if (CanSpawnPortal)
		{
			OnFire()->SetIsBlueProjectile(true);

			//Cast<AGE_II_P2Projectile>(ProjectileClass)->SetIsBlueProjectile(true);

			//UE_LOG(LogTemp, Warning, TEXT("fired left"));

		}
	}
	else {
		//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, FString::Printf(TEXT("Not Portal Fire")));;
		if (RuntimeProjectileClass == nullptr) {
			GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, FString::Printf(TEXT("RUNTIME PROJ NULL PE TE ERRE WHYYYYYYYYYYYYYYYYYYY")));;
		}
	}
}

void AGE_II_P2Character::OnFireRight()
{
	if (RuntimeProjectileClass && RuntimeProjectileClass->GetBulletType() == RuntimePortalProjectileClass->GetBulletType()) {
		if (CanSpawnPortal)
		{
			OnFire()->SetIsBlueProjectile(false);

			//Cast<AGE_II_P2Projectile>(ProjectileClass)->SetIsBlueProjectile(false);

			//UE_LOG(LogTemp, Warning, TEXT("fired right"));

		}
	}
	else {


	}
}

AGE_II_P2Projectile* AGE_II_P2Character::OnFire() const
{
	// try and fire a projectile
	//if (ProjectileClass != nullptr)
	//{
		UWorld* const World = GetWorld();

	/*	if (World != nullptr)
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, FString::Printf(TEXT("World Existe")));;

		}*/

		//if (World != nullptr)
		//{
				const FRotator SpawnRotation = GetControlRotation();
				// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
				// const FVector SpawnLocation = ((FP_MuzzleLocation != nullptr) ? FP_MuzzleLocation->GetComponentLocation() : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);
				const FVector SpawnLocation =  (FirstPersonCameraComponent->GetComponentLocation() + FirstPersonCameraComponent->GetForwardVector() * FVector(200.f, 200.f,0));
	
				//Set Spawn Collision Handling Override
				FActorSpawnParameters ActorSpawnParams;
				ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
				//NOT ALL CONTROL PATHS RETURN A VALUE
				// -> commented if blocks
		//}
	//}

	// try and play the sound if specified
	if (FireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}

	// try and play a firing animation if specified
	if (FireAnimation != nullptr)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}

	if (RuntimeProjectileClass == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, FString::Printf(TEXT("+351 808242424")));;

		return NULL;
	}

	AGE_II_P2Projectile* projectile = World->SpawnActor<AGE_II_P2Projectile>(RuntimeProjectileClass->GetClass(), SpawnLocation, SpawnRotation, ActorSpawnParams);

	// spawn the projectile at the muzzle

	return projectile;
}

void AGE_II_P2Character::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AGE_II_P2Character::BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == true)
	{
		return;
	}
	if ((FingerIndex == TouchItem.FingerIndex) && (TouchItem.bMoved == false))
	{
		OnFire();
	}
	TouchItem.bIsPressed = true;
	TouchItem.FingerIndex = FingerIndex;
	TouchItem.Location = Location;
	TouchItem.bMoved = false;
}

void AGE_II_P2Character::EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == false)
	{
		return;
	}
	TouchItem.bIsPressed = false;
}

//Commenting this section out to be consistent with FPS BP template.
//This allows the user to turn without using the right virtual joystick

//void AGE_II_P2Character::TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location)
//{
//	if ((TouchItem.bIsPressed == true) && (TouchItem.FingerIndex == FingerIndex))
//	{
//		if (TouchItem.bIsPressed)
//		{
//			if (GetWorld() != nullptr)
//			{
//				UGameViewportClient* ViewportClient = GetWorld()->GetGameViewport();
//				if (ViewportClient != nullptr)
//				{
//					FVector MoveDelta = Location - TouchItem.Location;
//					FVector2D ScreenSize;
//					ViewportClient->GetViewportSize(ScreenSize);
//					FVector2D ScaledDelta = FVector2D(MoveDelta.X, MoveDelta.Y) / ScreenSize;
//					if (FMath::Abs(ScaledDelta.X) >= 4.0 / ScreenSize.X)
//					{
//						TouchItem.bMoved = true;
//						float Value = ScaledDelta.X * BaseTurnRate;
//						AddControllerYawInput(Value);
//					}
//					if (FMath::Abs(ScaledDelta.Y) >= 4.0 / ScreenSize.Y)
//					{
//						TouchItem.bMoved = true;
//						float Value = ScaledDelta.Y * BaseTurnRate;
//						AddControllerPitchInput(Value);
//					}
//					TouchItem.Location = Location;
//				}
//				TouchItem.Location = Location;
//			}
//		}
//	}
//}

void AGE_II_P2Character::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AGE_II_P2Character::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AGE_II_P2Character::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AGE_II_P2Character::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

bool AGE_II_P2Character::EnableTouchscreenMovement(class UInputComponent* PlayerInputComponent)
{
	if (FPlatformMisc::SupportsTouchInput() || GetDefault<UInputSettings>()->bUseMouseForTouch)
	{
		PlayerInputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AGE_II_P2Character::BeginTouch);
		PlayerInputComponent->BindTouch(EInputEvent::IE_Released, this, &AGE_II_P2Character::EndTouch);

		//Commenting this out to be more consistent with FPS BP template.
		//PlayerInputComponent->BindTouch(EInputEvent::IE_Repeat, this, &AGE_II_P2Character::TouchUpdate);
		return true;
	}
	
	return false;
}

bool AGE_II_P2Character::HandlePortalPlacement()
{
	//Set up the properties for the linetrace
	FHitResult LineOutHit;

	FVector Start =  FirstPersonCameraComponent->GetComponentLocation() + FirstPersonCameraComponent->GetForwardVector() * FVector(20.f, 20.f,0);

	FVector End = Start + (FirstPersonCameraComponent->GetForwardVector() * 100000.f);

	FCollisionQueryParams CollisionParams;
	FCollisionResponseParams ResponseParam;

	//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Blue, FString::Printf(TEXT("amogos %s"), *GetDebugName(OutHit.GetActor())));

	//Linetrace that looks at where the player is looking at
	if (ThisWorld->LineTraceSingleByChannel(LineOutHit, Start, End, ECC_Visibility, CollisionParams))
	{
		//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, FString::Printf(TEXT("hit %s"), *GetDebugName(OutHit.GetActor())));

		//if the player is looking at an object with the tag that determines if that wall can be a portal wall or not
		if (LineOutHit.GetActor()->ActorHasTag(TEXT("Wall")))
		{
			//GEngine->AddOnScreenDebugMessage(-1, 0.03f, FColor::Green, FString::Printf(TEXT("Portal Wall")));	
			
			// Sets up all the properties needed for the linetraces that check if there are any walls on the sides
			FHitResult RightHit;
			bool DidHitRight = false;

			FHitResult LeftHit;
			bool DidHitLeft = false;

			FHitResult UpHit;
			bool DidHitUp = false;

			FHitResult DownHit;
			bool DidHitDown = false;


			// Sets up all the properties needed for the linetraces that check if the wall as ended
			FHitResult EndRightHit;
			bool DidEndRight = false;

			FHitResult EndLeftHit;
			bool DidEndLeft = false;

			FHitResult EndUpHit;
			bool DidEndUp = false;

			FHitResult EndDownHit;
			bool DidEndDown = false;

			//Rotator for the calculations of the line traces
			FRotator HitRotator = FRotator(0, 90, 0);


			//right wall check
			DidHitRight = ThisWorld->LineTraceSingleByChannel(RightHit, LineOutHit.Location + LineOutHit.Normal* 10, LineOutHit.Location + LineOutHit.Normal * 10 + (HitRotator.RotateVector(LineOutHit.Normal) * PortalRadius), ECC_Visibility, CollisionParams);
			DrawDebugLine(ThisWorld, LineOutHit.Location + LineOutHit.Normal * 10, LineOutHit.Location + LineOutHit.Normal * 10 + (HitRotator.RotateVector(LineOutHit.Normal) * PortalRadius), FColor::Red, false, 0.02f, 0, 10);
			
			//left wall check
			DidHitLeft = ThisWorld->LineTraceSingleByChannel(LeftHit, LineOutHit.Location + LineOutHit.Normal * 10, LineOutHit.Location + LineOutHit.Normal * 10 + (HitRotator.RotateVector(LineOutHit.Normal) * -PortalRadius), ECC_Visibility, CollisionParams);
			DrawDebugLine(ThisWorld, LineOutHit.Location + LineOutHit.Normal * 10, LineOutHit.Location + LineOutHit.Normal * 10 + (HitRotator.RotateVector(LineOutHit.Normal) * -PortalRadius), FColor::Blue, false, 0.02f, 0, 10);
			

			//up wall check
			DidHitUp = ThisWorld->LineTraceSingleByChannel(UpHit, LineOutHit.Location + LineOutHit.Normal * 10, LineOutHit.Location + LineOutHit.Normal * 10 + (LineOutHit.Normal.UpVector * PortalHalfHeight), ECC_Visibility, CollisionParams);
			DrawDebugLine(ThisWorld, LineOutHit.Location + LineOutHit.Normal * 10, LineOutHit.Location + LineOutHit.Normal * 10 + (LineOutHit.Normal.UpVector * PortalHalfHeight), FColor::Yellow, false, 0.02f, 0, 10);


			//down wall check
			DidHitDown = ThisWorld->LineTraceSingleByChannel(DownHit, LineOutHit.Location + LineOutHit.Normal, LineOutHit.Location + LineOutHit.Normal * 10 + (LineOutHit.Normal.UpVector * -PortalHalfHeight), ECC_Visibility, CollisionParams);
			DrawDebugLine(ThisWorld, LineOutHit.Location + LineOutHit.Normal * 10, LineOutHit.Location + LineOutHit.Normal * 10 + (LineOutHit.Normal.UpVector * -PortalHalfHeight), FColor::Purple, false, 0.02f, 0, 10);


			//normal vector
			//DrawDebugLine(ThisWorld, LineOutHit.Location + LineOutHit.Normal, LineOutHit.Location + LineOutHit.Normal + (LineOutHit.Normal * PortalRadius*2), FColor::Green, false, 0.02f, 0, 20);

			//if any of the line traces DID hit a wall, you can NOT place a portal. IF no traces hit any walls, check if the wall has ended
			if (DidHitRight || DidHitLeft || DidHitUp || DidHitDown) 
			{
				//BRUH, CRINGE NAO PODES SPAWNAR O PORTAL
				GEngine->AddOnScreenDebugMessage(-1, -1.f, FColor::Red, FString::Printf(TEXT("No Portals Here")));

				GEngine->AddOnScreenDebugMessage(-1, -1.f, FColor::Red, FString::Printf(TEXT("Right %s"), DidHitRight? TEXT("true") : TEXT("false")));
				GEngine->AddOnScreenDebugMessage(-1, -1.f, FColor::Red, FString::Printf(TEXT("Left %s"), DidHitLeft? TEXT("true") : TEXT("false")));
				GEngine->AddOnScreenDebugMessage(-1, -1.f, FColor::Red, FString::Printf(TEXT("Up %s"), DidHitUp? TEXT("true") : TEXT("false")));
				GEngine->AddOnScreenDebugMessage(-1, -1.f, FColor::Red, FString::Printf(TEXT("Down %s"), DidHitDown? TEXT("true") : TEXT("false")));

				//If the crosshair is showing. hide it
				CanSpawnPortal = false;
				
				if (MyHUD->bShowHUD)
				{
					MyHUD->ShowHUD();
				}
			}
			else 
			{
				//CHECK IF THE WALL DOENST END
				DidEndRight = ThisWorld->LineTraceSingleByChannel(EndRightHit, (LineOutHit.Location + LineOutHit.Normal + (HitRotator.RotateVector(LineOutHit.Normal) * PortalRadius)), (LineOutHit.Location + LineOutHit.Normal + (HitRotator.RotateVector(LineOutHit.Normal) * PortalRadius)) - LineOutHit.Normal * 100.f, ECC_Visibility, CollisionParams);
				DrawDebugLine(ThisWorld, (LineOutHit.Location + LineOutHit.Normal + (HitRotator.RotateVector(LineOutHit.Normal) * PortalRadius)), (LineOutHit.Location + LineOutHit.Normal + (HitRotator.RotateVector(LineOutHit.Normal) * PortalRadius)) - LineOutHit.Normal * 100.f, FColor::Red, false, 0.02f, 0, 10);

				DidEndLeft = ThisWorld->LineTraceSingleByChannel(EndLeftHit, (LineOutHit.Location + LineOutHit.Normal + (HitRotator.RotateVector(LineOutHit.Normal) * -PortalRadius)), (LineOutHit.Location + LineOutHit.Normal + (-HitRotator.RotateVector(LineOutHit.Normal) * PortalRadius)) - LineOutHit.Normal * 100.f, ECC_Visibility, CollisionParams);
				DrawDebugLine(ThisWorld, (LineOutHit.Location + LineOutHit.Normal + (HitRotator.RotateVector(LineOutHit.Normal) * -PortalRadius)), (LineOutHit.Location + LineOutHit.Normal + (-HitRotator.RotateVector(LineOutHit.Normal) * PortalRadius)) - LineOutHit.Normal * 100.f, FColor::Blue, false, 0.02f, 0, 10);

				DidEndUp = ThisWorld->LineTraceSingleByChannel(EndUpHit, (LineOutHit.Location + LineOutHit.Normal + (LineOutHit.Normal.UpVector * PortalHalfHeight)), (LineOutHit.Location + LineOutHit.Normal + (LineOutHit.Normal.UpVector * PortalHalfHeight)) - LineOutHit.Normal * 100.f, ECC_Visibility, CollisionParams);
				DrawDebugLine(ThisWorld, (LineOutHit.Location + LineOutHit.Normal + (LineOutHit.Normal.UpVector * PortalHalfHeight)), (LineOutHit.Location + LineOutHit.Normal + (LineOutHit.Normal.UpVector * PortalHalfHeight)) - LineOutHit.Normal * 100.f, FColor::Yellow, false, 0.02f, 0, 10);

				DidEndDown = ThisWorld->LineTraceSingleByChannel(EndDownHit, (LineOutHit.Location + LineOutHit.Normal + (LineOutHit.Normal.UpVector * PortalHalfHeight)), (LineOutHit.Location + LineOutHit.Normal + (LineOutHit.Normal.UpVector * -PortalHalfHeight)) - LineOutHit.Normal * 100.f, ECC_Visibility, CollisionParams);
				DrawDebugLine(ThisWorld, (LineOutHit.Location + LineOutHit.Normal + (LineOutHit.Normal.UpVector * -PortalHalfHeight)), (LineOutHit.Location + LineOutHit.Normal + (LineOutHit.Normal.UpVector * -PortalHalfHeight)) - LineOutHit.Normal * 100.f, FColor::Purple, false, 0.02f, 0, 10);
				
				if (!DidEndRight || !DidEndLeft || !DidEndUp || !DidEndDown)
				{
					//BRUH, CRINGE NAO PODES SPAWNAR O PORTAL
					GEngine->AddOnScreenDebugMessage(-1, -1.f, FColor::Red, FString::Printf(TEXT("No Portals Here")));

					//If the crosshair is showing. hide it
					CanSpawnPortal = false;
					
					if (MyHUD->bShowHUD)
					{
						MyHUD->ShowHUD();
					}
				}
				else 
				{
					//VERY POGGERS WOW! PODES SPAWNAR O PORTAL
					GEngine->AddOnScreenDebugMessage(-1, -1.f, FColor::Green, FString::Printf(TEXT("You Can Spawn Portals Here")));

					//If the crossair is HIDING. show it
					if (!MyHUD->bShowHUD)
					{
						MyHUD->ShowHUD();
					}

					//CAN SPAWN PORTAL CODE HERE
					CanSpawnPortal = true;

				}

			}


		}
		else
		{
			//DrawDebugLine(ThisWorld, Start, End, FColor::Red, false, 0.02f, 0, 5);
			
			//If the crosshair is showing. hide it
			CanSpawnPortal = false;
			
			if (MyHUD->bShowHUD) 
			{
				MyHUD->ShowHUD();
			}
			
		}
	}
	return false;
}
