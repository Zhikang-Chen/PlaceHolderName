// Copyright 2022 Alexander Floden, Alias Alex River. All Rights Reserved.


#include "GetColorsToApplyFromPhysMat.h"



#define LOCTEXT_NAMESPACE "RuntimeVertexPaintAndDetection"

//-------------------------------------------------------

// Construct - Should be overriden by children

UGetColorsToApplyFromPhysMat::UGetColorsToApplyFromPhysMat() {

	FunctionReference.SetFromField<UFunction>(UVertexPaintFunctionLibrary::StaticClass()->FindFunctionByName(GET_FUNCTION_NAME_CHECKED(UVertexPaintFunctionLibrary, GetColorsToApplyFromPhysicsSurface_Wrapper)), true);
}


//-------------------------------------------------------

// Expand Node - Should be overriden by children with their specific pin checks

void UGetColorsToApplyFromPhysMat::ExpandNode(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) {

	Super::ExpandNode(CompilerContext, SourceGraph);


	if (FindPinChecked(TEXT("materialToApplyColorsTo"))->LinkedTo.Num() == 0) {

		CompilerContext.MessageLog.Error(*LOCTEXT("material Missing Input!", "Must set a Valid Material").ToString());

		BreakAllNodeLinks();
		return;
	}
}


//-------------------------------------------------------

// Get Menu Category

FText UGetColorsToApplyFromPhysMat::GetMenuCategory() const {

	return FText::FromString("Runtime Vertex Color Paint and Detection Plugin|Fundamentals");
}


//-------------------------------------------------------

// Get Node Title - Should be overriden by children so they can set different title

FText UGetColorsToApplyFromPhysMat::GetNodeTitle(ENodeTitleType::Type TitleType) const {

	return LOCTEXT("Get Colors To Apply From Physics Surface", "Get Colors To Apply From Physics Surface");
}

#undef LOCTEXT_NAMESPACE