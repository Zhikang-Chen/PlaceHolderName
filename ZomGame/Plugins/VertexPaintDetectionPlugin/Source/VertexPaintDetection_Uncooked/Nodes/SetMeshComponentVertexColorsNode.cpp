// Copyright 2022 Alexander Floden, Alias Alex River. All Rights Reserved.


#include "SetMeshComponentVertexColorsNode.h"

#define LOCTEXT_NAMESPACE "RuntimeVertexPaintAndDetection"


//-------------------------------------------------------

// Construct - Should be overriden by children

USetMeshComponentVertexColorsNode::USetMeshComponentVertexColorsNode() {

	FunctionReference.SetFromField<UFunction>(UVertexPaintFunctionLibrary::StaticClass()->FindFunctionByName(GET_FUNCTION_NAME_CHECKED(UVertexPaintFunctionLibrary, SetMeshComponentVertexColors_Wrapper)), true);
}


//-------------------------------------------------------

// Expand Node - Should be overriden by children with their specific pin checks

void USetMeshComponentVertexColorsNode::ExpandNode(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) {

	Super::ExpandNode(CompilerContext, SourceGraph);


	if (FindPinChecked(TEXT("meshComponent"))->LinkedTo.Num() == 0) {

		CompilerContext.MessageLog.Error(*LOCTEXT("meshComponent Missing Input!", "Set Mesh Component Vertex Colors must have a valid Static or Skeletal Mesh assigned to meshComponent.").ToString());

		BreakAllNodeLinks();
		return;
	}
}


//-------------------------------------------------------

// Get Menu Category

FText USetMeshComponentVertexColorsNode::GetMenuCategory() const {

	return FText::FromString("Runtime Vertex Color Paint and Detection Plugin|Paint");
}


//-------------------------------------------------------

// Get Node Title - Should be overriden by children so they can set different title

FText USetMeshComponentVertexColorsNode::GetNodeTitle(ENodeTitleType::Type TitleType) const {

	return LOCTEXT("Set Mesh Component Vertex Colors", "Set Mesh Component Vertex Colors");
}

#undef LOCTEXT_NAMESPACE