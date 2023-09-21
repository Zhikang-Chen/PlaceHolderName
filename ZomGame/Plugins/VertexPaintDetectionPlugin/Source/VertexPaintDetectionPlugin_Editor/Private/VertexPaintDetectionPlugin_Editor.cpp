// Copyright 2022 Alexander Floden, Alias Alex River. All Rights Reserved. 

#include "VertexPaintDetectionPlugin_Editor.h"
#include "VertexPaintDetectionUpdatePopUp.h"

#define LOCTEXT_NAMESPACE "FVertexPaintDetectionPlugin_EditorModule"

void FVertexPaintDetectionPlugin_EditorModule::StartupModule()
{
	UE_LOG(LogTemp, Log, TEXT("Runtime Vertex Color Paint and Detection Plugin - Editor Module Startup!"));

	VertexPaintDetectionUpdatePopUp::Register();
}

void FVertexPaintDetectionPlugin_EditorModule::ShutdownModule()
{
	UE_LOG(LogTemp, Log, TEXT("Runtime Vertex Color Paint and Detection Plugin - Editor Module Shutdown!"));
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FVertexPaintDetectionPlugin_EditorModule, VertexPaintDetectionPlugin_Editor)