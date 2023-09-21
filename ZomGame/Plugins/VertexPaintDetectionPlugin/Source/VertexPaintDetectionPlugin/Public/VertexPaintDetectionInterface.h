// Copyright 2022 Alexander Floden, Alias Alex River. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "VertexPaintDetectionStructs.h"
#include "VertexPaintDetectionInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(Blueprintable, BlueprintType)
class UVertexPaintDetectionInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 *
 */
class VERTEXPAINTDETECTIONPLUGIN_API IVertexPaintDetectionInterface
{
	GENERATED_BODY()

		// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintNativeEvent, Category = "Runtime Vertex Color Paint and Detection Plugin|Detection", meta = (ToolTip = "Runs on the Actor that got Detected on. "))
		void GetClosestVertexDataOnActor(FVertexDetectTaskResultInfo taskResultInfo, const FVertexDetectMeshDataStruct& meshData, FVertexDetectInfoStruct detectedMeshWithSettings, FVertexDetectClosestVertexDataResultStruct closestVertexColorResult, FVertexDetectEstimatedColorAtHitLocationResultStruct estimatedColorAtHitLocationResult, FVertexDetectAvarageColorInAreaInfo avarageColorInAreaInfo, FVertexDetectAmountOfPaintedColorsOfEachChannel amountOfPaintedColorsOfEachChannel, FVertexDetectVertexColorsOnEachBone vertexColorsOnEachBone, FVertexDetectAdditionalDataToPassThrough additionalData);

	UFUNCTION(BlueprintNativeEvent, Category = "Runtime Vertex Color Paint and Detection Plugin|Detection", meta = (ToolTip = "Runs on the Actor that got Detected on. "))
		void GetAllVertexColorsOnlyOnActor(FVertexDetectTaskResultInfo taskResultInfo, const FVertexDetectMeshDataStruct& meshData, FVertexDetectGetColorsOnlyStruct gotAllVertexColorsWithSettings, FVertexDetectAmountOfPaintedColorsOfEachChannel amountOfPaintedColorsOfEachChannel, FVertexDetectVertexColorsOnEachBone vertexColorsOnEachBone, FVertexDetectAdditionalDataToPassThrough additionalData);

	UFUNCTION(BlueprintNativeEvent, Category = "Runtime Vertex Color Paint and Detection Plugin|Painting", meta = (ToolTip = "Runs on the Actor that got Painted At Location on it"))
		void PaintedOnActor_AtLocation(FVertexDetectTaskResultInfo taskResultInfo, const FVertexDetectMeshDataStruct& meshData, FVertexPaintAtLocationStruct paintedAtLocationSettings, FVertexDetectClosestVertexDataResultStruct closestVertexColorResult, FVertexDetectEstimatedColorAtHitLocationResultStruct estimatedColorAtHitLocationResult, FVertexDetectAvarageColorInAreaInfo avarageColorInAreaInfo, FVertexDetectAmountOfPaintedColorsOfEachChannel amountOfPaintedColorsOfEachChannel, FVertexDetectVertexColorsOnEachBone vertexColorsOnEachBone, FVertexDetectAdditionalDataToPassThrough additionalData);

	UFUNCTION(BlueprintNativeEvent, Category = "Runtime Vertex Color Paint and Detection Plugin|Painting", meta = (ToolTip = "Runs on the Actor that got Painted Within Area on it. "))
		void PaintedOnActor_WithinArea(FVertexDetectTaskResultInfo taskResultInfo, const FVertexDetectMeshDataStruct& meshData, FVertexPainthWithinAreaStruct paintedWithinAreaSettings, FVertexDetectAmountOfPaintedColorsOfEachChannel amountOfPaintedColorsOfEachChannel, FVertexDetectVertexColorsOnEachBone vertexColorsOnEachBone, FVertexDetectAdditionalDataToPassThrough additionalData);

	UFUNCTION(BlueprintNativeEvent, Category = "Runtime Vertex Color Paint and Detection Plugin|Painting", meta = (ToolTip = "Runs on the Actor that got Painted Entire Mesh on it. "))
		void PaintedOnActor_EntireMesh(FVertexDetectTaskResultInfo taskResultInfo, const FVertexDetectMeshDataStruct& meshData, FVertexPaintOnEntireMeshStruct paintedEntireMeshSettings, FVertexDetectAmountOfPaintedColorsOfEachChannel amountOfPaintedColorsOfEachChannel, FVertexDetectVertexColorsOnEachBone vertexColorsOnEachBone, FVertexDetectAdditionalDataToPassThrough additionalData);

	UFUNCTION(BlueprintNativeEvent, Category = "Runtime Vertex Color Paint and Detection Plugin|Painting", meta = (ToolTip = "Runs on the Actor that got Painted Color Snippet on it. "))
		void PaintedOnActor_PaintColorSnippet(FVertexDetectTaskResultInfo taskResultInfo, const FVertexDetectMeshDataStruct& meshData, FVertexPaintColorSnippetStruct paintedColorSnippetSettings, FVertexDetectAmountOfPaintedColorsOfEachChannel amountOfPaintedColorsOfEachChannel, FVertexDetectVertexColorsOnEachBone vertexColorsOnEachBone, FVertexDetectAdditionalDataToPassThrough additionalData);

	UFUNCTION(BlueprintNativeEvent, Category = "Runtime Vertex Color Paint and Detection Plugin|Painting", meta = (ToolTip = "Runs on the Actor that got Colors Applied on it"))
		void ColorsAppliedOnActor(FVertexDetectTaskResultInfo taskResultInfo, const FVertexDetectMeshDataStruct& meshData, FVertexPaintStruct paintedOnActorSettings, FVertexDetectAmountOfPaintedColorsOfEachChannel amountOfPaintedColorsOfEachChannel, FVertexDetectVertexColorsOnEachBone vertexColorsOnEachBone, FVertexDetectAdditionalDataToPassThrough additionalData);


	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Runtime Vertex Color Paint and Detection Plugin|Cloth", meta = (ToolTip = "Returns Cloths and whatever Physics settings they should have depending on RGBA Vertex Colors. For instance if Blue is painted 1, what the physics settings should be for the cloth then. "))
		TMap<UClothingAssetBase*, FVertexDetectClothSettingsStruct> GetSkeletalMeshClothPhysicsSettings(USkeletalMeshComponent* skeletalMeshComponentTryingToGetClothSettingsFor);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Runtime Vertex Color Paint and Detection Plugin|Cloth", meta = (ToolTip = "Runs on the Actor that Begin Overlapped Cloth. "))
		void ClothBeginOverlappingMesh(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Runtime Vertex Color Paint and Detection Plugin|Cloth", meta = (ToolTip = "Runs on the Actor that End Overlapped Cloth."))
		void ClothEndOverlappingMesh(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp);
};
