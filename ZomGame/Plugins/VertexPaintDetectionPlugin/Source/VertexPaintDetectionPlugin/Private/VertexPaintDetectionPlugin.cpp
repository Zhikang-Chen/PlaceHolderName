// Copyright 2022 Alexander Floden, Alias Alex River. All Rights Reserved. 

#include "VertexPaintDetectionPlugin.h"
#define LOCTEXT_NAMESPACE "FVertexPaintDetectionPluginModule"


//-------------------------------
// Startup Module

void FVertexPaintDetectionPluginModule::StartupModule() {

	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	UE_LOG(LogTemp, Log, TEXT("Runtime Vertex Color Paint and Detection Plugin - Runtime Module Startup!"));
}

//-------------------------------
// Shutdown Module

void FVertexPaintDetectionPluginModule::ShutdownModule() {

	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UE_LOG(LogTemp, Log, TEXT("Runtime Vertex Color Paint and Detection Plugin - Runtime Module Shutdown!"));
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FVertexPaintDetectionPluginModule, VertexPaintDetectionPlugin)