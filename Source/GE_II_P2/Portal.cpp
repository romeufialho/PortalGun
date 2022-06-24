// Fill out your copyright notice in the Description page of Project Settings.


#include "Portal.h"

#include "GE_II_P2Character.h"
#include "MyMathLibrary.h"
#include "MyPlayerController.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/SplineMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"


// Sets default values
APortal::APortal()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	SetRootComponent(RootComponent);
	
	PortalScreenMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Screen Mesh"));
	PortalScreenMesh->SetupAttachment(RootComponent);
	PortalBorderMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Border Mesh"));
	PortalBorderMesh->SetupAttachment(RootComponent);

	SceneCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("Camera"));
	SceneCapture->SetupAttachment(RootComponent);

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision Capsule"));
	BoxComponent->SetupAttachment(RootComponent);

	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &APortal::OnOverlapBegin);
	BoxComponent->OnComponentEndOverlap.AddDynamic(this, &APortal::OnOverlapEnd);

	BoxComponent->SetGenerateOverlapEvents(true);

	RotatedSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RotatedSceneComponent"));
	RotatedSceneComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void APortal::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void APortal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (OtherPortal != nullptr)
	{
		// Resize Render Target
		FVector2D Result;
		/*if (GEngine && GEngine->GameViewport)
		{
			GEngine->GameViewport->GetViewportSize(Result);
			SceneCapture->TextureTarget->ResizeTarget(Result.X, Result.Y);
		}*/

		// Portal Camera Location and Rotation
		if (RotatedSceneComponent->IsValidLowLevelFast())
		{
			FTransform Transform = RotatedSceneComponent->GetComponentTransform();
			APlayerCameraManager* CameraManager = Cast<APlayerCameraManager>(UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0));
			if (CameraManager != nullptr)
			{
				FTransform ParentTransform = CameraManager->GetTransform();
				FTransform NewTransform = UKismetMathLibrary::MakeRelativeTransform(ParentTransform, Transform);
				FHitResult HitResult;
				OtherPortal->SceneCapture->SetRelativeLocationAndRotation(
					NewTransform.GetLocation(), NewTransform.GetRotation(), false, &HitResult, ETeleportType::None);
			}

			// Custom Near Clipping Plane
			float NearClippingDistance = 1.f + UKismetMathLibrary::Vector_Distance(
				CameraManager->GetTransform().GetLocation(), this->GetActorLocation());
			OtherPortal->SceneCapture->CustomNearClippingPlane = NearClippingDistance;
		}	
	}
}

void APortal::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//Begin Teleport verifications
	if (bCanEnterPortal)
	{
		if (OtherPortal != nullptr)
		{
			AMyPlayerController* PlayerController = Cast<AMyPlayerController>(GetWorld()->GetFirstPlayerController());

			if (PlayerController != nullptr)
			{
				AGE_II_P2Character* Character = Cast<AGE_II_P2Character>(PlayerController->GetCharacter());

				if (Character != nullptr)
				{
					if(OtherActor == Character)
					{
						FVector SavedVelocity = Character->GetCharacterMovement()->Velocity;
						
						FHitResult HitResult;
						
						//TODO: add offset to the location player gets teleported too to avoid getting stuck
						//FVector TargetLocation = UMyMathLibrary::ConvertLocation(Character->GetActorLocation(), this, OtherPortal);
						FVector TargetLocation = OtherPortal->GetActorLocation() + (100.f * OtherPortal->GetActorForwardVector());
						FRotator TargetRotation = UMyMathLibrary::ConvertRotator(Character->GetActorRotation(), this, OtherPortal);
		
						Character->SetActorLocationAndRotation(TargetLocation, TargetRotation, false, &HitResult, ETeleportType::TeleportPhysics);
						UE_LOG(LogTemp, Warning, TEXT("Teleported"));

						PlayerController->SetControlRotation(UMyMathLibrary::ConvertRotator(PlayerController->GetControlRotation(), this, OtherPortal));

						FVector NewVelocity =
							FVector::DotProduct(SavedVelocity, GetActorForwardVector()) * OtherPortal->GetActorForwardVector() +
								FVector::DotProduct(SavedVelocity, GetActorRightVector()) * OtherPortal->GetActorRightVector() +
									FVector::DotProduct(SavedVelocity, GetActorUpVector()) * OtherPortal->GetActorUpVector();

						Character->GetCharacterMovement()->Velocity = - NewVelocity;

						bCanEnterPortal = false;

						OtherPortal->bCanEnterPortal = false;
					}
				}
			}
		}
	}
}

void APortal::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	bCanEnterPortal = true;
	if (OtherPortal != nullptr)
	{
		OtherPortal->bCanEnterPortal = true;
	}
}

void APortal::SetIsBluePortal(const bool bValue)
{
	IsBluePortal = bValue;

	SetPortalMaterial();
}

void APortal::SetPortalMaterial() const
{
	if (IsBluePortal)
	{
		PortalBorderMesh->SetMaterial(0, BlueMaterial);
		
		if (OtherPortal != nullptr)
		{
			PortalScreenMesh->SetMaterial(0, PortalVisionMaterial_1);
			OtherPortal->PortalScreenMesh->SetMaterial(0, PortalVisionMaterial_2);
			SceneCapture->TextureTarget = PortalVision_2;
		}
		else
		{
			PortalScreenMesh->SetMaterial(0, BlueMaterial);
		}
	}
	else
	{
		PortalBorderMesh->SetMaterial(0, OrangeMaterial);

		if (OtherPortal != nullptr)
		{
			PortalScreenMesh->SetMaterial(0, PortalVisionMaterial_2);
			OtherPortal->PortalScreenMesh->SetMaterial(0, PortalVisionMaterial_1);
			SceneCapture->TextureTarget = PortalVision_1;
		}
		else
		{
			PortalScreenMesh->SetMaterial(0, OrangeMaterial);
		}
	}
}



