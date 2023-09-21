// Copyright 2022 Alexander Floden, Alias Alex River. All Rights Reserved.


#include "GetMeshPaintedSinceSessionStart.h"


#define LOCTEXT_NAMESPACE "RuntimeVertexPaintAndDetection"

//-------------------------------------------------------

// Construct - Should be overriden by children

UGetMeshPaintedSinceSessionStart::UGetMeshPaintedSinceSessionStart() {

	FunctionReference.SetFromField<UFunction>(UVertexPaintFunctionLibrary::StaticClass()->FindFunctionByName(GET_FUNCTION_NAME_CHECKED(UVertexPaintFunctionLibrary, GetMeshPaintedSinceSessionStarted_Wrapper)), true);
}


//-------------------------------------------------------

// Get Menu Category

FText UGetMeshPaintedSinceSessionStart::GetMenuCategory() const {

	return FText::FromString("Runtime Vertex Color Paint and Detection Plugin|Utilities");
}


//-------------------------------------------------------

// Get Node Title - Should be overriden by children so they can set different title

FText UGetMeshPaintedSinceSessionStart::GetNodeTitle(ENodeTitleType::Type TitleType) const {

	return LOCTEXT("Get Mesh Painted Since Session Start", "Get Mesh Painted Since Session Start");
}

#undef LOCTEXT_NAMESPACE