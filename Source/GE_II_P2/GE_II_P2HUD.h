// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once 

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "GE_II_P2HUD.generated.h"

UCLASS()
class AGE_II_P2HUD : public AHUD
{
	GENERATED_BODY()

public:
	AGE_II_P2HUD();

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

private:
	/** Crosshair asset pointer */
	class UTexture2D* CrosshairTex;

};

