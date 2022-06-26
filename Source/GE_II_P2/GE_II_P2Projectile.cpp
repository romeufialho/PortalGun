// Copyright Epic Games, Inc. All Rights Reserved.

#include "GE_II_P2Projectile.h"

#include "MyPlayerController.h"
#include "Portal.h"
#include "Portal_Manager.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"

AGE_II_P2Projectile::AGE_II_P2Projectile() 
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &AGE_II_P2Projectile::OnHit);		// set up a notification for when this component hits something blocking

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 9000.f;
	ProjectileMovement->MaxSpeed = 9000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;

	// Use a given material from a subobject
	BlueMaterial = CreateDefaultSubobject<UMaterialInstance>(TEXT("Blue Material"));
	OrangeMaterial = CreateDefaultSubobject<UMaterialInstance>(TEXT("Orange Material"));

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	StaticMesh->SetupAttachment(RootComponent);
}

void AGE_II_P2Projectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Define player controller
	AMyPlayerController* PlayerController = Cast<AMyPlayerController>(GetWorld()->GetFirstPlayerController());
	AGE_II_P2Character* MyPlayerCharacter = Cast<AGE_II_P2Character>(PlayerController->GetPawn());

	//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("Hit something")));


	if (MyPlayerCharacter != nullptr) {

		int PlayerBulletType = MyPlayerCharacter->GetPlayerCurrentBulletType();

		if (PlayerBulletType == MyPlayerCharacter->Portal_BulletType){
			// Only add impulse and destroy projectile if we hit a physics
			if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr) /*&& OtherComp->IsSimulatingPhysics()*/)
			{
				//OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());

				// call spawn portal through player controller
				APortal_Manager* PortalManager = PlayerController->GetPortal_Manager();

				if (bIsBlue)
				{
					APortal* PortalCreated = PortalManager->SpawnBluePortal(Hit);
					PortalCreated->SetIsBluePortal(bIsBlue);
				}
				else
				{
					APortal* PortalCreated = PortalManager->SpawnOrangePortal(Hit);
					PortalCreated->SetIsBluePortal(bIsBlue);
				}
			}
		}
		else{

			if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr))
			{
				//check if hit a player
				AGE_II_P2Character* OtherPlayer = Cast<AGE_II_P2Character>(OtherActor);

				if (OtherPlayer == nullptr) {
					GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("OUTRO PLAYER NAO EXISTEEEEEEEEEEEEEEEEEEEEEE")));
				}else
				{
					FDamageEvent DamageEvent;
					OtherPlayer->TakeDamage(10.f, DamageEvent, PlayerController, MyPlayerCharacter);
					GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("DAMAGED PLAYER")));
				}
			}
		}
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("NAO EXISTE PLAYER")));
	}

	Destroy();
}
void AGE_II_P2Projectile::BeginPlay() {
	Super::BeginPlay();

}
void AGE_II_P2Projectile::SetIsBlueProjectile(bool bValue)
{
		bIsBlue = bValue;

		SetProjectileMaterial();
}

void AGE_II_P2Projectile::SetProjectileMaterial()
{
	if (bIsBlue)
	{
		StaticMesh->SetMaterial(0, BlueMaterial);
	}else
	{
		StaticMesh->SetMaterial(0, OrangeMaterial);
	}
}




