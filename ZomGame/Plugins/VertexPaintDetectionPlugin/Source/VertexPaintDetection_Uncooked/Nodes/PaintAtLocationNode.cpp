// Copyright 2022 Alexander Floden, Alias Alex River. All Rights Reserved.


#include "PaintAtLocationNode.h"

#define LOCTEXT_NAMESPACE "RuntimeVertexPaintAndDetection"

//-------------------------------------------------------

// Construct - Should be overriden by children

UPaintAtLocationNode::UPaintAtLocationNode() {

	FunctionReference.SetFromField<UFunction>(UVertexPaintFunctionLibrary::StaticClass()->FindFunctionByName(GET_FUNCTION_NAME_CHECKED(UVertexPaintFunctionLibrary, PaintOnMeshAtLocation_Wrapper)), true);
}


//-------------------------------------------------------

// Get Menu Category

FText UPaintAtLocationNode::GetMenuCategory() const {

	return FText::FromString("Runtime Vertex Color Paint and Detection Plugin|Paint");
}


//-------------------------------------------------------

// Get Node Title - Should be overriden by children so they can set different title

FText UPaintAtLocationNode::GetNodeTitle(ENodeTitleType::Type TitleType) const {

	return LOCTEXT("Paint on Mesh at Location", "Paint on Mesh at Location");
}

#undef LOCTEXT_NAMESPACE