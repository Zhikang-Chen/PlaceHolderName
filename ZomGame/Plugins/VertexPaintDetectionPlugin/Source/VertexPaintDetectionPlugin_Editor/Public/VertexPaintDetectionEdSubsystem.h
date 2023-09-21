// Copyright 2022 Alexander Floden, Alias Alex River. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "EditorSubsystem.h"
#include "RapidJsonModule.h"
#include "VertexPaintDetectionPlugin.h"
#include "VertexPaintDetectionStructs.h"
#include "VertexPaintDetectionEdSubsystem.generated.h"


class UVertexPaintColorSnippetDataAsset;
class UVertexPaintColorSnippetRefs;
class UVertexPaintOptimizationDataAsset;
class UVertexPaintMaterialDataAsset;
class UButton;
class UComboBoxString;


UCLASS()
class VERTEXPAINTDETECTIONPLUGIN_EDITOR_API UVertexPaintDetectionEdSubsystem : public UEditorSubsystem {

	GENERATED_BODY()

private:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void Deinitialize() override;

public:

	UFUNCTION(BlueprintCallable, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "Adds a Editor Notification in the Bottom Right Corner"))
		void AddEditorNotification(FString notification);

	UFUNCTION(BlueprintCallable, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "Can set Button Settings depending on Engine Version"))
		void SetEngineSpecificButtonSettings(TArray<UButton*> buttons, TArray<UComboBoxString*> comboBoxes);

	UFUNCTION(BlueprintCallable, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "Auto Saves this package so the user doesn't have to manually press save after doing changes"))
		bool SavePackageWrapper(UObject* packageToSave);

	UFUNCTION(BlueprintCallable, Category = "Runtime Vertex Color Paint and Detection Plugin")
		void AddSkeletalMeshAmountOfLODsToPaintByDefault(USkeletalMesh* skeletalMesh, int maxAmountOfLODsToPaint, bool& success);

	UFUNCTION(BlueprintCallable, Category = "Runtime Vertex Color Paint and Detection Plugin")
		void RemoveSkeletalMeshAmountOfLODsToPaintByDefault(USkeletalMesh* skeletalMesh, bool& success);

	UFUNCTION(BlueprintCallable, Category = "Runtime Vertex Color Paint and Detection Plugin")
		void AddStaticMeshAndAmountOfLODsToPaintByDefault(UStaticMesh* staticMesh, int maxAmountOfLODsToPaint, bool& success);

	UFUNCTION(BlueprintCallable, Category = "Runtime Vertex Color Paint and Detection Plugin")
		void RemoveStaticMeshAndAmountOfLODsToPaintByDefault(UStaticMesh* staticMesh, bool& success);

	UFUNCTION(BlueprintCallable, Category = "Runtime Vertex Color Paint and Detection Plugin")
		void AddMeshColorSnippet(UMeshComponent* meshComponent, FString colorSnippetID, UVertexPaintColorSnippetDataAsset* colorSnippetDataAsset, bool& success);

	UFUNCTION(BlueprintCallable, Category = "Runtime Vertex Color Paint and Detection Plugin")
		void MoveMeshColorSnippet(FString colorSnippetID, UVertexPaintColorSnippetDataAsset* colorSnippetDataAssetToMoveFrom, UVertexPaintColorSnippetDataAsset* colorSnippetDataAssetToMoveTo, bool& success);

	UFUNCTION(BlueprintCallable, Category = "Runtime Vertex Color Paint and Detection Plugin")
		void UpdateMeshColorSnippetID(FString prevColorSnippetID, FString newColorSnippetID, UVertexPaintColorSnippetDataAsset* colorSnippetDataAsset, bool& success);

	UFUNCTION(BlueprintCallable, Category = "Runtime Vertex Color Paint and Detection Plugin")
		void RemoveMeshColorSnippet(FString colorSnippetID, UVertexPaintColorSnippetDataAsset* colorSnippetDataAsset);

	UFUNCTION(BlueprintCallable, Category = "Runtime Vertex Color Paint and Detection Plugin")
		bool ApplyVertexColorToMeshAtLOD0(UMeshComponent* mesh, TArray<FColor> vertexColorsAtLOD0);

	UFUNCTION(BlueprintCallable, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "Refreshes the color snippets so it gets stored as a serialized jstrong string which is more optimized and performant, as well as can be used in webservices. "))
		void RefreshStoredColorSnippetFromSerializedStringToTArray(UVertexPaintColorSnippetDataAsset* colorSnippetDataAsset, FString colorSnippet);

	UFUNCTION(BlueprintPure, Category = "Runtime Vertex Color Paint and Detection Plugin")
		int GetStaticMeshVerticesAmount_Wrapper(UStaticMesh* mesh, int lod);

	UFUNCTION(BlueprintPure, Category = "Runtime Vertex Color Paint and Detection Plugin")
		int GetSkeletalMeshVerticesAmount_Wrapper(USkeletalMesh* mesh, int lod);

	UFUNCTION(BlueprintPure, Category = "Runtime Vertex Color Paint and Detection Plugin")
		int GetStaticMeshLODCount_Wrapper(UStaticMesh* mesh);

	UFUNCTION(BlueprintPure, Category = "Runtime Vertex Color Paint and Detection Plugin")
		int GetSkeletalMeshLODCount_Wrapper(USkeletalMesh* skeletalMesh);

	UFUNCTION(BlueprintPure, Category = "Runtime Vertex Color Paint and Detection Plugin")
		int GetSkeletalMeshComponentVertexCount_Wrapper(USkeletalMeshComponent* skeletalMeshComponent, int lod);

	UFUNCTION(BlueprintPure, Category = "Runtime Vertex Color Paint and Detection Plugin")
		int GetStaticMeshComponentVertexCount_Wrapper(UStaticMeshComponent* staticMeshComponent, int lod);

	UFUNCTION(BlueprintPure, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (WorldContext = "WorldContextObject"))
		UWorld* GetPersistentLevelsWorld_Wrapper(const UObject* worldContext);

	UFUNCTION(BlueprintCallable, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "Sets which Optimization Data Asset on the custom settings. Also tells the subsystem to update it's references"))
		void SetCustomSettingsOptimizationsDataAssetToUse(UObject* dataAsset);

	UFUNCTION(BlueprintCallable, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "Sets which vertex paint material to use that stores all materials we've added with vertexcolors. "))
		void SetCustomSettingsVertexPaintMaterialToUse(UObject* dataAsset);

	UFUNCTION(BlueprintCallable, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "Sets which color snippet reference data asset to use that stores references to Color snippet Data Asset. "))
		void SetCustomSettingsVertexPaintColorSnippetReferenceDataAssetToUse(UObject* dataAsset);

	UFUNCTION(BlueprintCallable, Category = "Runtime Vertex Color Paint and Detection Plugin")
		void LoadEssentialDataAssets();

	UFUNCTION(BlueprintCallable, Category = "Runtime Vertex Color Paint and Detection Plugin")
		bool AddColorSnippetTag(FString tag);

	UFUNCTION(BlueprintPure, Category = "Runtime Vertex Color Paint and Detection Plugin")
		bool DoesColorSnippetTagExist(FString tag);

	UFUNCTION(BlueprintCallable, Category = "Runtime Vertex Color Paint and Detection Plugin")
		TArray<FString> GetAllColorSnippetTagsDirectlyFromIni();

	UFUNCTION(BlueprintCallable, Category = "Runtime Vertex Color Paint and Detection Plugin")
		bool RemoveColorSnippetTag(FString tag);

	UFUNCTION(BlueprintCallable, Category = "Runtime Vertex Color Paint and Detection Plugin")
		void ClearAllAvailableColorSnippetCacheTagContainer();

	UFUNCTION(BlueprintCallable, Category = "Runtime Vertex Color Paint and Detection Plugin")
		void RenameColorSnippetTag(FString oldTag, FString newTag);

	UFUNCTION(BlueprintCallable, Category = "Runtime Vertex Color Paint and Detection Plugin")
		void RefreshAllAvailableCachedColorSnippetTagContainer();

private:

	FString colorSnippetDevComments = "Runtime Vertex Color Paint & Detection Plugin";
};
