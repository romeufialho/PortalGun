// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponSpawner.h"

#include "GE_II_P2Character.h"
#include "MyPlayerController.h"

// Sets default values
AWeaponSpawner::AWeaponSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	SetRootComponent(RootComponent);

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SpawnerMesh"));
	StaticMesh->SetupAttachment(RootComponent);

	WeaponLocation = CreateDefaultSubobject<USceneComponent>(TEXT("WeaponActor"));
	WeaponLocation->SetupAttachment(RootComponent);

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	BoxComponent->SetupAttachment(WeaponLocation);
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AWeaponSpawner::OnOverlapBegin);
	BoxComponent->OnComponentEndOverlap.AddDynamic(this, &AWeaponSpawner::OnOverlapEnd);
	BoxComponent->SetGenerateOverlapEvents(true);

}

// Called when the game starts or when spawned
void AWeaponSpawner::BeginPlay()
{
	Super::BeginPlay();
	
	SpawnWeapon();
}

// Called every frame
void AWeaponSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeaponSpawner::SpawnWeapon()
{
	FVector Location = WeaponLocation->GetComponentLocation();
	FRotator Rotation = GetActorRotation();
	//TODO: this is crashing
	SpawnedWeapon = GetWorld()->SpawnActor<AWeapon>(WeaponToSpawn, Location, Rotation);
}


void AWeaponSpawner::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Overlap Detected"));
	// Begin Collect Verifications
	AMyPlayerController* PlayerController = Cast<AMyPlayerController>(GetWorld()->GetFirstPlayerController());
	if (PlayerController != nullptr)
	{
		AGE_II_P2Character* Character = Cast<AGE_II_P2Character>(PlayerController->GetCharacter());
		if (Character != nullptr)
		{
			if(OtherActor == Character)
			{
				UE_LOG(LogTemp, Warning, TEXT("Overlap With Player Detected"));
				
				if (WeaponLocation->GetVisibleFlag())
				{
					UE_LOG(LogTemp, Warning, TEXT("Weapon Collected"));
					WeaponLocation->SetVisibility(false);
					if(SpawnedWeapon != nullptr)
					{
						SpawnedWeapon->Destroy();
					}
					//TODO: update player's available weapon
					StartTimer();
				}else
					UE_LOG(LogTemp, Warning, TEXT("Weapon On Cooldown"));
			}
		}
	}
}

void AWeaponSpawner::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	
}

void AWeaponSpawner::StartTimer()
{
	GetWorldTimerManager().SetTimer(_loopTimeHandle, this, &AWeaponSpawner::OnTimerEnd, RespawnTime, false);
}

void AWeaponSpawner::OnTimerEnd()
{
	UE_LOG(LogTemp, Warning, TEXT("Weapon Cooldown Over"));
	WeaponLocation->SetVisibility(true);
	SpawnWeapon();
}

