// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MyMathLibrary.generated.h"

/**
 * 
 */
UCLASS()
class GE_II_P2_API UMyMathLibrary : public UObject
{
	GENERATED_BODY()

public:
	/*static FVector ConvertLocation(FVector Location, AActor* Portal, AActor* Target);*/

	static FRotator ConvertRotator(FRotator Rotation, AActor* Portal, AActor* Target);

	bool CheckIsInFront(FVector Point, FVector PortalLocation, FVector PortalNormal);

	/*bool CheckIsCrossing(FVector Point, FVector PortaLocation, FVector PortalNormal,
		bool out_LastInFront, FVector out_LastPosition);*/

	// this is only for blueprint calling and testing, not needed in final version
	UFUNCTION(BlueprintCallable, Category = "Portal")
	void ResizeRender(UTextureRenderTarget2D* rendertarget, float sizeX, float sizeY);
};
