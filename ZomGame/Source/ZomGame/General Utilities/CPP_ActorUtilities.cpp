// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_ActorUtilities.h"
#include "Math/NumericLimits.h"
#include "Kismet/KismetMathLibrary.h"

AActor* UCPP_ActorUtilities::GetClosestActorFromArray(const UObject* WorldContextObject, AActor* TargetActor, TArray<AActor*> Array)
{
	AActor* ClosestActor = nullptr;
	FVector TargetLocation = TargetActor->GetActorLocation();

	if (Array.IsEmpty())
		return ClosestActor;

	float CurrentActorDistance = TNumericLimits<float>::Max();
	for (int i = 0; i < Array.Num(); ++i)
	{
		if (Array[i] != nullptr)
		{
			float distance = FVector::Distance(Array[i]->GetActorLocation(), TargetLocation);
			if (distance < CurrentActorDistance)
			{ 
				ClosestActor = Array[i];
				CurrentActorDistance = distance;
			}
		}
	}
	return ClosestActor;
}

AActor* UCPP_ActorUtilities::GetFarthestActorFromArray(const UObject* WorldContextObject, AActor* TargetActor, TArray<AActor*> Array)
{
	AActor* FarthestActor = nullptr;
	FVector TargetLocation = TargetActor->GetActorLocation();

	if (Array.IsEmpty())
		return FarthestActor;

	float CurrentActorDistance = 0.0f;
	for (int i = 0; i < Array.Num(); ++i)
	{
		if (Array[i] != nullptr)
		{
			float distance = FVector::Distance(Array[i]->GetActorLocation(), TargetLocation);
			if (distance > CurrentActorDistance)
			{
				FarthestActor = Array[i];
				CurrentActorDistance = distance;
			}
		}
	}
	return FarthestActor;
}

void UCPP_ActorUtilities::RotateActorTo(const UObject* WorldContextObject, AActor* Actor, FVector TargetLocation)
{
	FVector Forward = TargetLocation - Actor->GetActorLocation();
	FRotator NewRotation = UKismetMathLibrary::MakeRotFromXZ(Forward, FVector::UpVector);
	Actor->SetActorRotation(NewRotation);
}

void UCPP_ActorUtilities::RotateActorComponentTo(const UObject* WorldContextObject, USceneComponent* ActorComponent, FVector TargetLocation)
{
	FVector Forward = TargetLocation - ActorComponent->GetComponentLocation();
	FRotator NewRotation = UKismetMathLibrary::MakeRotFromXZ(Forward, FVector::UpVector);
	ActorComponent->SetWorldRotation(NewRotation);
}

void UCPP_ActorUtilities::RotateActorComponentYawTo(const UObject* WorldContextObject, USceneComponent* ActorComponent, FVector TargetLocation)
{
	FVector Forward = TargetLocation - ActorComponent->GetComponentLocation();
	FRotator NewRotation = UKismetMathLibrary::MakeRotFromXZ(Forward, FVector::UpVector);
	auto actorRot = ActorComponent->GetComponentRotation();
	FRotator rot = FRotator(actorRot.Pitch, NewRotation.Yaw, actorRot.Roll);
	ActorComponent->SetWorldRotation(rot);
}

void UCPP_ActorUtilities::RotateActorComponentPitchTo(const UObject* WorldContextObject, USceneComponent* ActorComponent, FVector TargetLocation)
{
	FVector Forward = TargetLocation - ActorComponent->GetComponentLocation();
	FRotator NewRotation = UKismetMathLibrary::MakeRotFromXZ(Forward, FVector::UpVector);
	auto actorRot = ActorComponent->GetComponentRotation();
	FRotator rot = FRotator(NewRotation.Pitch, actorRot.Yaw, actorRot.Roll);
	ActorComponent->SetWorldRotation(rot);
}




FVector UCPP_ActorUtilities::GetRandomPointInSphere(FVector Center, float Radius)
{
	FVector ret;
	auto randomRad = FMath::RandRange(0.0f, Radius);
	auto randomVector = FMath::VRand();
	ret = Center + (randomVector * randomRad);
	return ret;
}

FVector UCPP_ActorUtilities::GetRandomPointInOutterSphere(FVector Center, float Radius)
{
	FVector ret;
	auto randomVector = FMath::VRand();
	ret = Center + (randomVector * Radius);
	return ret;
}

