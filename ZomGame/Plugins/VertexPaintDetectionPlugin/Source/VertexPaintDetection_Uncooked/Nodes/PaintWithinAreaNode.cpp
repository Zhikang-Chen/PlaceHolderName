// Copyright 2022 Alexander Floden, Alias Alex River. All Rights Reserved.


#include "PaintWithinAreaNode.h"


#define LOCTEXT_NAMESPACE "RuntimeVertexPaintAndDetection"


//-------------------------------------------------------

// Construct - Should be overriden by children

UPaintWithinAreaNode::UPaintWithinAreaNode() {

	FunctionReference.SetFromField<UFunction>(UVertexPaintFunctionLibrary::StaticClass()->FindFunctionByName(GET_FUNCTION_NAME_CHECKED(UVertexPaintFunctionLibrary, PaintOnMeshWithinArea_Wrapper)), true);
}


//-------------------------------------------------------

// Expand Node - Should be overriden by children with their specific pin checks

void UPaintWithinAreaNode::ExpandNode(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) {

	Super::ExpandNode(CompilerContext, SourceGraph);


	if (FindPinChecked(TEXT("componentsToCheckIfIsWithin"))->LinkedTo.Num() == 0) {

		CompilerContext.MessageLog.Error(*LOCTEXT("componentsToCheckIfIsWithin Missing Input!", "Paint on Mesh Within Are must have valid Box/Sphere Components, or Static/Skeletal Mesh Components assigned to it.").ToString());

		BreakAllNodeLinks();
		return;
	}
}


//-------------------------------------------------------

// Get Menu Category

FText UPaintWithinAreaNode::GetMenuCategory() const {

	return FText::FromString("Runtime Vertex Color Paint and Detection Plugin|Paint");
}


//-------------------------------------------------------

// Get Node Title - Should be overriden by children so they can set different title

FText UPaintWithinAreaNode::GetNodeTitle(ENodeTitleType::Type TitleType) const {

	return LOCTEXT("Paint on Mesh Within Area", "Paint on Mesh Within Area");
}

#undef LOCTEXT_NAMESPACE