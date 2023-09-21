// Copyright 2022 Alexander Floden, Alias Alex River. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "VertexPaintDetectionSettings.generated.h"

class UVertexPaintOptimizationDataAsset;
class UVertexPaintMaterialDataAsset;
class UVertexPaintColorSnippetRefs;


/**
 *
 */
UCLASS(config = Game, defaultconfig, meta = (DisplayName = "Runtime Vertex Color Paint and Detection Plugin Settings"))
class VERTEXPAINTDETECTIONPLUGIN_API UVertexPaintDetectionSettings : public UDeveloperSettings {

	GENERATED_BODY()

public:

	UVertexPaintDetectionSettings();

	UPROPERTY(config, VisibleAnywhere, BlueprintReadOnly, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "Which Optimization Data Asset we should use when painting. Meshes that are registered in it are only painted on those amounts of LODs which can save alot of performance. \nThis can be changed in the Editor Widget. "))
		TSoftObjectPtr<UVertexPaintOptimizationDataAsset> vertexPaintAndDetectionPlugin_OptimizationDataAssetToUse = nullptr;

	UPROPERTY(config, VisibleAnywhere, BlueprintReadOnly, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "Which Vertex Paint Material Data Asset to use. This is the data asset you register your materials that use vertex colors that you want to detect what is on what channel etc. \nIf there is none set then you can't detect vertex data, you can only paint. \nThis can be changed in the Editor Widget. "))
		TSoftObjectPtr<UVertexPaintMaterialDataAsset> vertexPaintAndDetectionPlugin_MaterialsDataAssetToUse = nullptr;

	UPROPERTY(config, VisibleAnywhere, BlueprintReadOnly, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "Which Data Asset to store references to Color Snippet Data Assets. Used so we can more optimally get which color snippet data asset to bring into memory so we don't have to go through them all in order to find a stored snippet. \nThis can be changed in the Editor Widget. "))
		TSoftObjectPtr<UVertexPaintColorSnippetRefs> vertexPaintAndDetectionPlugin_ColorSnippetReferencesDataAssetToUse = nullptr;

	UPROPERTY(config, EditDefaultsOnly, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "If we should Show Notifications when making changes in the Editor Widget. "))
		bool vertexPaintAndDetectionPlugin_EditorWidgetNotificationEnabled = true;

	UPROPERTY(config, EditDefaultsOnly, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "Duration of the Notifications that pop up when making certain changes in  the editor widget. "))
		float vertexPaintAndDetectionPlugin_EditorWidgetNotificationDuration = 3;

	UPROPERTY(config, EditDefaultsOnly, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "This is the Max Amount of Tasks a Mesh can have, any new Tasks will Declined. Don't recommend changing this too much since if the Task Queue grows to large, performance will be affected since the TMaps become much more heavier when you run .Add, .FindRef etc. For instance at 1000 Tasks you can drop to 20FPS. \nRecommend making sure you don't add more Tasks than you need, for instance adding the new task when the current task is finished instead of every frame. If the queue grows too large, paint jobs will feel unresponsive as well since it may take a while before it reaches the latest added task. "))
		int vertexPaintAndDetectionPlugin_MaxAmountOfAllowedTasksPerMesh = 50;

private:

#if WITH_EDITOR

	void VertexPaintDetectionSettingsChanged(UObject* Settings, FPropertyChangedEvent& PropertyChangedEvent);

#endif
};
