// Copyright 2023 Alexander Floden, Alias Alex River. All Rights Reserved. 

#include "RapidJsonModule.h"

#define LOCTEXT_NAMESPACE "FRapidJsonModule"

void FRapidJsonModule::StartupModule()
{
    // This code will execute after your module is loaded into memory

    UE_LOG(LogTemp, Log, TEXT("Rapid Json Module - Runtime Module Startup!"));
}

void FRapidJsonModule::ShutdownModule()
{
    // This function may be called during shutdown to clean up your module


    UE_LOG(LogTemp, Log, TEXT("Rapid Json Module - Runtime Module Shutdown!"));
}


#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FRapidJsonModule, RapidJsonModule)