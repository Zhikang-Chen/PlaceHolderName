// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_DebugUtilities.h"

void UCPP_DebugUtilities::ClearOnScreenDebugMessages(const UObject* WorldContextObject)
{
	GEngine->ClearOnScreenDebugMessages();
}

void UCPP_DebugUtilities::PrintDebugMessage(const UObject* WorldContextObject, FString text, int32 key, float timeToDisplay, FColor colour)
{
	GEngine->AddOnScreenDebugMessage(key, timeToDisplay, colour, text);
}

bool UCPP_DebugUtilities::IsInEditor(const UObject* WorldContextObject)
{
#if WITH_EDITOR
	return true;
#endif
	return false;
}

bool UCPP_DebugUtilities::IsInDebugMode(const UObject* WorldContextObject)
{
#if UE_BUILD_DEBUG
	return true;
#endif
	return false;
}

bool UCPP_DebugUtilities::IsInDevelopmentMode(const UObject* WorldContextObject)
{
#if UE_BUILD_DEVELOPMENT
	return true;
#endif
	return false;
}