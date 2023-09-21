// Copyright 2022 Alexander Floden, Alias Alex River. All Rights Reserved.


#include "SetChaosVehicleFrictionMultiplier.h"


#define LOCTEXT_NAMESPACE "RuntimeVertexPaintAndDetection"

//-------------------------------------------------------

// Construct - Should be overriden by children

USetChaosVehicleFrictionMultiplier::USetChaosVehicleFrictionMultiplier() {

	FunctionReference.SetFromField<UFunction>(UVertexPaintFunctionLibrary::StaticClass()->FindFunctionByName(GET_FUNCTION_NAME_CHECKED(UVertexPaintFunctionLibrary, SetChaosWheeledVehicleFrictionMultiplier_Wrapper)), true);
}


//-------------------------------------------------------

// Expand Node - Should be overriden by children with their specific pin checks

void USetChaosVehicleFrictionMultiplier::ExpandNode(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) {

	Super::ExpandNode(CompilerContext, SourceGraph);

	if (FindPinChecked(TEXT("chaosWheeledVehicleMovementComponent"))->LinkedTo.Num() == 0) {

		CompilerContext.MessageLog.Error(*LOCTEXT("chaosWheeledVehicleMovementComponent Missing Input!", "chaosWheeledVehicleMovementComponent Missing Input!").ToString());

		BreakAllNodeLinks();
		return;
	}
}


//-------------------------------------------------------

// Get Menu Category

FText USetChaosVehicleFrictionMultiplier::GetMenuCategory() const {

	return FText::FromString("Runtime Vertex Color Paint and Detection Plugin|Vehicles");
}


//-------------------------------------------------------

// Get Node Title - Should be overriden by children so they can set different title

FText USetChaosVehicleFrictionMultiplier::GetNodeTitle(ENodeTitleType::Type TitleType) const {

	return LOCTEXT("Set Chaos Wheeled Vehicle Friction Multiplier", "Set Chaos Wheeled Vehicle Friction Multiplier");
}

#undef LOCTEXT_NAMESPACE