// Copyright 2022 Alexander Floden, Alias Alex River. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "VertexPaintDetectionUpdatConfig.generated.h"


UCLASS(config = EditorPerProjectUserSettings)
class VERTEXPAINTDETECTIONPLUGIN_EDITOR_API UVertexPaintDetectionUpdatConfig : public UDeveloperSettings {

	GENERATED_BODY()

public:

	UPROPERTY(config)
		FString PluginVersionUpdate = "";
};
