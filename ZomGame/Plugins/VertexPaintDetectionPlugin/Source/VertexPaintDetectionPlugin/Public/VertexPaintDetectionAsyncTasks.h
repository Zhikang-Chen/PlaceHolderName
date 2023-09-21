// Copyright 2022 Al0exander Floden, Alias Alex River. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "RapidJsonModule.h"
#include "Async/AsyncWork.h"
#include "VertexPaintDetectionStructs.h"

class UBoxComponent;
class USphereComponent;
class USplineMeshComponent;

DECLARE_MULTICAST_DELEGATE_TwoParams(FVertexPaintDetectionAsyncTaskCompleted, FCalculateColorsInfo, EVertexPaintDetectionTaskResult);


struct FDirectionFromHitLocationToClosestVerticesInfo {

	int lodVertexIndex = 0;

	FVector directionFromHitDirectionToVertex = FVector(ForceInitToZero);

	FVector vertexWorldPosition = FVector(ForceInitToZero);

	float distanceFromHitLocation = 0;
};


class VERTEXPAINTDETECTIONPLUGIN_API FVertexPaintDetectionAsyncTasks : public FNonAbandonableTask {


	friend class FAsyncTask<FVertexPaintDetectionAsyncTasks>;
	friend class FAutoDeleteAsyncTask<FVertexPaintDetectionAsyncTasks>;


public:

	FVertexPaintDetectionAsyncTasks(FCalculateColorsInfo calculateColorsInfo);

	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(FVertexPaintDetectionAsyncTasks, STATGROUP_ThreadPoolAsyncTasks);
	}

	void DoWork();

private:

	EVertexPaintDetectionTaskResult CalculateColorToApply(bool updateMeshVertexColorData = true);

	TArray<FColor> GetVertexColorsAtLOD(int lod, UPrimitiveComponent* component);

	bool ShouldLoopThroughVerticesAndLODs(FVertexDetectEventSettings callbackSettings, UPrimitiveComponent* component);

	bool PaintJobShouldLoopThroughVerticesAndLODS(FVertexPaintColorSettingStruct paintColorSettings);

	bool IsVertexWithinArea(const FVector& vertexWorldPosition, const FVertexPainthWithinAreaStruct& vertexPaintWithinAreaStruct, const FVertexPainthWithinArea_ComponentToCheckIfIsWithinInfo& componentToCheckIfIsWithinInfo);

	EVertexPaintDetectionTaskResult IsTaskValid(const int& currentLOD);

	void IsVertexCloseToEstimatedColorToHitLocation(float& currentLongestDistance, int& currentLongestDistanceIndex, TMap<int, FDirectionFromHitLocationToClosestVerticesInfo>& directionFromHitToClosestVertices, const FVector& hitLocationInWorldSpace, const FVector& vertexInWorldSpace, const int& vertexLODIndex);

	int GetEstimatedColorToHitLocationVertexToLerpColorToward(const TMap<int, FDirectionFromHitLocationToClosestVerticesInfo>& directionFromHitToClosestVertices, TArray<FVector>& closestVerticesToTheHitLocationInWorldLocation, const FVector& directionToClosestVertex, const int& closestVertexBased, const float& dotGraceRange = 0.2f);

	void GetEstimatedColorToHitLocationValues(const int& vertexToLerpToward, const FColor& closestVertexColor, const FVector& closestVertexInWorldSpace, const FVector& hitLocationInWorldSpace, const FColor& vertexToLerpTowardColor, const FVector& vertexToLerpTowardPositionInWorldSpace, FLinearColor& estimatedColorAtHitLocation, FVector& estimatedHitLocation);

	FVector GetSplineMeshVertexPositionInMeshSpace(const FVector& vertexPos, USplineMeshComponent* splineMeshComponent);

	FVector GetSplineMeshVertexNormalInMeshSpace(const FVector& normal, USplineMeshComponent* splineMeshComponent);

	bool IsCurrentVertexWithinPaintColorConditions(const FColor& currentVertexColor, const FVector& currentVertexPosition, const FVector& currentVertexNormal, const FName& currentBoneName, const FVertexDetectPaintColorConditionStruct& paintConditions, bool& applyColorStrengthIfConditionIsNotMet, float& colorStrengthIfConditionsIsNotMet);

	bool WillAppliedColorMakeChangeOnVertex(const FColor& currentColorOnVertex, const FVector& currentLocationOnVertex, const FVector& currentNormalOnVertex, const FVertexPaintColorSettingStruct& paintColorSettings);

	void GetVertexColorAfterAnyLimitation(const bool& limitIfColorWasAlreadyOverLimit, const float& originalRedVertexColor, float newRedVertexColor, const float& redVertexColorLimit, const float& originalGreenVertexColor, float newGreenVertexColor, const float& greenVertexColorLimit, const float& originalBlueVertexColor, float newBlueVertexColor, const float& blueVertexColorLimit, const float& originalAlphaVertexColor, float newAlphaVertexColor, const float& alphaVertexColorLimit, float& redColorOUT, float& greenColorOUT, float& blueColorOUT, float& alphaColorOUT);

	FColor GetColorToApplyOnVertex(const FColor& currentVertexColor, const FVector& currentVertexWorldPosition, const FVector& currentVertexNormal, const FName& currentBoneName, const float& fallOffStrength, const float& areaOfEffect, const float& areaOfEffectWithFullColor, const float& distanceToVertexFromPaintLocation, const FVertexPaintColorSettingStruct& paintOnMeshColorSetting, bool& vertexGotColorChanged, const bool& applyColorOnRedChannel, const bool& applyColorOnGreenChannel, const bool& applyColorOnBlueChannel, const bool& applyColorOnAlphaChannel);

	bool IsThereAnyPaintConditions(const FVertexDetectRGBAStruct& applyColorSettings);

	bool CompareVertexColor(const int& vertexIndex, const TArray<FColor>& meshColors, const TArray<FColor>& compareWithColors);

	FVertexDetectPaintColorConditionStruct GetPaintConditionAdjustedIfSetToUsePhysicsSurface(FVertexDetectPaintColorConditionStruct channelsPaintCondition, UMaterialInterface* material);

	void PropogateLOD0VertexColorsToLODs(const TArray<FPaintedVertex>& lod0PaintedVerts, FVertexDetectMeshDataStruct& vertexDetectMeshData, const TArray<FBox>& lodBaseBounds, TArray<FColorVertexBuffer*>& colorVertexBufferToUpdate, UPrimitiveComponent* component);


