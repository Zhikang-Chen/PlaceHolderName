// Copyright 2022 Alexander Floden, Alias Alex River. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "VertexPaintFunctionWrapper.h"
#include "SetChaosVehicleFrictionMultiplier.generated.h"

/**
 * 
 */
UCLASS()
class VERTEXPAINTDETECTION_UNCOOKED_API USetChaosVehicleFrictionMultiplier : public UVertexPaintFunctionWrapper
{
	GENERATED_BODY()
	
public:

	USetChaosVehicleFrictionMultiplier();

	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;

	virtual void ExpandNode(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;

	virtual FText GetMenuCategory() const override;
};
