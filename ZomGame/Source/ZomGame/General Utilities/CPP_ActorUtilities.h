// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CPP_ActorUtilities.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class ZOMGAME_API UCPP_ActorUtilities : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "", meta = (WorldContext = "WorldContextObject"))
		static AActor* GetClosestActorFromArray(const UObject* WorldContextObject, AActor* TargetActor,TArray<AActor*> Array);

	UFUNCTION(BlueprintCallable, Category = "", meta = (WorldContext = "WorldContextObject"))
		static AActor* GetFarthestActorFromArray(const UObject* WorldContextObject, AActor* TargetActor, TArray<AActor*> Array);

	UFUNCTION(BlueprintCallable, Category = "", meta = (WorldContext = "WorldContextObject"))
		static void RotateActorTo(const UObject* WorldContextObject, AActor* Actor, FVector TargetLocation);

	UFUNCTION(BlueprintCallable, Category = "", meta = (WorldContext = "WorldContextObject"))
		static void RotateActorComponentTo(const UObject* WorldContextObject, USceneComponent* ActorComponent, FVector TargetLocation);

		UFUNCTION(BlueprintCallable, Category = "", meta = (WorldContext = "WorldContextObject"))
		static void RotateActorComponentYawTo(const UObject* WorldContextObject, USceneComponent* ActorComponent, FVector TargetLocation);

		UFUNCTION(BlueprintCallable, Category = "", meta = (WorldContext = "WorldContextObject"))
		static void RotateActorComponentPitchTo(const UObject* WorldContextObject, USceneComponent* ActorComponent, FVector TargetLocation);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "")
	static FVector GetRandomPointInSphere(FVector Center, float Radius);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "")
	static FVector GetRandomPointInOutterSphere(FVector Center, float Radius);
};
