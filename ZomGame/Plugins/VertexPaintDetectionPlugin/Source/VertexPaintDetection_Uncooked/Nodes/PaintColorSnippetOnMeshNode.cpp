// Copyright 2022 Alexander Floden, Alias Alex River. All Rights Reserved.


#include "PaintColorSnippetOnMeshNode.h"


#define LOCTEXT_NAMESPACE "RuntimeVertexPaintAndDetection"

//-------------------------------------------------------

// Construct - Should be overriden by children

UPaintColorSnippetOnMeshNode::UPaintColorSnippetOnMeshNode() {

	FunctionReference.SetFromField<UFunction>(UVertexPaintFunctionLibrary::StaticClass()->FindFunctionByName(GET_FUNCTION_NAME_CHECKED(UVertexPaintFunctionLibrary, PaintColorSnippetOnMesh_Wrappers)), true);
}


//-------------------------------------------------------

// Get Menu Category

FText UPaintColorSnippetOnMeshNode::GetMenuCategory() const {

	return FText::FromString("Runtime Vertex Color Paint and Detection Plugin|Paint");
}


//-------------------------------------------------------

// Get Node Title - Should be overriden by children so they can set different title

FText UPaintColorSnippetOnMeshNode::GetNodeTitle(ENodeTitleType::Type TitleType) const {

	return LOCTEXT("Paint Color Snippet on Mesh", "Paint Color Snippet on Mesh");
}

#undef LOCTEXT_NAMESPACE