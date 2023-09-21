// Copyright 2022 Alexander Floden, Alias Alex River. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "VertexPaintFunctionWrapper.h"
#include "GetPhysMatUsingPhysicsSurface.generated.h"

/**
 * 
 */
UCLASS()
class VERTEXPAINTDETECTION_UNCOOKED_API UGetPhysMatUsingPhysicsSurface : public UVertexPaintFunctionWrapper
{
	GENERATED_BODY()
	
public:

	UGetPhysMatUsingPhysicsSurface();

	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;

	virtual FText GetMenuCategory() const override;
};
