// Copyright 2022 Alexander Floden, Alias Alex River. All Rights Reserved.


#include "PaintOnEntireMeshNode.h"



#define LOCTEXT_NAMESPACE "RuntimeVertexPaintAndDetection"

//-------------------------------------------------------

// Construct - Should be overriden by children

UPaintOnEntireMeshNode::UPaintOnEntireMeshNode() {

	FunctionReference.SetFromField<UFunction>(UVertexPaintFunctionLibrary::StaticClass()->FindFunctionByName(GET_FUNCTION_NAME_CHECKED(UVertexPaintFunctionLibrary, PaintOnEntireMesh_Wrapper)), true);
}


//-------------------------------------------------------

// Get Menu Category

FText UPaintOnEntireMeshNode::GetMenuCategory() const {

	return FText::FromString("Runtime Vertex Color Paint and Detection Plugin|Paint");
}


//-------------------------------------------------------

// Get Node Title - Should be overriden by children so they can set different title

FText UPaintOnEntireMeshNode::GetNodeTitle(ENodeTitleType::Type TitleType) const {

	return LOCTEXT("Paint on Entire Mesh", "Paint on Entire Mesh");
}

#undef LOCTEXT_NAMESPACE