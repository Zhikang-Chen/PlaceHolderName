// Copyright 2022 Alexander Floden, Alias Alex River. All Rights Reserved.


#include "ClearPaintsSinceSessionStartNode.h"


#define LOCTEXT_NAMESPACE "RuntimeVertexPaintAndDetection"

//-------------------------------------------------------

// Construct - Should be overriden by children

UClearPaintsSinceSessionStartNode::UClearPaintsSinceSessionStartNode() {

	FunctionReference.SetFromField<UFunction>(UVertexPaintFunctionLibrary::StaticClass()->FindFunctionByName(GET_FUNCTION_NAME_CHECKED(UVertexPaintFunctionLibrary, ClearMeshPaintedSinceSessionStarted_Wrapper)), true);
}


//-------------------------------------------------------

// Expand Node - Should be overriden by children with their specific pin checks

void UClearPaintsSinceSessionStartNode::ExpandNode(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) {

	Super::ExpandNode(CompilerContext, SourceGraph);

	if (FindPinChecked(TEXT("WorldContextObject"))->LinkedTo.Num() == 0) {

		CompilerContext.MessageLog.Error(*LOCTEXT("WorldContextObject Missing Input!", "WorldContextObject Missing Input!").ToString());

		BreakAllNodeLinks();
		return;
	}
}


//-------------------------------------------------------

// Get Menu Category

FText UClearPaintsSinceSessionStartNode::GetMenuCategory() const {

	return FText::FromString("Runtime Vertex Color Paint and Detection Plugin|Utilities");
}



//-------------------------------------------------------

// Get Node Title - Should be overriden by children so they can set different title

FText UClearPaintsSinceSessionStartNode::GetNodeTitle(ENodeTitleType::Type TitleType) const {

	return LOCTEXT("Clear Meshes Painted Since Session Start", "Clear Meshes Painted Since Session Start");
}

#undef LOCTEXT_NAMESPACE