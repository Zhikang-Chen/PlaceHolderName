// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CPP_DebugUtilities.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class ZOMGAME_API UCPP_DebugUtilities : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Utilities|String", meta = (WorldContext = "WorldContextObject"))
		static void ClearOnScreenDebugMessages(const UObject* WorldContextObject);

	// This exist so I can just print the text without care for all the other input
	UFUNCTION(Category = "Utilities|String", meta = (WorldContext = "WorldContextObject"))
		static void PrintDebugMessage(const UObject* WorldContextObject, FString text, int32 key = -1, float timeToDisplay = 0.2f, FColor colour = FColor::Yellow);

	UFUNCTION(Category = "Utilities|Editor", BlueprintCallable,BlueprintPure, meta = (WorldContext = "WorldContextObject"))
		static bool IsInEditor(const UObject* WorldContextObject);

	UFUNCTION(Category = "Utilities|Editor", BlueprintCallable, BlueprintPure, meta = (WorldContext = "WorldContextObject"))
		static bool IsInDebugMode(const UObject* WorldContextObject);

	UFUNCTION(Category = "Utilities|Editor", BlueprintCallable, BlueprintPure, meta = (WorldContext = "WorldContextObject"))
		static bool IsInDevelopmentMode(const UObject* WorldContextObject);
};
