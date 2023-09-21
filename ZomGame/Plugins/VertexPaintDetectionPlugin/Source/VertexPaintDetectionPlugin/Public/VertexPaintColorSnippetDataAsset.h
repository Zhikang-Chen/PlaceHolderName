// Copyright 2022 Alexander Floden, Alias Alex River. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "VertexPaintDetectionStructs.h"
#include "VertexPaintColorSnippetDataAsset.generated.h"

class UVertexPaintColorSnippetRefs;

/**
 *
 */
UCLASS(Blueprintable, BlueprintType)
class VERTEXPAINTDETECTIONPLUGIN_API UVertexPaintColorSnippetDataAsset : public UDataAsset {

	GENERATED_BODY()

public:


	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "TMap of an Object Snippet Color Data which can be a Static or Skeletal Mesh. Accesses which snippet with a String identifier. Color Snippets can be used if you for instance want a bunch of zombies with different blood patterns on them, then you can store a bunch of different snippets and randomize which to use when spawning a zombie. "))
		TMap<FString, FVertexDetectColorSnippetDataStruct> snippetColorData;
};
