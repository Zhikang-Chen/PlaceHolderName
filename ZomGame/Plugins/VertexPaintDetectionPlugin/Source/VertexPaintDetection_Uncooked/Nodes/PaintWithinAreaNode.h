// Copyright 2022 Alexander Floden, Alias Alex River. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "VertexPaintDetectionOnMeshNode.h"
#include "PaintWithinAreaNode.generated.h"

/**
 * 
 */
UCLASS()
class VERTEXPAINTDETECTION_UNCOOKED_API UPaintWithinAreaNode : public UVertexPaintDetectionOnMeshNode
{
	GENERATED_BODY()
	
public:

	UPaintWithinAreaNode();

	virtual void ExpandNode(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;

	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;

	virtual FText GetMenuCategory() const override;
};