#if ENGINE_MAJOR_VERSION == 4

	FVector Modified_GetTypedSkinnedVertexPosition(const USkinnedMeshComponent* SkinnedComp, const FSkelMeshRenderSection& Section, const FPositionVertexBuffer& PositionVertexBuffer, const FSkinWeightVertexBuffer& SkinWeightVertexBuffer, const int32 VertIndex, const TArray<FMatrix>& RefToLocals, uint32& boneIndex);

#elif ENGINE_MAJOR_VERSION == 5

	FVector Modified_GetTypedSkinnedVertexPosition(const USkinnedMeshComponent* SkinnedComp, const FSkelMeshRenderSection& Section, const FPositionVertexBuffer& PositionVertexBuffer, const FSkinWeightVertexBuffer& SkinWeightVertexBuffer, const int32 VertIndex, const TArray<FMatrix44f>& RefToLocals, uint32& boneIndex);

#endif

	//  Global Vars

	FCalculateColorsInfo calculateColorsInfo_Global;
	TArray<FString> calculateColorsResultMessage;
	FVertexDetectMeshDataStruct vertexDetectMeshData_Global;
	TMap<FName, FVertexDetectBoneColorsStruct> nameAndColorOfEachBone_Global;
	bool successFullyGotColorsForEachBone_Global = false;
	TArray<FColorVertexBuffer*> newColorVertexBufferPerLOD_Global;
	TMap<UClothingAssetBase*, FVertexDetectChaosClothPhysicsSettings> clothPhysicsSettings_Global;


	FVector GetTypedSkinnedVertexPosition_SkinnedPos_Global = FVector(ForceInitToZero);
	int32 GetTypedSkinnedVertexPosition_BufferVertIndex_Global = 0;
	int32 GetTypedSkinnedVertexPosition_MaxBoneInfluences_Global = 0;
	int32 GetTypedSkinnedVertexPosition_MeshBoneIndex_Global = 0;
	int32 GetTypedSkinnedVertexPosition_TransformBoneIndex_Global = 0;
	float GetTypedSkinnedVertexPosition_Weight_Global = 0;

	// Has a seperate closestDistanceToClosestVertex_Detect_Local and Closest Vertex for detection, so in case you're running a paint at location with detect before/after, but on a different location on the mesh, they don't interfere with eachother. 
	int closestVertexFound_Paint_Global = -1;
	int closestSectionFound_Paint_Global = -1;
	UMaterialInterface* materialAtClosestPaintVertex_Global = nullptr;
	int closestVertexFound_Detection_Global = -1;
	int closestSectionFound_Detection_Global = -1;
	UMaterialInterface* materialAtClosestDetectVertex_Global = nullptr;
	FColor closestVertexColor_BeforeApplyingColors_Detect_Global = FColor();
	FColor closestVertexColor_AfterApplyingColors_Detect_Global = FColor();
	FColor closestVertexColor_AfterApplyingColors_Paint_Global = FColor();

	int estimatedColorAtHitLocation_VertexToLerpToward_Paint_Global = -1;
	int estimatedColorAtHitLocation_VertexToLerpToward_Detect_Global = -1;
	TArray<FVector> estimatedColorAtHitLocation_ClosestVerticesToTheHitLocation_Paint_Global;
	TArray<FVector> estimatedColorAtHitLocation_ClosestVerticesToTheHitLocation_Detect_Global;
	FLinearColor estimatedColorAtHitLocation_BeforeApplyingColors_Detect_Global = FLinearColor();
	FLinearColor estimatedColorAtHitLocation_AfterApplyingColors_Detect_Global = FLinearColor();
	FLinearColor estimatedColorAtHitLocation_BeforeApplyingColors_Paint_Global = FLinearColor();
	FLinearColor estimatedColorAtHitLocation_AfterApplyingColors_Paint_Global = FLinearColor();
	FVector estimatedColorAtHitLocation_LocationInWorldSpace_Paint_Global = FVector(ForceInitToZero);
	FVector estimatedColorAtHitLocation_LocationInWorldSpace_Detect_Global = FVector(ForceInitToZero);
	FVector estimatedColorAtHitLocation_VertexToLerpToward_LocationInWorldSpace_Paint_Global = FVector(ForceInitToZero);
	FVector estimatedColorAtHitLocation_VertexToLerpToward_LocationInWorldSpace_Detect_Global = FVector(ForceInitToZero);

	FVector closestVertexPositionToHitLocationAtMinLOD_World_Paint_Global = FVector(ForceInitToZero);
	FVector closestVertexNormalToHitLocationAtMinLOD_World_Paint_Global = FVector(ForceInitToZero);

	FVector closestVertexPositionToHitLocationAtMinLOD_World_Detect_Global = FVector(ForceInitToZero);
	FVector closestVertexNormalToHitLocationAtMinLOD_World_Detect_Global = FVector(ForceInitToZero);

	FVertexDetectAmountOfPaintedColorsOfEachChannel amountOfPaintedColorsOfEachChannelAbove0_Global = FVertexDetectAmountOfPaintedColorsOfEachChannel();
	FVertexDetectAmountOfPaintedColorsOfEachChannel amountOfPaintedColorsOfEachChannelAtMinAmount_BeforeApplyingColor_Global = FVertexDetectAmountOfPaintedColorsOfEachChannel();
	FVertexDetectAmountOfPaintedColorsOfEachChannel amountOfPaintedColorsOfEachChannelAtMinAmount_Global = FVertexDetectAmountOfPaintedColorsOfEachChannel();
	FVertexDetectCompareMeshColorsToColorArrayResultStruct compareMeshColorsToColorArrayResult_Global;


	bool shouldApplyVertexColors_Global = false;
	bool paintWithinArea_TriedToApplyColorOnAnyVertex_Global = false;

	int amountOfVerticesWithinArea_Detect = 0;
	int amountOfVerticesWithinArea_Detect_After = 0;
	int amountOfVerticesWithinArea_Paint = 0;


	// Average Color within Area
	float avarageColorWithinArea_Global_Detect_Before_Red = 0;
	float avarageColorWithinArea_Global_Detect_Before_Green = 0;
	float avarageColorWithinArea_Global_Detect_Before_Blue = 0;
	float avarageColorWithinArea_Global_Detect_Before_Alpha = 0;

	float avarageColorWithinArea_Global_Detect_After_Red = 0;
	float avarageColorWithinArea_Global_Detect_After_Green = 0;
	float avarageColorWithinArea_Global_Detect_After_Blue = 0;
	float avarageColorWithinArea_Global_Detect_After_Alpha = 0;

	float avarageColorWithinArea_Global_Red = 0;
	float avarageColorWithinArea_Global_Green = 0;
	float avarageColorWithinArea_Global_Blue = 0;
	float avarageColorWithinArea_Global_Alpha = 0;


	// Is within Area Check Globals - Globals so we hopefully save some performance compared to initializing them for every vertex since we can loop through thousands of them

	TArray< FHitResult> isVertexWithinArea_HitResults_Global;
	FCollisionQueryParams isVertexWithinArea_ComplexShape_TraceParams_Global = FCollisionQueryParams();

	bool isVertexWithinArea_ShouldTrace = false;
	FVector isVertexWithinArea_TraceEndLocation = FVector(ForceInitToZero);

	float isVertexWithinArea_SquareXSize_Global = 0;
	float isVertexWithinArea_SquareYSize_Global = 0;
	float isVertexWithinArea_SquareZSize_Global = 0;

	FVector isVertexWithinArea_Vertex_LocalPos_Global = FVector(ForceInitToZero);

	FVector isVertexWithinArea_Square_BackPosWorld_Global = FVector(ForceInitToZero);
	FVector isVertexWithinArea_Square_BackPosLocal_Global = FVector(ForceInitToZero);

	FVector isVertexWithinArea_Square_ForwardPosWorld_Global = FVector(ForceInitToZero);
	FVector isVertexWithinArea_Square_ForwardPosLocal_Global = FVector(ForceInitToZero);

	FVector isVertexWithinArea_Square_LeftPosWorld_Global = FVector(ForceInitToZero);
	FVector isVertexWithinArea_Square_LeftPosLocal_Global = FVector(ForceInitToZero);

	FVector isVertexWithinArea_Square_RightPosWorld_Global = FVector(ForceInitToZero);
	FVector isVertexWithinArea_Square_RightPosLocal_Global = FVector(ForceInitToZero);

	FVector isVertexWithinArea_Square_DownPosWorld_Global = FVector(ForceInitToZero);
	FVector isVertexWithinArea_Square_DownPosLocal_Global = FVector(ForceInitToZero);

	FVector isVertexWithinArea_Square_UpPosWorld_Global = FVector(ForceInitToZero);
	FVector isVertexWithinArea_Square_UpPosLocal_Global = FVector(ForceInitToZero);


	// Color To Apply

	float getColorToApplyOnVertex_Distance_Global = 0;
	float getColorToApplyOnVertex_StrengthAfterFalloff_Global = 1;
	float getColorToApplyOnVertex_FallOffClamped_Global = 0;
	float getColorToApplyOnVertex_Falloff_Global = 0;
	FColor getColorToApplyOnVertex_ColorToApply_Global = FColor();

	float getColorToApplyOnVertex_ColorRed_Global = 0;
	float getColorToApplyOnVertex_ColorGreen_Global = 0;
	float getColorToApplyOnVertex_ColorBlue_Global = 0;
	float getColorToApplyOnVertex_ColorAlpha_Global = 0;

	float tryingToApplyColor_Red_Global = 0;
	float tryingToApplyColor_Green_Global = 0;
	float tryingToApplyColor_Blue_Global = 0;
	float tryingToApplyColor_Alpha_Global = 0;

	bool shouldApplyColorOnAnyChannel_Global = false;
	bool paintCondition_ApplyColorStrengthIfConditionIsNotMet_Global = false;
	float paintCondition_ColorStrengthIfConditionIsNotMet_Global = 0;
	bool paintCondition_AtAnyBone_Global = false;
};
