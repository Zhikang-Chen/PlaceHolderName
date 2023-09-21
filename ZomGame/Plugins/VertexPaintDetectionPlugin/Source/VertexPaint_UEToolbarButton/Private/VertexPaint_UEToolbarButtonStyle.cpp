// Copyright 2022 Alexander Floden, Alias Alex River. All Rights Reserved. 


#if ENGINE_MAJOR_VERSION == 4


#include "VertexPaint_UEToolbarButtonStyle.h"
#include "Framework/Application/SlateApplication.h"
#include "Styling/SlateStyleRegistry.h"
#include "Slate/SlateGameResources.h"
#include "Interfaces/IPluginManager.h"

TSharedPtr< FSlateStyleSet > FVertexPaint_UEToolbarButtonStyle::StyleInstance = NULL;

void FVertexPaint_UEToolbarButtonStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FVertexPaint_UEToolbarButtonStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

FName FVertexPaint_UEToolbarButtonStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("VertexPaint_UEToolbarButtonStyle"));
	return StyleSetName;
}

#define IMAGE_BRUSH( RelativePath, ... ) FSlateImageBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define BOX_BRUSH( RelativePath, ... ) FSlateBoxBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define BORDER_BRUSH( RelativePath, ... ) FSlateBorderBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define TTF_FONT( RelativePath, ... ) FSlateFontInfo( Style->RootToContentDir( RelativePath, TEXT(".ttf") ), __VA_ARGS__ )
#define OTF_FONT( RelativePath, ... ) FSlateFontInfo( Style->RootToContentDir( RelativePath, TEXT(".otf") ), __VA_ARGS__ )

const FVector2D Icon16x16(16.0f, 16.0f);
const FVector2D Icon20x20(20.0f, 20.0f);
const FVector2D Icon40x40(40.0f, 40.0f);

TSharedRef< FSlateStyleSet > FVertexPaint_UEToolbarButtonStyle::Create()
{
	TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet("VertexPaint_UEToolbarButtonStyle"));
	Style->SetContentRoot(IPluginManager::Get().FindPlugin("VertexPaintDetectionPlugin")->GetBaseDir() / TEXT("Resources"));

	Style->Set("VertexPaint_UEToolbarButton.PluginAction", new IMAGE_BRUSH(TEXT("VertexPaintPluginIcon"), Icon40x40));

	return Style;
}

#undef IMAGE_BRUSH
#undef BOX_BRUSH
#undef BORDER_BRUSH
#undef TTF_FONT
#undef OTF_FONT

void FVertexPaint_UEToolbarButtonStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

const ISlateStyle& FVertexPaint_UEToolbarButtonStyle::Get()
{
	return *StyleInstance;
}



#elif ENGINE_MAJOR_VERSION == 5


#include "VertexPaint_UEToolbarButtonStyle.h"
#include "VertexPaint_UEToolbarButton.h"
#include "Framework/Application/SlateApplication.h"
#include "Styling/SlateStyleRegistry.h"
#include "Slate/SlateGameResources.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleMacros.h"

#define RootToContentDir Style->RootToContentDir

TSharedPtr<FSlateStyleSet> FVertexPaint_UEToolbarButtonStyle::StyleInstance = nullptr;

void FVertexPaint_UEToolbarButtonStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FVertexPaint_UEToolbarButtonStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

FName FVertexPaint_UEToolbarButtonStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("VertexPaint_UEToolbarButtonStyle"));
	return StyleSetName;
}


const FVector2D Icon16x16(16.0f, 16.0f);
const FVector2D Icon20x20(20.0f, 20.0f);

TSharedRef< FSlateStyleSet > FVertexPaint_UEToolbarButtonStyle::Create()
{
	TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet("VertexPaint_UEToolbarButtonStyle"));
	Style->SetContentRoot(IPluginManager::Get().FindPlugin("VertexPaintDetectionPlugin")->GetBaseDir() / TEXT("Resources"));

	Style->Set("VertexPaint_UEToolbarButton.PluginAction", new IMAGE_BRUSH(TEXT("VertexPaintPluginIcon"), Icon20x20));
	return Style;
}

void FVertexPaint_UEToolbarButtonStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

const ISlateStyle& FVertexPaint_UEToolbarButtonStyle::Get()
{
	return *StyleInstance;
}


#endif