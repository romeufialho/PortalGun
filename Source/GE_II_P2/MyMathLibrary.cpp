// Fill out your copyright notice in the Description page of Project Settings.


#include "MyMathLibrary.h"


FVector UMyMathLibrary::ConvertLocation(FVector Location, AActor* Portal, AActor* Target)
{
	FVector Direction = Location - Portal->GetActorLocation();
	FVector TargetLocation = Target->GetActorLocation();

	FVector Dots;
	Dots.X = FVector::DotProduct(Direction, Portal->GetActorForwardVector());
	Dots.Y = FVector::DotProduct(Direction, Portal->GetActorRightVector());
	Dots.Z = FVector::DotProduct(Direction, Portal->GetActorUpVector());

	FVector NewDirection = Dots.X * -Target->GetActorForwardVector()
		+ Dots.Y * -Target->GetActorRightVector()
		+ Dots.Z * Target->GetActorUpVector();

	return TargetLocation + NewDirection;
}

FRotator UMyMathLibrary::ConvertRotator(FRotator Rotation, AActor* Portal, AActor* Target)
{
	FVector RotationAdjustment(0.f, 0.f, -180.f);
	FVector LocalAdjustment = FVector::ZeroVector;

	/**
	 * Kinda_Smal_Number -> checks if two floats are approximately equal
	 */
	if (FVector::DotProduct(Portal->GetActorForwardVector(), FVector::UpVector) > KINDA_SMALL_NUMBER)
	{
		LocalAdjustment.X = FMath::UnwindDegrees(Portal->GetTransform().GetRotation().Euler().X);
		LocalAdjustment.Y = 180.f;
		RotationAdjustment.Z += LocalAdjustment.X;
	}
	else if (FVector::DotProduct(Portal->GetActorForwardVector(), FVector::UpVector) < -KINDA_SMALL_NUMBER)
	{
		LocalAdjustment.X = FMath::UnwindDegrees(Portal->GetTransform().GetRotation().Euler().X);
		LocalAdjustment.Y = -180.f;
		RotationAdjustment.Z -= LocalAdjustment.X;
	}

	FQuat QuatRotation = FQuat::MakeFromEuler(RotationAdjustment) * FQuat(Rotation);
	FQuat LocalQuat = FQuat::MakeFromEuler(LocalAdjustment) * Portal->GetActorTransform().GetRotation().Inverse() *
		QuatRotation;
	FQuat NewWorldQuat = Target->GetActorTransform().GetRotation() * LocalQuat;
	return NewWorldQuat.Rotator();
}

bool UMyMathLibrary::CheckIsInFront(FVector Point, FVector PortalLocation, FVector PortalNormal)
{
	return false;
}

bool UMyMathLibrary::CheckIsCrossing(FVector Point, FVector PortaLocation, FVector  PortalNormal, bool out_LastInFront,
	FVector out_LastPosition)
{
	return false;
}
