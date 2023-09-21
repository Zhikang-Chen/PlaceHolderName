// Copyright 2022 Alexander Floden, Alias Alex River. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "VertexPaintDetectionStructs.h"
#include "VertexPaintColorSnippetRefs.generated.h"

/**
 *
 */
UCLASS(Blueprintable, BlueprintType)
class VERTEXPAINTDETECTIONPLUGIN_API UVertexPaintColorSnippetRefs : public UDataAsset {

	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "Runtime Vertex Color Paint and Detection Plugin")
		TSoftObjectPtr<UObject> GetObjectFromSnippetID(FString snippetID);

	UFUNCTION(BlueprintCallable, Category = "Runtime Vertex Color Paint and Detection Plugin")
		TMap<FString, TSoftObjectPtr<UVertexPaintColorSnippetDataAsset>> GetAllColorSnippetsAndDataAssetForObject(UObject* object);

	UFUNCTION(BlueprintCallable, Category = "Runtime Vertex Color Paint and Detection Plugin")
	bool ContainsColorSnippet(FString snippetID);

	void RemoveSnippetObject(UObject* object);

	void RemoveColorSnippet(FString snippetID);



	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin")
		TMap<TSoftObjectPtr<UObject>, FVertexDetectColorSnippetReferenceDataStruct> staticMeshesColorSnippets;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin")
		TMap<TSoftObjectPtr<UObject>, FVertexDetectColorSnippetReferenceDataStruct> skeletalMeshesColorSnippets;

	// You can add new tags even if visible anywhere so we skip that for this
	UPROPERTY(/*VisibleAnywhere*/)
		FGameplayTagContainer allAvailableColorSnippets;
};
