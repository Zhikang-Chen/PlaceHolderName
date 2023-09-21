// Copyright 2022 Alexander Floden, Alias Alex River. All Rights Reserved. 

#include "VertexPaint_UEToolbarButtonCommands.h"

#define LOCTEXT_NAMESPACE "FVertexPaint_UEToolbarButtonModule"

void FVertexPaint_UEToolbarButtonCommands::RegisterCommands()
{
#if ENGINE_MAJOR_VERSION == 4

	UI_COMMAND(PluginAction, "Runtime Vertex Color \nPaint & Detection", "Open/Closes the Runtime Vertex Color Paint & Detection Plugin Editor Widget", EUserInterfaceActionType::Button, FInputGesture());

#elif ENGINE_MAJOR_VERSION == 5

	UI_COMMAND(PluginAction, "Runtime Vertex Color \nPaint & Detection", "Open/Closes the Runtime Vertex Color Paint & Detection Plugin Editor Widget", EUserInterfaceActionType::Button, FInputChord());

#endif
}

#undef LOCTEXT_NAMESPACE
