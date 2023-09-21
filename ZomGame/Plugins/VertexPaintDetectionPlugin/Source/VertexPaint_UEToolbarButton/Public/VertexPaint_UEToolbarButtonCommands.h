// Copyright 2022 Alexander Floden, Alias Alex River. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "VertexPaint_UEToolbarButtonStyle.h"

class FVertexPaint_UEToolbarButtonCommands : public TCommands<FVertexPaint_UEToolbarButtonCommands>
{
public:

	FVertexPaint_UEToolbarButtonCommands()
		: TCommands<FVertexPaint_UEToolbarButtonCommands>(TEXT("VertexPaint_UEToolbarButton"), NSLOCTEXT("Contexts", "VertexPaint_UEToolbarButton", "VertexPaint_UEToolbarButton Plugin"), NAME_None, FVertexPaint_UEToolbarButtonStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > PluginAction;
};
