// Copyright 2022 Alexander Floden, Alias Alex River. All Rights Reserved. 


#include "VertexPaintDetectionAsyncTasks.h"
#include "Async/Async.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"
#include "RapidJsonFunctionLibrary.h" 
#include "Math/GenericOctree.h"
#include "Engine/GameInstance.h"

#include "VertexPaintFunctionLibrary.h"
#include "VertexPaintDetectionGISubSystem.h"
#include "VertexPaintDetectionInterface.h"
#include "VertexPaintColorSnippetDataAsset.h"
#include "VertexPaintMaterialDataAsset.h"

#include "Animation/Skeleton.h"
#include "Engine/StaticMesh.h"
#include "StaticMeshResources.h"
#include "Engine/SkeletalMesh.h"
#include "Rendering/SkeletalMeshRenderData.h"
#include "Rendering/ColorVertexBuffer.h"

#include "Components/StaticMeshComponent.h"
#include "Components/SplineMeshComponent.h"

#include "ClothingAsset.h"
#include "ClothingAssetBase.h"

#if ENGINE_MAJOR_VERSION == 5

#include "ChaosCloth/ChaosClothingSimulation.h"
#include "ChaosCloth/ChaosClothingSimulationInteractor.h"
#include "BoneWeights.h"

#endif


//-------------------------------------------------------

// Construct

FVertexPaintDetectionAsyncTasks::FVertexPaintDetectionAsyncTasks(FCalculateColorsInfo calculateColorsInfo) {

	calculateColorsInfo_Global = calculateColorsInfo;

	// Sets vertexDetectMeshData_Global here that we then change during the calculations, then we set calculateColorsInfo.meshVertexData to be what the result was when finished
	vertexDetectMeshData_Global = calculateColorsInfo.meshVertexData;
}


//-------------------------------------------------------

// Do Work

void FVertexPaintDetectionAsyncTasks::DoWork() {

	// If stopped PIE then doesn't run calculations and broadcasts that it was finished
	if (!IsValid(calculateColorsInfo_Global.vertexPaintDetectionHitActor)) return;
	if (!IsValid(calculateColorsInfo_Global.vertexPaintComponent)) return;
	if (!UVertexPaintFunctionLibrary::IsWorldValid(calculateColorsInfo_Global.worldTaskWasCreatedIn)) return;


	EVertexPaintDetectionTaskResult paintTasResultTemp = EVertexPaintDetectionTaskResult::Fail;


	if (IsValid(calculateColorsInfo_Global.vertexPaintSettings.actor)) {

		newColorVertexBufferPerLOD_Global.SetNum(calculateColorsInfo_Global.lodsToLoopThrough, false);
	}

	if (IsValid(calculateColorsInfo_Global.getAllVertexColorsSettings.actor)) {

		TArray<FColor> colorFromLOD_Local;

		for (int i = 0; i < calculateColorsInfo_Global.lodsToLoopThrough; i++) {

			if (!vertexDetectMeshData_Global.meshDataPerLOD.IsValidIndex(i)) break;


			vertexDetectMeshData_Global.meshDataPerLOD[i].lod = i;

			// If doesn't already have gotten vertex colors previously
			if (vertexDetectMeshData_Global.meshDataPerLOD[i].meshVertexColorsPerLODArray.Num() <= 0)
				vertexDetectMeshData_Global.meshDataPerLOD[i].meshVertexColorsPerLODArray = GetVertexColorsAtLOD(i, calculateColorsInfo_Global.vertexPaintComponent);


			vertexDetectMeshData_Global.meshDataPerLOD[i].amountOfVerticesAtLOD = vertexDetectMeshData_Global.meshDataPerLOD[i].meshVertexColorsPerLODArray.Num();

			// Gets the total one which is the one we use to check if we've fully painted or fully removed a mesh etc. so we don't override vertex colors if there's not gonna be any difference
			if (i == 0) {

				amountOfPaintedColorsOfEachChannelAbove0_Global = UVertexPaintFunctionLibrary::GetAmountOfPaintedColorsForEachChannel(vertexDetectMeshData_Global.meshDataPerLOD[0].meshVertexColorsPerLODArray, 0);
			}
		}


		if (ShouldLoopThroughVerticesAndLODs(calculateColorsInfo_Global.fundementalStruct.callbackSettings, calculateColorsInfo_Global.vertexPaintComponent)) {

			paintTasResultTemp = CalculateColorToApply(false);
		}

		else {

			if (calculateColorsInfo_Global.fundementalStruct.callbackSettings.includeAmountOfPaintedColorsOfEachChannel.includeAmountOfPaintedColorsOfEachChannel && vertexDetectMeshData_Global.meshDataPerLOD.IsValidIndex(0)) {

				amountOfPaintedColorsOfEachChannelAtMinAmount_Global = UVertexPaintFunctionLibrary::GetAmountOfPaintedColorsForEachChannel(vertexDetectMeshData_Global.meshDataPerLOD[0].meshVertexColorsPerLODArray, calculateColorsInfo_Global.fundementalStruct.callbackSettings.includeAmountOfPaintedColorsOfEachChannel.includeAmountOfPaintedColorsOfEachChannel_IfMinColorAmount);

				// Sets the regular one as well so it's consistent with the others, this is the one that will be returned to the user as well. 
				amountOfPaintedColorsOfEachChannelAtMinAmount_BeforeApplyingColor_Global = amountOfPaintedColorsOfEachChannelAtMinAmount_Global;
			}

			paintTasResultTemp = EVertexPaintDetectionTaskResult::Success;
		}
	}

	else if (IsValid(calculateColorsInfo_Global.setMeshComponentVertexColorsSettings.actor)) {

		// If got mesh data at LOD0 set to something
		if (vertexDetectMeshData_Global.meshDataPerLOD.IsValidIndex(0) && vertexDetectMeshData_Global.meshDataPerLOD[0].meshVertexColorsPerLODArray.Num() > 0) {

			int successfullyLODs = 0;

			for (int i = 0; i < calculateColorsInfo_Global.lodsToLoopThrough; i++) {

				if (!vertexDetectMeshData_Global.meshDataPerLOD.IsValidIndex(i)) break;


				// Gets the total one which is the one we use to check if we've fully painted or fully removed a mesh etc. so we don't override vertex colors if there's not gonna be any difference
				if (i == 0) {

					amountOfPaintedColorsOfEachChannelAbove0_Global = UVertexPaintFunctionLibrary::GetAmountOfPaintedColorsForEachChannel(vertexDetectMeshData_Global.meshDataPerLOD[0].meshVertexColorsPerLODArray, 0);
				}


				if (vertexDetectMeshData_Global.meshDataPerLOD[i].meshVertexColorsPerLODArray.Num() > 0) {

					vertexDetectMeshData_Global.meshDataPerLOD[i].lod = i;
					vertexDetectMeshData_Global.meshDataPerLOD[i].amountOfVerticesAtLOD = vertexDetectMeshData_Global.meshDataPerLOD[i].meshVertexColorsPerLODArray.Num();

					newColorVertexBufferPerLOD_Global[i] = new FColorVertexBuffer();
					newColorVertexBufferPerLOD_Global[i]->InitFromColorArray(vertexDetectMeshData_Global.meshDataPerLOD[i].meshVertexColorsPerLODArray);

					successfullyLODs++;
				}
			}


			// Just like Paint Entire Mesh with Set, always result in change since we have no way of way of checking if they did without looping through verts, which would make the task take longer. So sets to true so colors always get applied
			shouldApplyVertexColors_Global = true;


			if (ShouldLoopThroughVerticesAndLODs(calculateColorsInfo_Global.fundementalStruct.callbackSettings, calculateColorsInfo_Global.vertexPaintComponent)) {

				// True since Paint Color Snippet and SetMeshVertexColors Propogate LOD0 to other LODs
				paintTasResultTemp = CalculateColorToApply(true);
			}

			else {

				if (calculateColorsInfo_Global.fundementalStruct.callbackSettings.includeAmountOfPaintedColorsOfEachChannel.includeAmountOfPaintedColorsOfEachChannel && vertexDetectMeshData_Global.meshDataPerLOD.IsValidIndex(0))
					amountOfPaintedColorsOfEachChannelAtMinAmount_Global = UVertexPaintFunctionLibrary::GetAmountOfPaintedColorsForEachChannel(vertexDetectMeshData_Global.meshDataPerLOD[0].meshVertexColorsPerLODArray, calculateColorsInfo_Global.fundementalStruct.callbackSettings.includeAmountOfPaintedColorsOfEachChannel.includeAmountOfPaintedColorsOfEachChannel_IfMinColorAmount);


				if (vertexDetectMeshData_Global.meshDataPerLOD.Num() > 1 && calculateColorsInfo_Global.propogateLOD0ToAllLODs) {

					paintTasResultTemp = CalculateColorToApply(true);
				}

				else {

					paintTasResultTemp = EVertexPaintDetectionTaskResult::Success;
				}
			}
		}
	}

	else if (IsValid(calculateColorsInfo_Global.paintOnColorSnippetSettings.actor) || IsValid(calculateColorsInfo_Global.setMeshComponentVertexColorsUsingSerializedStringSettings.actor)) {


		TArray<FColor> originalColorsFromLOD_Local;

		if (!vertexDetectMeshData_Global.meshDataPerLOD.IsValidIndex(0))
			vertexDetectMeshData_Global.meshDataPerLOD.SetNum(1, false);

		// If this task was prefilled from the former task (which is the case if there was a queue and vertex colors was included in the callback settings) then we can use them instead of running GetVertexColorsAtLOD()
		originalColorsFromLOD_Local = vertexDetectMeshData_Global.meshDataPerLOD[0].meshVertexColorsPerLODArray;
		int amountOfVertices_Local = 0;

		if (originalColorsFromLOD_Local.Num() <= 0)
			originalColorsFromLOD_Local = GetVertexColorsAtLOD(0, calculateColorsInfo_Global.vertexPaintComponent);

		amountOfVertices_Local = originalColorsFromLOD_Local.Num();

		if (vertexDetectMeshData_Global.meshDataPerLOD[0].meshVertexColorsPerLODArray.Num() <= 0)
			vertexDetectMeshData_Global.meshDataPerLOD[0].meshVertexColorsPerLODArray.SetNumZeroed(originalColorsFromLOD_Local.Num(), false);

		// Gets the total one which is the one we use to check if we've fully painted or fully removed a mesh etc. so we don't override vertex colors if there's not gonna be any difference
		amountOfPaintedColorsOfEachChannelAbove0_Global = UVertexPaintFunctionLibrary::GetAmountOfPaintedColorsForEachChannel(originalColorsFromLOD_Local, 0);


		if (calculateColorsInfo_Global.setMeshComponentVertexColorsUsingSerializedStringSettings.actor) {

			FString rapidJsonStringToDeSerialize = calculateColorsInfo_Global.setMeshComponentVertexColorsUsingSerializedStringSettings.serializedColorDataAtLOD0;

			if (rapidJsonStringToDeSerialize.Len() > 0) {

				// De-Serializes the colors to apply
				vertexDetectMeshData_Global.meshDataPerLOD[0].meshVertexColorsPerLODArray = URapidJsonFunctionLibrary::DeserializeTArrayFColor_Wrapper(rapidJsonStringToDeSerialize);
			}

			else {

				calculateColorsResultMessage.Add(FString::Printf(TEXT("Runtime Vertex Color Paint and Detection Plugin - Set Mesh Colors with Serialized String Task Fail as the String to De-Serialize and get colors from is 0 in Length!")));
			}
		}

		else if (calculateColorsInfo_Global.paintOnColorSnippetSettings.actor && calculateColorsInfo_Global.paintOnColorSnippetSettings.colorSnippetDataAssetSnippetIsStoredIn) {

			if (calculateColorsInfo_Global.paintOnColorSnippetSettings.colorSnippetDataAssetSnippetIsStoredIn->snippetColorData.Contains(calculateColorsInfo_Global.paintOnColorSnippetSettings.colorSnippetID)) {

				if (vertexDetectMeshData_Global.meshDataPerLOD.IsValidIndex(0) && calculateColorsInfo_Global.paintOnColorSnippetSettings.colorSnippetDataAssetSnippetIsStoredIn->snippetColorData.FindRef(calculateColorsInfo_Global.paintOnColorSnippetSettings.colorSnippetID).colorSnippetDataPerLOD.IsValidIndex(0)) {

					vertexDetectMeshData_Global.meshDataPerLOD[0].meshVertexColorsPerLODArray = calculateColorsInfo_Global.paintOnColorSnippetSettings.colorSnippetDataAssetSnippetIsStoredIn->snippetColorData.FindRef(calculateColorsInfo_Global.paintOnColorSnippetSettings.colorSnippetID).colorSnippetDataPerLOD[0].meshVertexColorsPerLODArray;
				}
			}
		}


		// If the De-serialized Colors at LOD0 matches the original LOD0 colors amount
		if (vertexDetectMeshData_Global.meshDataPerLOD[0].meshVertexColorsPerLODArray.Num() == amountOfVertices_Local) {

			newColorVertexBufferPerLOD_Global[0] = new FColorVertexBuffer();
			newColorVertexBufferPerLOD_Global[0]->InitFromColorArray(vertexDetectMeshData_Global.meshDataPerLOD[0].meshVertexColorsPerLODArray);
			vertexDetectMeshData_Global.meshDataPerLOD[0].lod = 0;
			vertexDetectMeshData_Global.meshDataPerLOD[0].amountOfVerticesAtLOD = vertexDetectMeshData_Global.meshDataPerLOD[0].meshVertexColorsPerLODArray.Num();

			shouldApplyVertexColors_Global = true;


			// True since Paint Color Snippet and SetMeshVertexColors Propogate LOD0 to other LODs
			if ((vertexDetectMeshData_Global.meshDataPerLOD.Num() > 1 && calculateColorsInfo_Global.propogateLOD0ToAllLODs) || ShouldLoopThroughVerticesAndLODs(calculateColorsInfo_Global.fundementalStruct.callbackSettings, calculateColorsInfo_Global.vertexPaintComponent)) {

				paintTasResultTemp = CalculateColorToApply(true);
			}

			else {

				if (calculateColorsInfo_Global.fundementalStruct.callbackSettings.includeAmountOfPaintedColorsOfEachChannel.includeAmountOfPaintedColorsOfEachChannel && vertexDetectMeshData_Global.meshDataPerLOD.IsValidIndex(0)) {

					amountOfPaintedColorsOfEachChannelAtMinAmount_Global = UVertexPaintFunctionLibrary::GetAmountOfPaintedColorsForEachChannel(vertexDetectMeshData_Global.meshDataPerLOD[0].meshVertexColorsPerLODArray, calculateColorsInfo_Global.fundementalStruct.callbackSettings.includeAmountOfPaintedColorsOfEachChannel.includeAmountOfPaintedColorsOfEachChannel_IfMinColorAmount);
				}

				paintTasResultTemp = EVertexPaintDetectionTaskResult::Success;
			}
		}

		else {

			calculateColorsResultMessage.Add(FString::Printf(TEXT("Runtime Vertex Color Paint and Detection Plugin - Paint Color Snippet / Set Mesh Colors with Serialized String Task Fail as the Color Array didn't match the amount of Vertices the Mesh has at LOD0! Mesh LOD0 Vertex Amount: %i   -  Deserialized LOD0 Amount: %i"), amountOfVertices_Local, vertexDetectMeshData_Global.meshDataPerLOD[0].meshVertexColorsPerLODArray.Num()));
		}
	}

	else if (IsValid(calculateColorsInfo_Global.paintOnEntireMeshSettings.actor)) {

		if (!calculateColorsInfo_Global.paintOnEntireMeshSettings.paintOnRandomVerticesSettings.paintAtRandomVerticesSpreadOutOverTheEntireMesh && calculateColorsInfo_Global.paintOnMeshColorSettings.applyVertexColorSettings.applyVertexColorSetting == EApplyVertexColorSetting::ESetVertexColor) {


			// If there's no reason to loop through all the vertices then initializes the color buffer right away. Checks if set to apply vertex colors using physics surface as well, because if so, then we need to loop through the vertices since we need to get the sections to get the material of each section and thus the physics surface registered at RGBA at each section and if they have the physics surface we're trying to paint. 
			if (!calculateColorsInfo_Global.paintOnEntireMeshSettings.applyVertexColorSettings.applyColorsUsingPhysicsSurface.applyVertexColorUsingPhysicsSurface && PaintJobShouldLoopThroughVerticesAndLODS(calculateColorsInfo_Global.paintOnEntireMeshSettings) && !IsThereAnyPaintConditions(calculateColorsInfo_Global.paintOnEntireMeshSettings.applyVertexColorSettings)) {

				int successfullyLODs = 0;


				for (int i = 0; i < calculateColorsInfo_Global.lodsToLoopThrough; i++) {

					if (!vertexDetectMeshData_Global.meshDataPerLOD.IsValidIndex(i)) break;


					int32 meshLODTotalAmountOfVerts_Local = 0;


					// Gets the total one which is the one we use to check if we've fully painted or fully removed a mesh etc. so we don't override vertex colors if there's not gonna be any difference
					if (i == 0)
						amountOfPaintedColorsOfEachChannelAbove0_Global = UVertexPaintFunctionLibrary::GetAmountOfPaintedColorsForEachChannel(GetVertexColorsAtLOD(i, calculateColorsInfo_Global.vertexPaintComponent), 0);


					if (calculateColorsInfo_Global.vertexPaintSkelComponent) {

						meshLODTotalAmountOfVerts_Local = calculateColorsInfo_Global.vertexPaintSkelComponent->GetSkeletalMeshRenderData()->LODRenderData[i].GetNumVertices();
					}

					else if (calculateColorsInfo_Global.vertexPaintStaticMeshComponent) {

						meshLODTotalAmountOfVerts_Local = calculateColorsInfo_Global.vertexPaintStaticMeshComponent->GetStaticMesh()->GetRenderData()->LODResources[i].VertexBuffers.PositionVertexBuffer.GetNumVertices();
					}


					FLinearColor colorToInitialize_Local = FLinearColor();
					colorToInitialize_Local.R = calculateColorsInfo_Global.paintOnEntireMeshSettings.applyVertexColorSettings.redColor;
					colorToInitialize_Local.G = calculateColorsInfo_Global.paintOnEntireMeshSettings.applyVertexColorSettings.greenColor;
					colorToInitialize_Local.B = calculateColorsInfo_Global.paintOnEntireMeshSettings.applyVertexColorSettings.blueColor;
					colorToInitialize_Local.A = calculateColorsInfo_Global.paintOnEntireMeshSettings.applyVertexColorSettings.alphaColor;


					newColorVertexBufferPerLOD_Global[i] = new FColorVertexBuffer();
					newColorVertexBufferPerLOD_Global[i]->InitFromSingleColor(colorToInitialize_Local.ToFColor(false), meshLODTotalAmountOfVerts_Local);


					TArray<FColor> colorFromLOD_Local;
					newColorVertexBufferPerLOD_Global[i]->GetVertexColors(colorFromLOD_Local);

					vertexDetectMeshData_Global.meshDataPerLOD[i].lod = i;
					vertexDetectMeshData_Global.meshDataPerLOD[i].meshVertexColorsPerLODArray = colorFromLOD_Local;
					vertexDetectMeshData_Global.meshDataPerLOD[i].amountOfVerticesAtLOD = colorFromLOD_Local.Num();

					successfullyLODs++;
				}

				// If Setting the Entire Mesh Color, always result in change since we have no way of way of checking if they did without looping through verts, which would make the task take longer. So sets to true
				shouldApplyVertexColors_Global = true;


				paintTasResultTemp = EVertexPaintDetectionTaskResult::Success;
			}

			else {

				paintTasResultTemp = CalculateColorToApply(true);
			}
		}

		else {

			paintTasResultTemp = CalculateColorToApply(true);
		}
	}

	else {

		paintTasResultTemp = CalculateColorToApply(true);

	}



	// So far only Async Task Globals has been Set, since we don't want to set calculateColorsInfo_Global when we run the calculations. So here we set the calculateColorsInfo_Global global with their settings. It is what the actual result will be and what will be sent back to the game thread


	calculateColorsInfo_Global.shouldApplyVertexColors = shouldApplyVertexColors_Global;


	// Mesh Data
	calculateColorsInfo_Global.meshVertexData = vertexDetectMeshData_Global;

	calculateColorsInfo_Global.paintWithinArea_TriedToApplyColorOnAnyVertex = paintWithinArea_TriedToApplyColorOnAnyVertex_Global;

	// Color Buffer
	calculateColorsInfo_Global.newColorVertexBufferPerLOD = newColorVertexBufferPerLOD_Global;

	// Closest Vertex
	calculateColorsInfo_Global.closestVertexFound_Paint = closestVertexFound_Paint_Global;
	calculateColorsInfo_Global.closestSectionFound_Paint = closestSectionFound_Paint_Global;
	calculateColorsInfo_Global.materialAtClosestPaintVertex = materialAtClosestPaintVertex_Global;

	calculateColorsInfo_Global.closestVertexFound_Detection = closestVertexFound_Detection_Global;
	calculateColorsInfo_Global.closestSectionFound_Detect = closestSectionFound_Detection_Global;
	calculateColorsInfo_Global.materialAtClosestDetectVertex = materialAtClosestDetectVertex_Global;


	calculateColorsInfo_Global.closestVertexColor_BeforeApplyingColors_Detect = closestVertexColor_BeforeApplyingColors_Detect_Global;
	calculateColorsInfo_Global.closestVertexColor_AfterApplyingColors_Detect = closestVertexColor_AfterApplyingColors_Detect_Global;
	calculateColorsInfo_Global.closestVertexColor_AfterApplyingColors_Paint = closestVertexColor_AfterApplyingColors_Paint_Global;


	calculateColorsInfo_Global.closestVertexPositionToHitLocationAtMinLOD_World_Paint = closestVertexPositionToHitLocationAtMinLOD_World_Paint_Global;
	calculateColorsInfo_Global.closestVertexNormalToHitLocationAtMinLOD_World_Paint = closestVertexNormalToHitLocationAtMinLOD_World_Paint_Global;

	calculateColorsInfo_Global.closestVertexPositionToHitLocationAtMinLOD_World_Detect = closestVertexPositionToHitLocationAtMinLOD_World_Detect_Global;
	calculateColorsInfo_Global.closestVertexNormalToHitLocationAtMinLOD_World_Detect = closestVertexNormalToHitLocationAtMinLOD_World_Detect_Global;


	// Estimated Color at Hit Location
	calculateColorsInfo_Global.estimatedColorAtHitLocation_VertexToLerpToward_Paint = estimatedColorAtHitLocation_VertexToLerpToward_Paint_Global;
	calculateColorsInfo_Global.estimatedColorAtHitLocation_VertexToLerpToward_Detect = estimatedColorAtHitLocation_VertexToLerpToward_Detect_Global;

	calculateColorsInfo_Global.estimatedColorAtHitLocation_LocationInWorldSpace_Paint = estimatedColorAtHitLocation_LocationInWorldSpace_Paint_Global;
	calculateColorsInfo_Global.estimatedColorAtHitLocation_LocationInWorldSpace_Detect = estimatedColorAtHitLocation_LocationInWorldSpace_Detect_Global;

	calculateColorsInfo_Global.estimatedColorAtHitLocation_VertexToLerpToward_LocationInWorldSpace_Paint = estimatedColorAtHitLocation_VertexToLerpToward_LocationInWorldSpace_Paint_Global;
	calculateColorsInfo_Global.estimatedColorAtHitLocation_VertexToLerpToward_LocationInWorldSpace_Detect = estimatedColorAtHitLocation_VertexToLerpToward_LocationInWorldSpace_Detect_Global;

	calculateColorsInfo_Global.estimatedColorAtHitLocation_BeforeApplyingColors_Detect = estimatedColorAtHitLocation_BeforeApplyingColors_Detect_Global;
	calculateColorsInfo_Global.estimatedColorAtHitLocation_AfterApplyingColors_Detect = estimatedColorAtHitLocation_AfterApplyingColors_Detect_Global;
	calculateColorsInfo_Global.estimatedColorAtHitLocation_AfterApplyingColors_Paint = estimatedColorAtHitLocation_AfterApplyingColors_Paint_Global;

	calculateColorsInfo_Global.estimatedColorAtHitLocation_ClosestVertexWorldLocationsToTheHitLocation_Paint = estimatedColorAtHitLocation_ClosestVerticesToTheHitLocation_Paint_Global;
	calculateColorsInfo_Global.estimatedColorAtHitLocation_ClosestVertexWorldLocationsToTheHitLocation_Detect = estimatedColorAtHitLocation_ClosestVerticesToTheHitLocation_Detect_Global;


	// Amount of Colors of Each Channel
	calculateColorsInfo_Global.amountOfPaintedColorsOfEachChannelAbove0 = amountOfPaintedColorsOfEachChannelAbove0_Global;
	calculateColorsInfo_Global.amountOfPaintedColorsOfEachChannelAtMinAmount_BeforeApplyingColor = amountOfPaintedColorsOfEachChannelAtMinAmount_BeforeApplyingColor_Global;
	calculateColorsInfo_Global.amountOfPaintedColorsOfEachChannelAtMinAmount_AfterApplyingColor = amountOfPaintedColorsOfEachChannelAtMinAmount_Global;


	// Average Color
	calculateColorsInfo_Global.avarageColorWithinArea_Detect_BeforeApplyingColor = FColor(avarageColorWithinArea_Global_Detect_Before_Red / amountOfVerticesWithinArea_Detect, avarageColorWithinArea_Global_Detect_Before_Green / amountOfVerticesWithinArea_Detect, avarageColorWithinArea_Global_Detect_Before_Blue / amountOfVerticesWithinArea_Detect, avarageColorWithinArea_Global_Detect_Before_Alpha / amountOfVerticesWithinArea_Detect);

	calculateColorsInfo_Global.avarageColorWithinArea_Detect_AfterApplyingColor = FColor(avarageColorWithinArea_Global_Detect_After_Red / amountOfVerticesWithinArea_Detect_After, avarageColorWithinArea_Global_Detect_After_Green / amountOfVerticesWithinArea_Detect_After, avarageColorWithinArea_Global_Detect_After_Blue / amountOfVerticesWithinArea_Detect_After, avarageColorWithinArea_Global_Detect_After_Alpha / amountOfVerticesWithinArea_Detect_After);

	calculateColorsInfo_Global.avarageColorWithinArea_Paint = FColor(avarageColorWithinArea_Global_Red / amountOfVerticesWithinArea_Paint, avarageColorWithinArea_Global_Green / amountOfVerticesWithinArea_Paint, avarageColorWithinArea_Global_Blue / amountOfVerticesWithinArea_Paint, avarageColorWithinArea_Global_Alpha / amountOfVerticesWithinArea_Paint);


	// Bone
	calculateColorsInfo_Global.vertexColorsOnEachBone.nameAndColorOfEachBone = nameAndColorOfEachBone_Global;
	calculateColorsInfo_Global.vertexColorsOnEachBone.successFullyGotColorsForEachBone = successFullyGotColorsForEachBone_Global;

	// Cloth
	if (calculateColorsInfo_Global.vertexPaintSettings.affectClothPhysics)
		calculateColorsInfo_Global.clothPhysicsSettings = clothPhysicsSettings_Global;


	// Task Result
	calculateColorsInfo_Global.taskResult.compareMeshColorsToColorArrayResult = compareMeshColorsToColorArrayResult_Global;


	if (calculateColorsInfo_Global.fundementalStruct.callbackSettings.includeVertexDataOnlyForLOD0 && calculateColorsInfo_Global.meshVertexData.meshDataPerLOD.Num() > 1) {

		calculateColorsResultMessage.Add(FString::Printf(TEXT("Runtime Vertex Color Paint and Detection Plugin - Adjusting Amount of Mesh Data LODs as the Callback Settings is set to only Include Vertex Data for LOD0. "), *calculateColorsInfo_Global.vertexPaintDetectionHitActor->GetName(), *calculateColorsInfo_Global.vertexPaintComponent->GetName(), calculateColorsInfo_Global.lodsToLoopThrough));
	}


	// Adjusts callbacks so we don't send things back to game thread that we don't need. Was a noticable performance difference if painting on extremely heavy meshes like 1m vertices if you didn't include colors, position or normal data. Does Not reset amount of colors here since we need it later to check if painting on a fully painted mesh, if so we can print debug string and not Apply Vertex Colors since it won't make a difference
	if (IsValid(calculateColorsInfo_Global.vertexPaintSettings.actor)) {

		calculateColorsInfo_Global = UVertexPaintFunctionLibrary::AdjustCallbackVertexData(calculateColorsInfo_Global, true, false, true);
	}

	// If its a detect however then we can fully adjust then since it doesn't apply vertex colors or print anything regarding it. 
	else if (IsValid(calculateColorsInfo_Global.detectInfoSettings.actor)) {

		calculateColorsInfo_Global = UVertexPaintFunctionLibrary::AdjustCallbackVertexData(calculateColorsInfo_Global, true, true, true);
	}


	if (IsInGameThread()) {

		if (UVertexPaintFunctionLibrary::IsWorldValid(calculateColorsInfo_Global.worldTaskWasCreatedIn)) {

			UGameInstance* gameInstance_Local = calculateColorsInfo_Global.worldTaskWasCreatedIn->GetGameInstance();

			if (IsValid(gameInstance_Local)) {

				auto gameInstanceSubsystem_Local = gameInstance_Local->GetSubsystem<UVertexPaintDetectionGISubSystem>();

				if (IsValid(gameInstanceSubsystem_Local)) {

					gameInstanceSubsystem_Local->CalculateColorsTaskFinished(calculateColorsInfo_Global, paintTasResultTemp, calculateColorsResultMessage);
				}
			}
		}
	}

	else {

		FCalculateColorsInfo calculateColorsInfoTemp = calculateColorsInfo_Global;
		TArray<FString> resultMessageTemp = calculateColorsResultMessage;

		// Broadcasts on Game Thread so the actual Applying of colors happens there
		AsyncTask(ENamedThreads::GameThread, [calculateColorsInfoTemp, paintTasResultTemp, resultMessageTemp]() {


			if (UVertexPaintFunctionLibrary::IsWorldValid(calculateColorsInfoTemp.worldTaskWasCreatedIn)) {

				UGameInstance* gameInstance_Local = calculateColorsInfoTemp.worldTaskWasCreatedIn->GetGameInstance();

				if (IsValid(gameInstance_Local)) {

					auto gameInstanceSubsystem_Local = gameInstance_Local->GetSubsystem<UVertexPaintDetectionGISubSystem>();

					if (IsValid(gameInstanceSubsystem_Local)) {

						gameInstanceSubsystem_Local->CalculateColorsTaskFinished(calculateColorsInfoTemp, paintTasResultTemp, resultMessageTemp);
					}
				}
			}
			});
	}
}


//-------------------------------------------------------

// Calculate Color To Apply And Fill Vertex Data Arrays

EVertexPaintDetectionTaskResult FVertexPaintDetectionAsyncTasks::CalculateColorToApply(bool updateMeshVertexColorData) {


	calculateColorsResultMessage.Add(FString::Printf(TEXT("Runtime Vertex Color Paint and Detection Plugin - CalculateColorToApply() on Actor %s and Mesh: %s with %i LODs!"), *calculateColorsInfo_Global.vertexPaintDetectionHitActor->GetName(), *calculateColorsInfo_Global.vertexPaintComponent->GetName(), calculateColorsInfo_Global.lodsToLoopThrough));


	// Fundementals
	int lodsToLoopThrough_Local = calculateColorsInfo_Global.lodsToLoopThrough;
	int currentLODVertex_Local = 0;
	FVector currentVertexActorSpace = FVector(ForceInitToZero);
	FVector currentVertexWorldSpace = FVector(ForceInitToZero);
	FVector skeletalMeshComponentLocal_Local = FVector(ForceInitToZero);
	FVector hitLocation_WorldSpace_Paint_Local = FVector(ForceInitToZero);
	FVector hitLocation_WorldSpace_Detection_Local = FVector(ForceInitToZero);
	FVector hitNormal_Detection_Local = FVector(ForceInitToZero);
	bool vertexColorGotChanged_Local = false;
	bool triedToApplyColorOnVertex_Local = false;
	FLinearColor linearColor_Local;
	float vertexNormalMinDotToHitNormalToGetColored_MinDot_Local = 0;
	float cureentVertexNormalToHitNormalDot_Paint = 0;
	float cureentVertexNormalToHitNormalDot_Detect = 0;
	FVertexDetectGetAverageColorStruct detectGetAverageColor_Local;
	FVertexDetectRGBAStruct originalApplyVertexColorSettings;
	bool applyColorsOnRedChannel_Local = true;
	bool applyColorsOnGreenChannel_Local = true;
	bool applyColorsOnBlueChannel_Local = true;
	bool applyColorsOnAlphaChannel_Local = true;

	// Cloth
	TArray<UClothingAssetBase*> clothingAssets_Local;
	TMap<UClothingAssetBase*, FVertexDetectClothSettingsStruct> clothsToColorAndPhysicsSettings_Local; // Only run at the end and not every vertex so can be TMap as it isn't as costly with .FindRef
	TArray<FVector> clothBoneLocationInComponentSpace_Local;
	TArray<FQuat> clothBoneQuaternionsInComponentSpace_Local;
	TArray<TArray<FColor>> clothPaintedColors_Local;
	TArray<UClothingAssetBase*> paintedCloths_Local; // Cloths that got painted in the order they got painted in. 
	TArray<FVertexDetectRGBAStruct> paintedClothsAndAvarageColor_Local;
	FClothSimulData currentClothSimulData_Local;
	UClothingAssetBase* currentClothingAssetBase_Local;
	uint32 currentBoneIndex_Local = 0;
	FName currentBoneName_Local;
	TArray<TArray<FColor>> boneIndexColors_Local;

	// Paint Within Area
	TArray<float> paintWithinArea_AreaOfEffects_Local;
	float paintWithinArea_DistanceFromVertex_Local = 0;

	// Amount of Colors of Each Channel
	FVertexDetectAmountOfPaintedColorsOfEachChannel amountOfPaintedColorsOfEachChannel_Total_Local;
	FVertexDetectAmountOfPaintedColorsOfEachChannel amountOfPaintedColorsOfEachChannel_BeforeApplyingColor_Local;
	FVertexDetectAmountOfPaintedColorsOfEachChannel amountOfPaintedColorsOfEachChannel_Local;

	// Estimated Color at Hit Location
	bool getEstimatedColorAtHitLocation_Local = false;
	FVector directionToClosestVertex_Paint_Local;
	FVector directionToClosestVertex_Detect_Local;
	float directionFromHitToClosestVerticesInfo_LongestDistanceTemp_Paint = 10000000;
	int directionFromHitToClosestVerticesInfo_LongestDistanceIndexTemp_Paint = -1;
	TMap<int, FDirectionFromHitLocationToClosestVerticesInfo> directionFromHitToClosestVerticesInfo_Paint;

	float directionFromHitToClosestVerticesInfo_LongestDistanceTemp_Detect = 10000000;
	int directionFromHitToClosestVerticesInfo_LongestDistanceIndexTemp_Detect = -1;
	TMap<int, FDirectionFromHitLocationToClosestVerticesInfo> directionFromHitToClosestVerticesInfo_Detect;

	// Paint Entire Mesh Random & Propogate Colors to other LODs
	TArray<FBox> completeLODsBaseBounds_Local;
	TArray<FPaintedVertex> paintedVerticesAtLOD0_Local;
	float paintEntireMesh_Random_AmountOfVerticesLeftToRandomize_Local = 0;
	TMap<FVector, FColor> paintEntireMeshRandom_VerticesDoublettesPaintedAtLOD0_Local;


	USkeletalMesh* skelMeshTemp = nullptr;

#if ENGINE_MAJOR_VERSION == 4

	TArray<FVector> currentClothSectionVertexPositionsTemp_Local;
	TArray<FVector> currentClothSectionVertexNormalsTemp_Local;
	TMap<int32, TArray<FVector>> skeletalMeshClothSimulData_Position_Local;
	TMap<int32, TArray<FVector>> skeletalMeshClothSimulData_Normals_Local;
	TArray<FMatrix> refToLocals_Local;

	if (IsValid(calculateColorsInfo_Global.vertexPaintSkelComponent))
		skelMeshTemp = calculateColorsInfo_Global.vertexPaintSkelComponent->SkeletalMesh;

#elif ENGINE_MAJOR_VERSION == 5

	TArray<FVector3f> currentClothSectionVertexPositionsTemp_Local;
	TArray<FVector3f> currentClothSectionVertexNormalsTemp_Local;
	TMap<int32, TArray<FVector3f>> skeletalMeshClothSimulData_Position_Local;
	TMap<int32, TArray<FVector3f>> skeletalMeshClothSimulData_Normals_Local;
	TArray<FMatrix44f> refToLocals_Local;


#if ENGINE_MINOR_VERSION == 0

	if (IsValid(calculateColorsInfo_Global.vertexPaintSkelComponent))
		skelMeshTemp = calculateColorsInfo_Global.vertexPaintSkelComponent->SkeletalMesh.Get();


#else

	if (IsValid(calculateColorsInfo_Global.vertexPaintSkelComponent))
		skelMeshTemp = calculateColorsInfo_Global.vertexPaintSkelComponent->GetSkeletalMeshAsset();

#endif
#endif


	if (calculateColorsInfo_Global.vertexPaintSkelComponent) {

		// If current viewable LOD has cloth
		if (calculateColorsInfo_Global.vertexPaintSkelComponent->GetSkeletalMeshRenderData()->LODRenderData[calculateColorsInfo_Global.vertexPaintSkelComponent->GetPredictedLODLevel()].HasClothData()) {

			if (calculateColorsInfo_Global.vertexPaintSettings.affectClothPhysics && UKismetSystemLibrary::DoesImplementInterface(calculateColorsInfo_Global.vertexPaintSettings.actor, UVertexPaintDetectionInterface::StaticClass())) {

				clothsToColorAndPhysicsSettings_Local = IVertexPaintDetectionInterface::Execute_GetSkeletalMeshClothPhysicsSettings(calculateColorsInfo_Global.vertexPaintSettings.actor, calculateColorsInfo_Global.vertexPaintSkelComponent);
			}


			TMap<int32, FClothSimulData> skeletalMeshClothSimulData_Local;

			if (IsInGameThread())
				skeletalMeshClothSimulData_Local = calculateColorsInfo_Global.vertexPaintSkelComponent->GetCurrentClothingData_GameThread();
			else
				skeletalMeshClothSimulData_Local = calculateColorsInfo_Global.vertexPaintSkelComponent->GetCurrentClothingData_AnyThread();

			clothingAssets_Local = skelMeshTemp->GetMeshClothingAssets();


			for (int i = 0; i < clothingAssets_Local.Num(); i++) {


				skeletalMeshClothSimulData_Position_Local.Add(i, skeletalMeshClothSimulData_Local.FindRef(i).Positions);
				skeletalMeshClothSimulData_Normals_Local.Add(i, skeletalMeshClothSimulData_Local.FindRef(i).Normals);


#if ENGINE_MAJOR_VERSION == 5

				if (auto clothingAssetCommon_Local = Cast<UClothingAssetCommon>(clothingAssets_Local[i])) {

					// Get Bone Transform in World Space
					auto boneTransform_Local = calculateColorsInfo_Global.vertexPaintSkelComponent->GetBoneTransform(clothingAssetCommon_Local->ReferenceBoneIndex);

					// Transforms from World to Component Local
					boneTransform_Local.SetLocation(UKismetMathLibrary::InverseTransformLocation(calculateColorsInfo_Global.vertexPaintSkelComponent->GetComponentTransform(), boneTransform_Local.GetLocation()));

					clothBoneLocationInComponentSpace_Local.Add(boneTransform_Local.GetLocation());

					clothBoneQuaternionsInComponentSpace_Local.Add(calculateColorsInfo_Global.vertexPaintSkelComponent->GetBoneQuaternion(calculateColorsInfo_Global.vertexPaintSkelComponent->GetBoneName(clothingAssetCommon_Local->ReferenceBoneIndex), EBoneSpaces::ComponentSpace));

				}
#endif
			}
		}
	}

	if (calculateColorsInfo_Global.paintOnMeshWithinAreaSettings.actor) {


		// If paint within area then sets area of effect using componentToCheckIfIsWithin bounds
		paintWithinArea_AreaOfEffects_Local.SetNum(calculateColorsInfo_Global.paintOnMeshWithinArea_ComponentsToCheckIfIsWithinInfo.Num());


		// Since complex shape we can be dependent on the component still existing and on the components current location because we're tracing for it anyway. 
		for (int i = 0; i < calculateColorsInfo_Global.paintOnMeshWithinArea_ComponentsToCheckIfIsWithinInfo.Num(); i++) {


			// Transforms from Mesh to Paint Local to World and sets the World Transform again so we can use it as intended
			calculateColorsInfo_Global.paintOnMeshWithinArea_ComponentsToCheckIfIsWithinInfo[i].componentToCheckIfIsWithin_WorldTransform.SetLocation(UKismetMathLibrary::TransformLocation(calculateColorsInfo_Global.vertexPaintComponent->GetComponentTransform(), calculateColorsInfo_Global.paintOnMeshWithinArea_ComponentsToCheckIfIsWithinInfo[i].componentToCheckIfIsWithin_LocationInMeshComponentToPaintSpace));

			calculateColorsInfo_Global.paintOnMeshWithinArea_ComponentsToCheckIfIsWithinInfo[i].componentToCheckIfIsWithin_WorldTransform.SetRotation(UKismetMathLibrary::TransformRotation(calculateColorsInfo_Global.vertexPaintComponent->GetComponentTransform(), calculateColorsInfo_Global.paintOnMeshWithinArea_ComponentsToCheckIfIsWithinInfo[i].componentToCheckIfIsWithin_RotationInMeshComponentToPaintSpace).Quaternion());

			calculateColorsInfo_Global.paintOnMeshWithinArea_ComponentsToCheckIfIsWithinInfo[i].componentToCheckIfIsWithin_WorldTransform.SetScale3D(UKismetMathLibrary::TransformLocation(calculateColorsInfo_Global.vertexPaintComponent->GetComponentTransform(), calculateColorsInfo_Global.paintOnMeshWithinArea_ComponentsToCheckIfIsWithinInfo[i].componentToCheckIfIsWithin_ScaleInMeshComponentToPaintSpace));


			auto compsToCheckWithinTemp = calculateColorsInfo_Global.paintOnMeshWithinArea_ComponentsToCheckIfIsWithinInfo[i];


			// Always sets area of effect, so if we have a areaOfEffectWithFullColor set it will work even if there's no falloff. 

			if (compsToCheckWithinTemp.paintWithinAreaShape == Enum_PaintWithinAreaShape::isComplexShape) {

				// Complex Shape demands that the component still exists as we use it's component locatino and trace for it etc. 
				if (!IsValid(compsToCheckWithinTemp.componentToCheckIfIsWithin)) return EVertexPaintDetectionTaskResult::FailWithInvalidMesh;


				// Ignores self as we only want to check if hit on componentToCheckIfIsWithin
				isVertexWithinArea_ComplexShape_TraceParams_Global.AddIgnoredActor(calculateColorsInfo_Global.paintOnMeshWithinAreaSettings.actor);


				// When Task finally starts and complex, we updates world transform based on where the component is now, since with complex we're dependent on the component actually existing since we're tracing for it
				calculateColorsInfo_Global.paintOnMeshWithinArea_ComponentsToCheckIfIsWithinInfo[i].componentToCheckIfIsWithin_WorldTransform = compsToCheckWithinTemp.componentToCheckIfIsWithin->GetComponentTransform();


				if (compsToCheckWithinTemp.componentToCheckIfIsWithin_IsSkeletalMeshComponent) {

					// If Skel Mesh and complex shape we can be dependent on the component still existing and do a cast
					if (auto paintWithinArea_skelMeshComp_Local = Cast<USkeletalMeshComponent>(compsToCheckWithinTemp.componentToCheckIfIsWithin)) {

						if (calculateColorsInfo_Global.paintOnMeshWithinAreaSettings.falloffType == EVertexPaintWithinAreaFallOffType::SphericalFromCenter) {

							paintWithinArea_AreaOfEffects_Local[i] = (compsToCheckWithinTemp.componentToCheckIfIsWithin_Bounds.GetBox().GetExtent() * calculateColorsInfo_Global.paintOnMeshWithinArea_ComponentsToCheckIfIsWithinInfo[i].componentToCheckIfIsWithin->GetComponentScale()).Size();
						}

						else if (calculateColorsInfo_Global.paintOnMeshWithinAreaSettings.falloffType == EVertexPaintWithinAreaFallOffType::GradiantUpward || calculateColorsInfo_Global.paintOnMeshWithinAreaSettings.falloffType == EVertexPaintWithinAreaFallOffType::GradiantDownward) {

							paintWithinArea_AreaOfEffects_Local[i] = (compsToCheckWithinTemp.componentToCheckIfIsWithin_Bounds.GetBox().GetExtent().Z * 2) * calculateColorsInfo_Global.paintOnMeshWithinArea_ComponentsToCheckIfIsWithinInfo[i].componentToCheckIfIsWithin->GetComponentScale().Z;
						}
					}
				}

				else if (compsToCheckWithinTemp.componentToCheckIfIsWithin_IsStaticMeshComponent) {

					if (calculateColorsInfo_Global.paintOnMeshWithinAreaSettings.falloffType == EVertexPaintWithinAreaFallOffType::SphericalFromCenter) {

						paintWithinArea_AreaOfEffects_Local[i] = (calculateColorsInfo_Global.paintOnMeshWithinArea_ComponentsToCheckIfIsWithinInfo[i].componentToCheckIfIsWithin_CleanAggGeomBounds.GetBox().GetExtent() * calculateColorsInfo_Global.paintOnMeshWithinArea_ComponentsToCheckIfIsWithinInfo[i].componentToCheckIfIsWithin_WorldTransform.GetScale3D()).Size();
					}

					else if (calculateColorsInfo_Global.paintOnMeshWithinAreaSettings.falloffType == EVertexPaintWithinAreaFallOffType::GradiantUpward || calculateColorsInfo_Global.paintOnMeshWithinAreaSettings.falloffType == EVertexPaintWithinAreaFallOffType::GradiantDownward) {

						paintWithinArea_AreaOfEffects_Local[i] = (calculateColorsInfo_Global.paintOnMeshWithinArea_ComponentsToCheckIfIsWithinInfo[i].componentToCheckIfIsWithin_CleanAggGeomBounds.GetBox().GetExtent().Z * 2) * calculateColorsInfo_Global.paintOnMeshWithinArea_ComponentsToCheckIfIsWithinInfo[i].componentToCheckIfIsWithin_WorldTransform.GetScale3D().Z;
					}
				}

				else if (compsToCheckWithinTemp.componentToCheckIfIsWithin_IsSplineMeshComponent) {

					if (calculateColorsInfo_Global.paintOnMeshWithinAreaSettings.falloffType == EVertexPaintWithinAreaFallOffType::SphericalFromCenter) {

						// Can't calculate AoE based off the componentToCheckIfIsWithin_Bounds since they only cover each indvidual part. This covers all of them 
						paintWithinArea_AreaOfEffects_Local[i] = compsToCheckWithinTemp.componentToCheckIfIsWithin_ActorBounds_Extent.Size();
					}

					else if (calculateColorsInfo_Global.paintOnMeshWithinAreaSettings.falloffType == EVertexPaintWithinAreaFallOffType::GradiantUpward || calculateColorsInfo_Global.paintOnMeshWithinAreaSettings.falloffType == EVertexPaintWithinAreaFallOffType::GradiantDownward) {

						paintWithinArea_AreaOfEffects_Local[i] = compsToCheckWithinTemp.componentToCheckIfIsWithin_ActorBounds_Extent.Z * 2;
					}
				}
			}

			else {

				// Caches box comp if it is one, so we later can get the extents without casting.
				if (calculateColorsInfo_Global.paintOnMeshWithinArea_ComponentsToCheckIfIsWithinInfo[i].componentToCheckIfIsWithin_IsBoxComponent) {

					if (compsToCheckWithinTemp.paintWithinAreaShape == Enum_PaintWithinAreaShape::isSquareOrRectangleShape) {

						if (calculateColorsInfo_Global.paintOnMeshWithinAreaSettings.falloffType == EVertexPaintWithinAreaFallOffType::SphericalFromCenter) {

							paintWithinArea_AreaOfEffects_Local[i] = calculateColorsInfo_Global.paintOnMeshWithinArea_ComponentsToCheckIfIsWithinInfo[i].componentToCheckIfIsWithin_ScaledBoxExtent.Size();
						}

						else if (calculateColorsInfo_Global.paintOnMeshWithinAreaSettings.falloffType == EVertexPaintWithinAreaFallOffType::GradiantUpward || calculateColorsInfo_Global.paintOnMeshWithinAreaSettings.falloffType == EVertexPaintWithinAreaFallOffType::GradiantDownward) {

							paintWithinArea_AreaOfEffects_Local[i] = calculateColorsInfo_Global.paintOnMeshWithinArea_ComponentsToCheckIfIsWithinInfo[i].componentToCheckIfIsWithin_IsVertexWithinArea_CubeOrRectangle_SquareZHalfSize * 2;
						}
					}
				}

				// Caches sphere comp so we can use it's radius later without casting
				else if (calculateColorsInfo_Global.paintOnMeshWithinArea_ComponentsToCheckIfIsWithinInfo[i].componentToCheckIfIsWithin_IsSphereComponent) {

					if (compsToCheckWithinTemp.paintWithinAreaShape == Enum_PaintWithinAreaShape::isSphereShape) {

						paintWithinArea_AreaOfEffects_Local[i] = calculateColorsInfo_Global.paintOnMeshWithinArea_ComponentsToCheckIfIsWithinInfo[i].componentToCheckIfIsWithin_IsVertexWithinArea_SphereShape;
					}
				}

				else if (calculateColorsInfo_Global.paintOnMeshWithinArea_ComponentsToCheckIfIsWithinInfo[i].componentToCheckIfIsWithin_IsStaticMeshComponent) {

					switch (compsToCheckWithinTemp.paintWithinAreaShape) {

						// Length from centre to corner becomes the area of effect for squares and rectangles, so if you're in a corner of one you're at the very edge of the AoE
					case Enum_PaintWithinAreaShape::isSquareOrRectangleShape:

						if (calculateColorsInfo_Global.paintOnMeshWithinAreaSettings.falloffType == EVertexPaintWithinAreaFallOffType::SphericalFromCenter) {

							// This gives the same result as calculating the distance to the corners using pythagoras. 
							paintWithinArea_AreaOfEffects_Local[i] = (calculateColorsInfo_Global.paintOnMeshWithinArea_ComponentsToCheckIfIsWithinInfo[i].componentToCheckIfIsWithin_CleanAggGeomBounds.GetBox().GetExtent() * calculateColorsInfo_Global.paintOnMeshWithinArea_ComponentsToCheckIfIsWithinInfo[i].componentToCheckIfIsWithin_WorldTransform.GetScale3D()).Size();
						}

						else if (calculateColorsInfo_Global.paintOnMeshWithinAreaSettings.falloffType == EVertexPaintWithinAreaFallOffType::GradiantUpward || calculateColorsInfo_Global.paintOnMeshWithinAreaSettings.falloffType == EVertexPaintWithinAreaFallOffType::GradiantDownward) {

							paintWithinArea_AreaOfEffects_Local[i] = calculateColorsInfo_Global.paintOnMeshWithinArea_ComponentsToCheckIfIsWithinInfo[i].componentToCheckIfIsWithin_IsVertexWithinArea_CubeOrRectangle_SquareZHalfSize * 2;
						}

						break;

						// For spheres just use the radie as area of effect
					case Enum_PaintWithinAreaShape::isSphereShape:

						paintWithinArea_AreaOfEffects_Local[i] = calculateColorsInfo_Global.paintOnMeshWithinArea_ComponentsToCheckIfIsWithinInfo[i].componentToCheckIfIsWithin_IsVertexWithinArea_SphereShape * calculateColorsInfo_Global.paintOnMeshWithinArea_ComponentsToCheckIfIsWithinInfo[i].componentToCheckIfIsWithin_RelativeScale.X;


						break;

					default:
						break;
					}
				}
			}

			paintWithinArea_AreaOfEffects_Local[i] += calculateColorsInfo_Global.paintOnMeshWithinAreaSettings.extraExtentToApplyPaintOn;

			// UE_LOG(LogTemp, Warning, TEXT("Aoe: %f"), paintWithinArea_AreaOfEffects_Local[i]);
		}
	}

	if (calculateColorsInfo_Global.detectInfoSettings.actor) {

		detectGetAverageColor_Local = calculateColorsInfo_Global.detectInfoSettings.getAverageColorInAreaSettings;
		hitLocation_WorldSpace_Detection_Local = UKismetMathLibrary::TransformLocation(calculateColorsInfo_Global.vertexPaintComponent->GetComponentTransform(), calculateColorsInfo_Global.detectInfoSettings.hitFundementals.hitLocationInComponentSpace);
		hitNormal_Detection_Local = calculateColorsInfo_Global.detectInfoSettings.hitFundementals.hitNormal;
	}

	if (calculateColorsInfo_Global.paintOnMeshAtLocationSettings.actor) {

		if (calculateColorsInfo_Global.vertexPaintStaticMeshComponent) {

			vertexNormalMinDotToHitNormalToGetColored_MinDot_Local = calculateColorsInfo_Global.paintOnMeshAtLocationSettings.paintAtAreaSettings.VertexNormalToHitNormal_MinimumDotProductToGetPainted_StaticMesh;
		}

		else if (calculateColorsInfo_Global.vertexPaintSkelComponent) {

			vertexNormalMinDotToHitNormalToGetColored_MinDot_Local = calculateColorsInfo_Global.paintOnMeshAtLocationSettings.paintAtAreaSettings.VertexNormalToHitNormal_MinimumDotProductToGetPainted_SkeletalMesh;
		}

		hitLocation_WorldSpace_Paint_Local = UKismetMathLibrary::TransformLocation(calculateColorsInfo_Global.vertexPaintComponent->GetComponentTransform(), calculateColorsInfo_Global.paintOnMeshAtLocationSettings.hitFundementals.hitLocationInComponentSpace);
	}


	if (calculateColorsInfo_Global.paintOnMeshColorSettings.actor) {

		originalApplyVertexColorSettings = calculateColorsInfo_Global.paintOnMeshColorSettings.applyVertexColorSettings;

		if (calculateColorsInfo_Global.paintOnMeshColorSettings.limitPaintColor.usePaintLimits) {

			calculateColorsResultMessage.Add(FString::Printf(TEXT("Runtime Vertex Color Paint and Detection Plugin - Paint Job is set to Limit Paint Color on R: %f, G: %f, B: %f, A: %f"), calculateColorsInfo_Global.paintOnMeshColorSettings.limitPaintColor.paintLimitOnRedChannel, calculateColorsInfo_Global.paintOnMeshColorSettings.limitPaintColor.paintLimitOnGreenChannel, calculateColorsInfo_Global.paintOnMeshColorSettings.limitPaintColor.paintLimitOnBlueChannel, calculateColorsInfo_Global.paintOnMeshColorSettings.limitPaintColor.paintLimitOnAlphaChannel));
		}
	}


	// Sets it to be the amount of loops we've run through, in case it was stored with 8 loops, but was overriden to only run 2 loops, then we don't want it to be 8 loops after this but only 2 so only 2 gets changed in the subsystem. 
	vertexDetectMeshData_Global.meshDataPerLOD.SetNum(lodsToLoopThrough_Local, true);
	completeLODsBaseBounds_Local.SetNum(lodsToLoopThrough_Local, false);


	for (int currentLOD_Local = 0; currentLOD_Local < lodsToLoopThrough_Local; currentLOD_Local++) {

		currentLODVertex_Local = 0;
		int meshLODTotalAmountOfVerts_Local = 0;
		float closestDistanceToClosestVertex_Paint_Local = 1000000000;
		float closestDistanceToClosestVertex_Detect_Local = 1000000000;
		getEstimatedColorAtHitLocation_Local = false;
		int closestVertexFound_Paint_Temp = -1;

		TArray<int32> sectionsToLoopThrough_Local;
		int sectionMaxAmountOfVerts_Local = 0;

		TArray<FColor> defaultColorFromLOD_Local;
		TArray<FColor> colorFromLOD_Local;
		TArray<FVector> positionsInWorldSpaceFromLOD_Local;
		TArray<FVector> positionsInComponentLocalSpaceFromLOD_Local;
		TArray<FVector> normalsFromLOD_Local;
		FPositionVertexBuffer* posVertBufferAtLOD_Local = nullptr;
		FStaticMeshVertexBuffer* meshVertBufferAtLOD_Local = nullptr;
		FString serializedColorData_Local;

		// Paint Entire Mesh at Random Vertices
		FRandomStream paintEntireMesh_Random_Stream_Local;
		float paintEntireMesh_Random_Probability_Local = 0;
		bool paintEntireMesh_PaintCurrentVertex_Local = false;

		// Propogating to LODs 
		FBox currentLODBaseBounds_Local;
		FPaintedVertex paintedVertex_Local;


		// Necessary in case you stopped playing while tasks being calculated, got crash otherwise on Merged Skeletal Meshes otherwise. 
		if (IsTaskValid(currentLOD_Local) != EVertexPaintDetectionTaskResult::Success) return IsTaskValid(currentLOD_Local);


		if (IsValid(calculateColorsInfo_Global.vertexPaintStaticMeshComponent)) {

			// Adds a section for each one that exists. We can also get the first vertex index of each one and the material using the material index
			for (int32 i = 0; i < calculateColorsInfo_Global.vertexPaintStaticMeshComponent->GetStaticMesh()->GetRenderData()->LODResources[currentLOD_Local].Sections.Num(); i++) {

				sectionsToLoopThrough_Local.Add(i);

				// Can get the Min and Max Vertex Index, as well as the Material of this Section
				// UE_LOG(LogTemp, Warning, TEXT("LOD: %i  -  MinVertexIndex: %i  -  MaxVertexIndex: %i   -  Material Index: %i  "), currentLOD_Local, calculateColorsInfo_Global.vertexPaintStaticMeshComponent->GetStaticMesh()->GetRenderData()->LODResources[currentLOD_Local].Sections[i].MinVertexIndex, calculateColorsInfo_Global.vertexPaintStaticMeshComponent->GetStaticMesh()->GetRenderData()->LODResources[currentLOD_Local].Sections[i].MaxVertexIndex, calculateColorsInfo_Global.vertexPaintStaticMeshComponent->GetStaticMesh()->GetRenderData()->LODResources[currentLOD_Local].Sections[i].MaterialIndex);
			}


			posVertBufferAtLOD_Local = &calculateColorsInfo_Global.vertexPaintStaticMeshComponent->GetStaticMesh()->GetRenderData()->LODResources[currentLOD_Local].VertexBuffers.PositionVertexBuffer;
			meshVertBufferAtLOD_Local = &calculateColorsInfo_Global.vertexPaintStaticMeshComponent->GetStaticMesh()->GetRenderData()->LODResources[currentLOD_Local].VertexBuffers.StaticMeshVertexBuffer;
			meshLODTotalAmountOfVerts_Local = posVertBufferAtLOD_Local->GetNumVertices();

			normalsFromLOD_Local.SetNum(meshLODTotalAmountOfVerts_Local, true);
			colorFromLOD_Local.SetNum(meshLODTotalAmountOfVerts_Local, true);


			// Only sets colors in vertexDetectMeshData_Global if there isn't already anythnig in it. For example if this is a paint / detect job that was next in a queue, so this got set from the result of the previous paint job. Then we already have the latest colors and don't want to run this. If this task wants to paint on more LODs than previous and is CPU Access then it will also get the colors here. Also, if this is run after for isntance Paint Color Snippet, which sets vertexDetectMeshData_Global colors, then we also will use those. 
			if (vertexDetectMeshData_Global.meshDataPerLOD[currentLOD_Local].meshVertexColorsPerLODArray.Num() <= 0) {

				colorFromLOD_Local = UVertexPaintFunctionLibrary::VertexPaintDetectionPlugin_GetStaticMeshVertexColorsAtLOD(calculateColorsInfo_Global.vertexPaintStaticMeshComponent, currentLOD_Local);

				vertexDetectMeshData_Global.meshDataPerLOD[currentLOD_Local].meshVertexColorsPerLODArray = colorFromLOD_Local;
			}

			else {

				colorFromLOD_Local = vertexDetectMeshData_Global.meshDataPerLOD[currentLOD_Local].meshVertexColorsPerLODArray;
			}
		}

		// If skel mesh
		else if (IsValid(calculateColorsInfo_Global.vertexPaintSkelComponent)) {


			if (!calculateColorsInfo_Global.vertexPaintSkelComponent->GetSkeletalMeshRenderData()->LODRenderData.IsValidIndex(currentLOD_Local))
				continue;


			meshLODTotalAmountOfVerts_Local = calculateColorsInfo_Global.vertexPaintSkelComponent->GetSkeletalMeshRenderData()->LODRenderData[currentLOD_Local].GetNumVertices();
			vertexDetectMeshData_Global.meshDataPerLOD[currentLOD_Local].lod = currentLOD_Local;

			// Allocated the normals if we know we're gonna be able to fill it up
			normalsFromLOD_Local.SetNum(meshLODTotalAmountOfVerts_Local, true);


			USkinnedMeshComponent* skinnedMeshComp_Local = calculateColorsInfo_Global.vertexPaintSkelComponent;


#if ENGINE_MAJOR_VERSION == 4

			// If it has a Master Component then we had to use that when calling CacheRefToLocalMatrices, otherwise we got a crash
			if (calculateColorsInfo_Global.vertexPaintSkelComponent->MasterPoseComponent.Get())
				skinnedMeshComp_Local = calculateColorsInfo_Global.vertexPaintSkelComponent->MasterPoseComponent.Get();

#elif ENGINE_MAJOR_VERSION == 5

#if ENGINE_MINOR_VERSION == 0

			// If it has a Master Component then we had to use that when calling CacheRefToLocalMatrices, otherwise we got a crash
			if (calculateColorsInfo_Global.vertexPaintSkelComponent->MasterPoseComponent.Get())
				skinnedMeshComp_Local = calculateColorsInfo_Global.vertexPaintSkelComponent->MasterPoseComponent.Get();


#else

			// If it has a Master Component then we had to use that when calling CacheRefToLocalMatrices, otherwise we got a crash
			if (calculateColorsInfo_Global.vertexPaintSkelComponent->LeaderPoseComponent.Get())
				skinnedMeshComp_Local = calculateColorsInfo_Global.vertexPaintSkelComponent->LeaderPoseComponent.Get();

#endif

#endif


			skinnedMeshComp_Local->CacheRefToLocalMatrices(refToLocals_Local);


			// Only sets colors in vertexDetectMeshData_Global if there isn't already anythnig in it. For example if this is a paint / detect job that was next in a queue, so this got set from the result of the previous paint job. Then we already have the latest colors and don't want to run this. If this task wants to paint on more LODs than previous and is CPU Access then it will also get the colors here. Also, if this is run after for isntance Paint Color Snippet, which sets vertexDetectMeshData_Global colors, then we also will use those. 
			if (vertexDetectMeshData_Global.meshDataPerLOD[currentLOD_Local].meshVertexColorsPerLODArray.Num() <= 0) {

				colorFromLOD_Local = UVertexPaintFunctionLibrary::VertexPaintDetectionPlugin_GetSkeletalMeshVertexColorsAtLOD(calculateColorsInfo_Global.vertexPaintSkelComponent, currentLOD_Local);

				vertexDetectMeshData_Global.meshDataPerLOD[currentLOD_Local].meshVertexColorsPerLODArray = colorFromLOD_Local;
			}

			else {

				colorFromLOD_Local = vertexDetectMeshData_Global.meshDataPerLOD[currentLOD_Local].meshVertexColorsPerLODArray;
			}

			vertexDetectMeshData_Global.meshDataPerLOD[currentLOD_Local].lod = currentLOD_Local;


			for (int j = 0; j < calculateColorsInfo_Global.vertexPaintSkelComponent->GetSkeletalMeshRenderData()->LODRenderData[currentLOD_Local].RenderSections.Num(); j++) {


				// If Get Closest Vertex Data and doesn't have to loop through all vertices and LODs, then we can just loop through the section of the hit bone and not the others so the task is much quicker. 
				if (calculateColorsInfo_Global.detectInfoSettings.actor && !calculateColorsInfo_Global.getAllVertexColorsSettings.actor && calculateColorsInfo_Global.detectInfoSettings.hitFundementals.hitBone != "None" && !ShouldLoopThroughVerticesAndLODs(calculateColorsInfo_Global.fundementalStruct.callbackSettings, calculateColorsInfo_Global.vertexPaintComponent)) {

					bool gotSectionRelatedToBone_Local = false;

					for (auto boneTemp : calculateColorsInfo_Global.vertexPaintSkelComponent->GetSkeletalMeshRenderData()->LODRenderData[currentLOD_Local].RenderSections[j].BoneMap) {


						if (skelMeshTemp->GetSkeleton()->GetReferenceSkeleton().GetBoneName(boneTemp) == calculateColorsInfo_Global.detectInfoSettings.hitFundementals.hitBone) {

							// Only adds the section with the bone then bails out of the loops. 
							sectionsToLoopThrough_Local.Add(j);

							gotSectionRelatedToBone_Local = true;

							// Updates so we don't start at 0 but at this sections start vertex index. 
							currentLODVertex_Local = calculateColorsInfo_Global.vertexPaintSkelComponent->GetSkeletalMeshRenderData()->LODRenderData[currentLOD_Local].RenderSections[j].BaseVertexIndex;

							// UE_LOG(LogTemp, Warning, TEXT("bone to loop through: %s  -  Section: %i  -  Starting at vertex: %i"), *calculateColorsInfo_Global.vertexPaintSkelComponent->SkeletalMesh.Get()->Skeleton.Get()->GetReferenceSkeleton().GetBoneName(boneTemp).ToString(), j, currentLODVertex_Local);

							break;
						}
					}

					// Breaks the section loop as well, since if this was run it means it's not set to include colors of bones so doesn't need to run that below
					if (gotSectionRelatedToBone_Local)
						break;
				}

				else {

					sectionsToLoopThrough_Local.Add(j);
				}


				if (calculateColorsInfo_Global.fundementalStruct.callbackSettings.includeColorsOfEachBone) {

					for (auto boneTemp : calculateColorsInfo_Global.vertexPaintSkelComponent->GetSkeletalMeshRenderData()->LODRenderData[currentLOD_Local].RenderSections[j].BoneMap) {

						// Can increase the array of array colors but not shrink it, so we will cover all bone indexes. 
						if (boneTemp >= (boneIndexColors_Local.Num())) {

							boneTemp++;
							boneIndexColors_Local.SetNum(boneTemp, false);
						}
					}
				}
			}
		}


		if (colorFromLOD_Local.Num() <= 0) {

			calculateColorsResultMessage.Add(FString::Printf(TEXT("Runtime Vertex Color Paint and Detection Plugin - Calculate Colors Task Fail - Couldn't get colors for Actor: %s and Mesh: %s on LOD: %i!"), *calculateColorsInfo_Global.vertexPaintDetectionHitActor->GetName(), *calculateColorsInfo_Global.vertexPaintComponent->GetName(), currentLOD_Local));

			// If couldn't even get the colors on LOD 0 then something has gone wrong, then we shouldn't be in this function in the first place
			if (currentLOD_Local == 0)
				return EVertexPaintDetectionTaskResult::FailWithInvalidMesh;
			else
				continue;
		}

		if (colorFromLOD_Local.Num() != meshLODTotalAmountOfVerts_Local) {

			calculateColorsResultMessage.Add(FString::Printf(TEXT("Runtime Vertex Color Paint and Detection Plugin - Calculate Colors Task Fail - Colors Vertex Amount we got for Actor: %s and Mesh: %s on LOD: %i wasn't the same amount as the Position Vertex Amount. Most likely we're somehow painting on a Mesh component that got it's mesh changed but still has the old color buffer. "), *calculateColorsInfo_Global.vertexPaintDetectionHitActor->GetName(), *calculateColorsInfo_Global.vertexPaintComponent->GetName(), currentLOD_Local));

			// If couldn't even get the colors on LOD 0 then something has gone wrong, then we shouldn't be in this function in the first place
			if (currentLOD_Local == 0)
				return EVertexPaintDetectionTaskResult::FailWithInvalidMesh;
			else
				continue;
		}

		vertexDetectMeshData_Global.meshDataPerLOD[currentLOD_Local].amountOfVerticesAtLOD = meshLODTotalAmountOfVerts_Local;
		defaultColorFromLOD_Local = colorFromLOD_Local;
		positionsInWorldSpaceFromLOD_Local.SetNum(meshLODTotalAmountOfVerts_Local, true);
		positionsInComponentLocalSpaceFromLOD_Local.SetNum(meshLODTotalAmountOfVerts_Local, true);


		if (IsValid(calculateColorsInfo_Global.vertexPaintSettings.actor)) {

			// If haven't been init earlier
			if (newColorVertexBufferPerLOD_Global.IsValidIndex(currentLOD_Local)) {

				if (!newColorVertexBufferPerLOD_Global[currentLOD_Local]) {

					newColorVertexBufferPerLOD_Global[currentLOD_Local] = new FColorVertexBuffer();

					// newColorVertexBufferPerLOD_Global[currentLOD_Local]->Init(meshLODTotalAmountOfVerts_Local, true);

					// Will have to InitFromColorArray in case we Skip a Section, which can happen if Paint using physics surface fails to get any colors, and there's no reason for us to loop through all the vertices. The Tasks seem to take the same amount of time with InitFromColorArray vs Init anyways so. 
					newColorVertexBufferPerLOD_Global[currentLOD_Local]->InitFromColorArray(colorFromLOD_Local);
				}
			}
		}



		if (calculateColorsInfo_Global.paintOnEntireMeshSettings.actor && (calculateColorsInfo_Global.paintOnEntireMeshSettings.paintOnRandomVerticesSettings.onlyRandomizeWithinAreaOfEffectAtLocation || (calculateColorsInfo_Global.paintOnEntireMeshSettings.paintOnRandomVerticesSettings.paintAtRandomVerticesSpreadOutOverTheEntireMesh && calculateColorsInfo_Global.paintOnEntireMeshSettings.paintOnRandomVerticesSettings.paintAtRandomVerticesSpreadOutOverTheEntireMesh_PercentToPaint > 0))) {

			// Can override the random seed, which is useful if for instance a server want clients to paint using the same seeds so they get the same random pattern. 
			if (calculateColorsInfo_Global.paintOnEntireMeshSettings.paintOnRandomVerticesSettings.overrideRandomSeeds) {

				paintEntireMesh_Random_Stream_Local = calculateColorsInfo_Global.paintOnEntireMeshSettings.paintOnRandomVerticesSettings.seedToOverrideWith;
			}
			else {

				paintEntireMesh_Random_Stream_Local.GenerateNewSeed();
			}


			calculateColorsInfo_Global.paintOnEntireMeshSettings.paintOnRandomVerticesSettings.randomSeedsUsedInPaintJob = paintEntireMesh_Random_Stream_Local;
			

			if (calculateColorsInfo_Global.paintOnEntireMeshSettings.paintOnRandomVerticesSettings.onlyRandomizeWithinAreaOfEffectAtLocation) {

				// If set to randomize within area then we don't increase the probability depending on how many verts left to randomize, as we're not using the Percent because that's for randomizing a percent over the entire mesh
				paintEntireMesh_Random_AmountOfVerticesLeftToRandomize_Local = colorFromLOD_Local.Num();

				calculateColorsResultMessage.Add(FString::Printf(TEXT("Runtime Vertex Color Paint and Detection Plugin - Paint Entire Mesh Set to Randomize Vertices to Paint Within Area of Effect: %f"), calculateColorsInfo_Global.paintOnEntireMeshSettings.paintOnRandomVerticesSettings.onlyRandomizeWithinAreaOfEffectAtLocation_AreaOfEffect));
			}

			else {

				if (!calculateColorsInfo_Global.propogateLOD0ToAllLODs) {

					paintEntireMesh_Random_AmountOfVerticesLeftToRandomize_Local = UKismetMathLibrary::MapRangeClamped(calculateColorsInfo_Global.paintOnEntireMeshSettings.paintOnRandomVerticesSettings.paintAtRandomVerticesSpreadOutOverTheEntireMesh_PercentToPaint, 0, 100, 0, colorFromLOD_Local.Num());
				}

				// If gonna propogate LOD0 Colors to other LODs then we only randomize on LOD0
				else if (currentLOD_Local == 0) {

					paintEntireMesh_Random_AmountOfVerticesLeftToRandomize_Local = UKismetMathLibrary::MapRangeClamped(calculateColorsInfo_Global.paintOnEntireMeshSettings.paintOnRandomVerticesSettings.paintAtRandomVerticesSpreadOutOverTheEntireMesh_PercentToPaint, 0, 100, 0, colorFromLOD_Local.Num());
				}

				calculateColorsResultMessage.Add(FString::Printf(TEXT("Runtime Vertex Color Paint and Detection Plugin - Paint Entire Mesh Set to Randomize %i Amount of Vertices over the Mesh on LOD: %i   -   Total Amount of Verts: %i"), (int)paintEntireMesh_Random_AmountOfVerticesLeftToRandomize_Local, currentLOD_Local, colorFromLOD_Local.Num()));
			}
		}


		// calculateColorsResultMessage.Add(FString::Printf(TEXT("Runtime Vertex Color Paint and Detection Plugin - CalculateColorToApply() - Amount of Sections to Loop Through: %i"), sectionsToLoopThrough_Local.Num()));

		// For skeletal meshes we loop through sections and verts on sections so we can more optimally get vertex positions using our own Modified GetTypedSkinnedVertexPosition instead of running ComputeSkinnedPositions (which also loops through sections and verts) before this, because then we have to loop through all verts twice. With this solution we only have to loop through sections and vertices once. 

		for (auto currentSection_Local : sectionsToLoopThrough_Local) {

			// Necessary in case you stopped playing while tasks being calculated, got crash otherwise on Merged Skeletal Meshes otherwise. 
			if (IsTaskValid(currentLOD_Local) != EVertexPaintDetectionTaskResult::Success) return IsTaskValid(currentLOD_Local);


			// UE_LOG(LogTemp, Warning, TEXT("Current Section: %i"), currentSection_Local);


			UMaterialInterface* materialAtSection = nullptr;
			bool shouldApplyVertexColors_Local = true;


			if (IsValid(calculateColorsInfo_Global.vertexPaintStaticMeshComponent)) {


				if (currentSection_Local > 0) {

					// Sets this section max amount of verts, not the max amount of loops in total up to this section. So if the section goes between vertex 300 - 600, we will only loop through 300
					sectionMaxAmountOfVerts_Local = calculateColorsInfo_Global.vertexPaintStaticMeshComponent->GetStaticMesh()->GetRenderData()->LODResources[currentLOD_Local].Sections[currentSection_Local].MaxVertexIndex - calculateColorsInfo_Global.vertexPaintStaticMeshComponent->GetStaticMesh()->GetRenderData()->LODResources[currentLOD_Local].Sections[currentSection_Local].MinVertexIndex;
				}
				else {

					// If the very first section then we need to +1 since MaxVertexIndex isn't the total amount of vertices but just the max. For instance if you have a Mesh with 1801 vertices, i.e. 0 - 1800, then MaxVertexIndex would be 1800, so we can't just use that because then we would loop through 0-1799. 
					sectionMaxAmountOfVerts_Local = calculateColorsInfo_Global.vertexPaintStaticMeshComponent->GetStaticMesh()->GetRenderData()->LODResources[currentLOD_Local].Sections[currentSection_Local].MaxVertexIndex + 1;
				}

				// Makes sure the lod vertex is in sync with the section we're on. Necessary in case we skip this section if it's a paint job with apply using physics surface, but it failed and there's no reason to loop through verts. 
				currentLODVertex_Local = calculateColorsInfo_Global.vertexPaintStaticMeshComponent->GetStaticMesh()->GetRenderData()->LODResources[currentLOD_Local].Sections[currentSection_Local].MinVertexIndex;

				// In case the previous section had higher MaxVertIndex, if so it could become -
				sectionMaxAmountOfVerts_Local = FMath::Abs(sectionMaxAmountOfVerts_Local);

				materialAtSection = calculateColorsInfo_Global.vertexPaintStaticMeshComponent->GetMaterial(calculateColorsInfo_Global.vertexPaintStaticMeshComponent->GetStaticMesh()->GetRenderData()->LODResources[currentLOD_Local].Sections[currentSection_Local].MaterialIndex);


				// if (currentLOD_Local == 0)
				//	UE_LOG(LogTemp, Warning, TEXT("Current Section: %i  -  currentLODVertex: %i   -   FirstIndex: %i   -   MinVertexIndex: %i   -    MaxVertexIndex: %i   -   sectionMaxAmountOfVerts_Local: %i"), currentSection_Local, currentLODVertex_Local, calculateColorsInfo_Global.vertexPaintStaticMeshComponent->GetStaticMesh()->GetRenderData()->LODResources[currentLOD_Local].Sections[currentSection_Local].FirstIndex, calculateColorsInfo_Global.vertexPaintStaticMeshComponent->GetStaticMesh()->GetRenderData()->LODResources[currentLOD_Local].Sections[currentSection_Local].MinVertexIndex, calculateColorsInfo_Global.vertexPaintStaticMeshComponent->GetStaticMesh()->GetRenderData()->LODResources[currentLOD_Local].Sections[currentSection_Local].MaxVertexIndex, sectionMaxAmountOfVerts_Local);
			}

			else if (calculateColorsInfo_Global.vertexPaintSkelComponent) {

				// Makes sure the lod vertex is in sync with the section we're on. Necessary in case we skip this section if it's a paint job with apply using physics surface, but it failed and there's no reason to loop through verts
				currentLODVertex_Local = calculateColorsInfo_Global.vertexPaintSkelComponent->GetSkeletalMeshRenderData()->LODRenderData[currentLOD_Local].RenderSections[currentSection_Local].BaseVertexIndex;


				sectionMaxAmountOfVerts_Local = calculateColorsInfo_Global.vertexPaintSkelComponent->GetSkeletalMeshRenderData()->LODRenderData[currentLOD_Local].RenderSections[currentSection_Local].GetNumVertices();

				const FSkeletalMeshLODRenderData& skelMeshRenderData_Local = calculateColorsInfo_Global.vertexPaintSkelComponent->GetSkeletalMeshRenderData()->LODRenderData[currentLOD_Local];

				materialAtSection = calculateColorsInfo_Global.vertexPaintSkelComponent->GetMaterial(skelMeshRenderData_Local.RenderSections[currentSection_Local].MaterialIndex);


				// If invalid section then something have gone so we end the task. There was a very very rare crash later with cloth this seems to fix
				if (!skelMeshRenderData_Local.RenderSections.IsValidIndex(currentSection_Local)) {

					calculateColorsResultMessage.Add(FString::Printf(TEXT("Runtime Vertex Color Paint and Detection Plugin - Async Task Failed because invalid Render Section!")));

					return EVertexPaintDetectionTaskResult::Fail;
				}

				// for each section we check if it's cloth and if so gets pos and normal of the cloth. Sets a local var here for use later on so we don't have to do .FindRef several times
				if (skelMeshRenderData_Local.RenderSections[currentSection_Local].HasClothingData() && currentLOD_Local == calculateColorsInfo_Global.vertexPaintSkelComponent->GetPredictedLODLevel()) {


					currentClothSectionVertexPositionsTemp_Local = skeletalMeshClothSimulData_Position_Local.FindRef(skelMeshRenderData_Local.RenderSections[currentSection_Local].CorrespondClothAssetIndex);

					currentClothSectionVertexNormalsTemp_Local = skeletalMeshClothSimulData_Normals_Local.FindRef(skelMeshRenderData_Local.RenderSections[currentSection_Local].CorrespondClothAssetIndex);
				}
			}


			// UE_LOG(LogTemp, Warning, TEXT("Section Start Vertex Index: %i"), currentLODVertex_Local);


			UMaterialInterface* materialToGetSurfacesFrom = nullptr;

			// If it's registered, then we get whether materialAtSection material, or it's parent (if it's an instance) is registered, so we can you stuff like find what physics surfaces is painted at what channel etc. using this. Otherwise we had an issue if an instance wasn't registered, but the parent was, then we expect to work off of what the parent has. 
			if (UVertexPaintFunctionLibrary::GetVertexPaintMaterialDataAsset() && materialAtSection)
				materialToGetSurfacesFrom = UVertexPaintFunctionLibrary::GetVertexPaintMaterialDataAsset()->GetRegisteredMaterialInstanceOrParentMaterial(materialAtSection);
			else
				materialToGetSurfacesFrom = materialAtSection;


			// Resets to True for each section since by default we want to apply on all channels. It's just the Apply Colors Using Physics Surface, if Painting with Set that has the unique use case where they shouldn't change a channel at all unless it has the physics surface. 
			applyColorsOnRedChannel_Local = true;
			applyColorsOnGreenChannel_Local = true;
			applyColorsOnBlueChannel_Local = true;
			applyColorsOnAlphaChannel_Local = true;


			// If set to Paint Using Physics Surface, and we was successfull at getting the Material, then we Get the Colors to Apply based on what's Registered in the Editor Widget. 
			if (calculateColorsInfo_Global.paintOnMeshColorSettings.applyVertexColorSettings.applyColorsUsingPhysicsSurface.applyVertexColorUsingPhysicsSurface) {


				float redColorToApplyTemp = 0;
				bool redColorToApplySuccessfullyGotTemp = false;

				float greenColorToApplyTemp = 0;
				bool greenColorToApplySuccessfullyGotTemp = false;

				float blueColorToApplyTemp = 0;
				bool blueColorToApplySuccessfullyGotTemp = false;

				float alphaColorToApplyTemp = 0;
				bool alphaColorToApplySuccessfullyGotTemp = false;

				bool successfullyGotVertexColorsToApplyTemp = false;

				UVertexPaintFunctionLibrary::GetColorsToApplyFromPhysicsSurface_Wrapper(materialToGetSurfacesFrom, calculateColorsInfo_Global.paintOnMeshColorSettings.applyVertexColorSettings.applyColorsUsingPhysicsSurface.physicsSurfaceToApply, redColorToApplyTemp, redColorToApplySuccessfullyGotTemp, greenColorToApplyTemp, greenColorToApplySuccessfullyGotTemp, blueColorToApplyTemp, blueColorToApplySuccessfullyGotTemp, alphaColorToApplyTemp, alphaColorToApplySuccessfullyGotTemp, successfullyGotVertexColorsToApplyTemp, calculateColorsInfo_Global.paintOnMeshColorSettings.applyVertexColorSettings.applyColorsUsingPhysicsSurface.paintStrengthToApply, calculateColorsInfo_Global.paintOnMeshColorSettings.applyVertexColorSettings.applyColorsUsingPhysicsSurface.paintStrengtOnChannelsWithoutThePhysicsSurface, calculateColorsInfo_Global.paintOnMeshColorSettings.applyVertexColorSettings.applyColorsUsingPhysicsSurface.applyOnChannelsThatIsAParentOfPhysicsSurface, calculateColorsInfo_Global.paintOnMeshColorSettings.applyVertexColorSettings.applyColorsUsingPhysicsSurface.applyOnChannelsThatsChildOfPhysicsSurface, calculateColorsInfo_Global.paintOnMeshColorSettings.applyVertexColorSettings.applyColorsUsingPhysicsSurface.applyOnChannelsThatResultsInBlendableSurfaces);


				if (successfullyGotVertexColorsToApplyTemp) {

					// If applying with Set, then only want the channels that was successfull, i.e. had the same physics surface to actually Set the colors. 
					if (calculateColorsInfo_Global.paintOnMeshColorSettings.applyVertexColorSettings.applyVertexColorSetting == EApplyVertexColorSetting::ESetVertexColor) {

						applyColorsOnRedChannel_Local = false;
						applyColorsOnGreenChannel_Local = false;
						applyColorsOnBlueChannel_Local = false;
						applyColorsOnAlphaChannel_Local = false;
					}


					// If successfull in getting color to apply on at least 1 channel, it means they all are set to either paintStrengthToApply, or paintStrengtOnChannelsWithoutThePhysicsSurface, either way they should get whatever color that got returned for them
					calculateColorsInfo_Global.paintOnMeshColorSettings.applyVertexColorSettings.redColor = redColorToApplyTemp;
					calculateColorsInfo_Global.paintOnMeshColorSettings.applyVertexColorSettings.greenColor = greenColorToApplyTemp;
					calculateColorsInfo_Global.paintOnMeshColorSettings.applyVertexColorSettings.blueColor = blueColorToApplyTemp;
					calculateColorsInfo_Global.paintOnMeshColorSettings.applyVertexColorSettings.alphaColor = alphaColorToApplyTemp;

					// UE_LOG(LogTemp, Warning, TEXT("Trying to Apply Vertex Color R: %f - G: %f - B: %f - A: %f to Section: %i With Material: %s"), redColorToApplyTemp, greenColorToApplyTemp, blueColorToApplyTemp, alphaColorToApplyTemp, currentSection_Local, *materialAtSection->GetName());



					// If set to Apply Colors using Physics Surface, then sets Red, Green, Blue and Alpha Condition based on what the paintUsingPhysicsSurfaceConditions are, since even if we're set to apply colors using physics surface, we're changing the RGBA colors to apply, so want to change their conditions as well! 
					if (redColorToApplySuccessfullyGotTemp) {

						calculateColorsInfo_Global.paintOnMeshColorSettings.applyVertexColorSettings.redColorCondition = GetPaintConditionAdjustedIfSetToUsePhysicsSurface(calculateColorsInfo_Global.paintOnMeshColorSettings.applyVertexColorSettings.applyColorsUsingPhysicsSurface.paintUsingPhysicsSurfaceConditions, materialToGetSurfacesFrom);

						applyColorsOnRedChannel_Local = true;

						if (!calculateColorsInfo_Global.appliedToVertexColorChannels.Contains(EVertexColorChannel::RedChannel))
							calculateColorsInfo_Global.appliedToVertexColorChannels.Add(EVertexColorChannel::RedChannel);
					}

					// If failed then we set them based on paintConditionsOnChannelsWithoutThePhysicsSurface
					else {

						calculateColorsInfo_Global.paintOnMeshColorSettings.applyVertexColorSettings.redColorCondition = GetPaintConditionAdjustedIfSetToUsePhysicsSurface(calculateColorsInfo_Global.paintOnMeshColorSettings.applyVertexColorSettings.applyColorsUsingPhysicsSurface.paintConditionsOnChannelsWithoutThePhysicsSurface, materialToGetSurfacesFrom);
					}


					if (greenColorToApplySuccessfullyGotTemp) {

						calculateColorsInfo_Global.paintOnMeshColorSettings.applyVertexColorSettings.greenColorCondition = GetPaintConditionAdjustedIfSetToUsePhysicsSurface(calculateColorsInfo_Global.paintOnMeshColorSettings.applyVertexColorSettings.applyColorsUsingPhysicsSurface.paintUsingPhysicsSurfaceConditions, materialToGetSurfacesFrom);

						applyColorsOnGreenChannel_Local = true;

						if (!calculateColorsInfo_Global.appliedToVertexColorChannels.Contains(EVertexColorChannel::GreenChannel))
							calculateColorsInfo_Global.appliedToVertexColorChannels.Add(EVertexColorChannel::GreenChannel);
					}
					else {

						calculateColorsInfo_Global.paintOnMeshColorSettings.applyVertexColorSettings.greenColorCondition = GetPaintConditionAdjustedIfSetToUsePhysicsSurface(calculateColorsInfo_Global.paintOnMeshColorSettings.applyVertexColorSettings.applyColorsUsingPhysicsSurface.paintConditionsOnChannelsWithoutThePhysicsSurface, materialToGetSurfacesFrom);
					}


					if (blueColorToApplySuccessfullyGotTemp) {

						calculateColorsInfo_Global.paintOnMeshColorSettings.applyVertexColorSettings.blueColorCondition = GetPaintConditionAdjustedIfSetToUsePhysicsSurface(calculateColorsInfo_Global.paintOnMeshColorSettings.applyVertexColorSettings.applyColorsUsingPhysicsSurface.paintUsingPhysicsSurfaceConditions, materialToGetSurfacesFrom);

						applyColorsOnBlueChannel_Local = true;

						if (!calculateColorsInfo_Global.appliedToVertexColorChannels.Contains(EVertexColorChannel::BlueChannel))
							calculateColorsInfo_Global.appliedToVertexColorChannels.Add(EVertexColorChannel::BlueChannel);
					}
					else {

						calculateColorsInfo_Global.paintOnMeshColorSettings.applyVertexColorSettings.blueColorCondition = GetPaintConditionAdjustedIfSetToUsePhysicsSurface(calculateColorsInfo_Global.paintOnMeshColorSettings.applyVertexColorSettings.applyColorsUsingPhysicsSurface.paintConditionsOnChannelsWithoutThePhysicsSurface, materialToGetSurfacesFrom);
					}


					if (alphaColorToApplySuccessfullyGotTemp) {

						calculateColorsInfo_Global.paintOnMeshColorSettings.applyVertexColorSettings.alphaColorCondition = GetPaintConditionAdjustedIfSetToUsePhysicsSurface(calculateColorsInfo_Global.paintOnMeshColorSettings.applyVertexColorSettings.applyColorsUsingPhysicsSurface.paintUsingPhysicsSurfaceConditions, materialToGetSurfacesFrom);

						applyColorsOnAlphaChannel_Local = true;

						if (!calculateColorsInfo_Global.appliedToVertexColorChannels.Contains(EVertexColorChannel::AlphaChannel))
							calculateColorsInfo_Global.appliedToVertexColorChannels.Add(EVertexColorChannel::AlphaChannel);
					}
					else {

						calculateColorsInfo_Global.paintOnMeshColorSettings.applyVertexColorSettings.alphaColorCondition = GetPaintConditionAdjustedIfSetToUsePhysicsSurface(calculateColorsInfo_Global.paintOnMeshColorSettings.applyVertexColorSettings.applyColorsUsingPhysicsSurface.paintConditionsOnChannelsWithoutThePhysicsSurface, materialToGetSurfacesFrom);
					}


					// Sets the applyColorsUsingPhysicsSurface.paintUsingPhysicsSurfaceConditions as well, just so it matches, in case we try to use this later. Otherwise this may have an incorrect setting at the vertex channel or something
					calculateColorsInfo_Global.paintOnMeshColorSettings.applyVertexColorSettings.applyColorsUsingPhysicsSurface.paintUsingPhysicsSurfaceConditions = GetPaintConditionAdjustedIfSetToUsePhysicsSurface(calculateColorsInfo_Global.paintOnMeshColorSettings.applyVertexColorSettings.applyColorsUsingPhysicsSurface.paintUsingPhysicsSurfaceConditions, materialToGetSurfacesFrom);
				}

				// If it's not registered or the Material isn't valid, and it's set to only apply colors if it's registered, then we just set the colors to 0, so no difference will occur at least for the section with this particular Material
				else if (!calculateColorsInfo_Global.paintOnMeshColorSettings.applyVertexColorSettings.applyColorsUsingPhysicsSurface.fallbackToRGBAColorsIfFailedToApplyColorsUsingPhysicsSurface) {

					shouldApplyVertexColors_Local = false;

					if (materialToGetSurfacesFrom) {

						calculateColorsResultMessage.Add(FString::Printf(TEXT("Runtime Vertex Color Paint and Detection Plugin - Set to Apply Colors using Physics Surface but GetColorsToApplyFromPhysicsSurface Failed on Material: %s. Make sure Physics Surface Parent/Childs are Setup Correctly, as well as correct Physics Surfaces on the Vertex Colors Channels for the Material. onlyApplyColorsIfPhysicsSurfaceIsRegistered is True so no colors will be changed. "), *materialToGetSurfacesFrom->GetName()));
					}
				}

				// If it's not successfull at getting colors, i.e. not registered and onlyApplyColorsIfPhysicsSurfaceIsRegistered is False, then we Fall Back to the original RGBA colors that the user set as well as whatever conditions that was there!
				else {

					calculateColorsInfo_Global.paintOnMeshColorSettings.applyVertexColorSettings = originalApplyVertexColorSettings;
				}
			}

			else {

				// If not set to paint using physics surface, then we adjust each RGBA Condition based on their own condition and not the condition from the Paint Using Physics Surface Settings.
				calculateColorsInfo_Global.paintOnMeshColorSettings.applyVertexColorSettings.redColorCondition = GetPaintConditionAdjustedIfSetToUsePhysicsSurface(calculateColorsInfo_Global.paintOnMeshColorSettings.applyVertexColorSettings.redColorCondition, materialToGetSurfacesFrom);

				calculateColorsInfo_Global.paintOnMeshColorSettings.applyVertexColorSettings.greenColorCondition = GetPaintConditionAdjustedIfSetToUsePhysicsSurface(calculateColorsInfo_Global.paintOnMeshColorSettings.applyVertexColorSettings.greenColorCondition, materialToGetSurfacesFrom);

				calculateColorsInfo_Global.paintOnMeshColorSettings.applyVertexColorSettings.blueColorCondition = GetPaintConditionAdjustedIfSetToUsePhysicsSurface(calculateColorsInfo_Global.paintOnMeshColorSettings.applyVertexColorSettings.blueColorCondition, materialToGetSurfacesFrom);

				calculateColorsInfo_Global.paintOnMeshColorSettings.applyVertexColorSettings.alphaColorCondition = GetPaintConditionAdjustedIfSetToUsePhysicsSurface(calculateColorsInfo_Global.paintOnMeshColorSettings.applyVertexColorSettings.alphaColorCondition, materialToGetSurfacesFrom);
			}



			// If Paint Job that applies color with using RGBA, i.e. not a color snippet, then checks if we should even apply colors after the applyVertexColorUsingPhysicsSurface, if not, and there's no reason to loop through vertices then we can skip this section since no difference will occur anyway. currentLODVertex_Local gets set earlier to be whatever the min of the section is so it should match the next section we skip to. 
			if (calculateColorsInfo_Global.paintOnMeshColorSettings.actor) {

				// UE_LOG(LogTemp, Warning, TEXT("shouldApplyVertexColors_Local: %i  -  PaintJobShouldLoopThroughVerticesAndLODS: %i"), shouldApplyVertexColors_Local, PaintJobShouldLoopThroughVerticesAndLODS(calculateColorsInfo_Global.paintOnMeshColorSettings));

				if (!shouldApplyVertexColors_Local && !PaintJobShouldLoopThroughVerticesAndLODS(calculateColorsInfo_Global.paintOnMeshColorSettings)) {

					// if (materialAtSection)
					// UE_LOG(LogTemp, Warning, TEXT("skipping section %i with Material: %s"), currentSection_Local, *materialAtSection->GetName());

					continue;
				}
			}


			// currentLODVertex_Local gets ++ at the end of the vert loop

			for (uint32 currentSectionVertex_Local = 0; currentSectionVertex_Local < (uint32)sectionMaxAmountOfVerts_Local; currentSectionVertex_Local++) {


				paintEntireMesh_PaintCurrentVertex_Local = false;
				paintEntireMesh_Random_Probability_Local = 0;
				triedToApplyColorOnVertex_Local = false;
				currentClothingAssetBase_Local = nullptr;
				paintedVertex_Local = FPaintedVertex();


				// Necessary in case you stopped playing while tasks being calculated, got crash otherwise on Merged Skeletal Meshes otherwise
				if (IsTaskValid(currentLOD_Local) != EVertexPaintDetectionTaskResult::Success) return IsTaskValid(currentLOD_Local);

				if (!colorFromLOD_Local.IsValidIndex(currentLODVertex_Local)) return EVertexPaintDetectionTaskResult::Fail;


				if (calculateColorsInfo_Global.vertexPaintStaticMeshComponent) {


#if ENGINE_MAJOR_VERSION == 4

					// if static mesh and cpu access
					if (posVertBufferAtLOD_Local)
						positionsInComponentLocalSpaceFromLOD_Local[currentLODVertex_Local] = posVertBufferAtLOD_Local->VertexPosition(currentLODVertex_Local);


#elif ENGINE_MAJOR_VERSION == 5

					// if static mesh and cpu access
					if (posVertBufferAtLOD_Local)
						positionsInComponentLocalSpaceFromLOD_Local[currentLODVertex_Local] = (FVector)posVertBufferAtLOD_Local->VertexPosition(currentLODVertex_Local);
#endif


					if (meshVertBufferAtLOD_Local)
						normalsFromLOD_Local[currentLODVertex_Local] = FVector(meshVertBufferAtLOD_Local->VertexTangentZ(currentLODVertex_Local).X, meshVertBufferAtLOD_Local->VertexTangentZ(currentLODVertex_Local).Y, meshVertBufferAtLOD_Local->VertexTangentZ(currentLODVertex_Local).Z);


					if (IsValid(calculateColorsInfo_Global.vertexPaintSplineMeshComponent)) {

						positionsInComponentLocalSpaceFromLOD_Local[currentLODVertex_Local] = GetSplineMeshVertexPositionInMeshSpace(positionsInComponentLocalSpaceFromLOD_Local[currentLODVertex_Local], calculateColorsInfo_Global.vertexPaintSplineMeshComponent);

						normalsFromLOD_Local[currentLODVertex_Local] = GetSplineMeshVertexNormalInMeshSpace(normalsFromLOD_Local[currentLODVertex_Local], calculateColorsInfo_Global.vertexPaintSplineMeshComponent);
					}
				}

				else if (calculateColorsInfo_Global.vertexPaintSkelComponent) {


					const FSkeletalMeshLODRenderData& skelMeshRenderData_Local = calculateColorsInfo_Global.vertexPaintSkelComponent->GetSkeletalMeshRenderData()->LODRenderData[currentLOD_Local];

					FSkinWeightVertexBuffer& skinWeightBuffer_Local = *calculateColorsInfo_Global.vertexPaintSkelComponent->GetSkinWeightBuffer(currentLOD_Local);

#if ENGINE_MAJOR_VERSION == 4


					// skelMeshRenderData_Local.StaticVertexBuffers.StaticMeshVertexBuffer.GetResourceSize()

					normalsFromLOD_Local[currentLODVertex_Local] = skelMeshRenderData_Local.StaticVertexBuffers.StaticMeshVertexBuffer.VertexTangentZ(currentLODVertex_Local);

					// 100% crash on ln 217 on StaticMeshVertexBuffer with this
					// normalsFromLOD_Local[currentLODVertex_Local] = skelMeshRenderData_Local.StaticVertexBuffers.StaticMeshVertexBuffer.VertexTangentZ_Typed<EStaticMeshVertexTangentBasisType::HighPrecision>(currentLODVertex_Local);


#elif ENGINE_MAJOR_VERSION == 5

					normalsFromLOD_Local[currentLODVertex_Local] = (FVector)skelMeshRenderData_Local.StaticVertexBuffers.StaticMeshVertexBuffer.VertexTangentZ(currentLODVertex_Local);
#endif


					// If cloth, then gets and adjust position from cloth simul data
					if (skelMeshRenderData_Local.RenderSections[currentSection_Local].HasClothingData() && currentLOD_Local == calculateColorsInfo_Global.vertexPaintSkelComponent->GetPredictedLODLevel()) {

						if (!skelMeshRenderData_Local.ClothVertexBuffer.IsInitialized()) {

							calculateColorsResultMessage.Add(FString::Printf(TEXT("Runtime Vertex Color Paint and Detection Plugin - Async Task Failed skeletal mesh with cloth's cloth vertex buffer isn't initialized")));

							return EVertexPaintDetectionTaskResult::Fail;
						}


						if (currentClothSectionVertexPositionsTemp_Local.IsValidIndex(currentSectionVertex_Local) && currentClothSectionVertexNormalsTemp_Local.IsValidIndex(currentSectionVertex_Local)) {


							currentClothingAssetBase_Local = clothingAssets_Local[skelMeshRenderData_Local.RenderSections[currentSection_Local].CorrespondClothAssetIndex];


							// This logic copied from ComputeSkinnedPositions do not work, and ComputeSkinnedPositions doesn't work 100% if blend weight is below 1 either, it prints positions as if the blend weight is 1.
							// TODO See if you can fix so if blend weight is < 1, it positions match the cloth
							/*
							if (calculateColorsInfo_Global.vertexPaintSkelComponent->ClothBlendWeight < 1) {

								FVector skinnedPosTemp = CustomGetTypedSkinnedVertexPosition(calculateColorsInfo_Global.vertexPaintSkelComponent, skelMeshRenderData_Local.RenderSections[SectionIdx], skelMeshRenderData_Local.StaticVertexBuffers.PositionVertexBuffer, skinWeightBuffer_Local, currentSectionVertexIndexTemp, refToLocals_LocalTempTest);

								simulPosTemp = (FVector)currentClothSimulData_Local.Positions[currentVertexIndex_Section_Local] * calculateColorsInfo_Global.vertexPaintSkelComponent->ClothBlendWeight + skinnedPosTemp * (1 - calculateColorsInfo_Global.vertexPaintSkelComponent->ClothBlendWeight);
							}
							*/


							positionsInComponentLocalSpaceFromLOD_Local[currentLODVertex_Local] = (FVector)currentClothSectionVertexPositionsTemp_Local[currentSectionVertex_Local];

							normalsFromLOD_Local[currentLODVertex_Local] = (FVector)currentClothSectionVertexNormalsTemp_Local[currentSectionVertex_Local];


							// UE5 with Chaos Cloth required a conversion of the cloth positions and rotation to get them aligned to the bone the cloth is attached to
#if ENGINE_MAJOR_VERSION == 5

							// Updates current in array - Rotates the Vector according to the bone it's attached to
							positionsInComponentLocalSpaceFromLOD_Local[currentLODVertex_Local] = UKismetMathLibrary::Quat_RotateVector(clothBoneQuaternionsInComponentSpace_Local[skelMeshRenderData_Local.RenderSections[currentSection_Local].CorrespondClothAssetIndex], (FVector)currentClothSectionVertexPositionsTemp_Local[currentSectionVertex_Local]);

							// Updates current in array - Since the Vertex Positions we get from Cloth is component 0, 0, 0 we had to add the bone component local here to get it at the right location, so we now have the correct rotation and location for each cloth vertex 
							positionsInComponentLocalSpaceFromLOD_Local[currentLODVertex_Local] += clothBoneLocationInComponentSpace_Local[skelMeshRenderData_Local.RenderSections[currentSection_Local].CorrespondClothAssetIndex];
#endif


							if (calculateColorsInfo_Global.drawClothVertexPositionDebugPoint && !calculateColorsInfo_Global.fundementalStruct.multiThreadSettings.useMultithreadingForCalculations) {

								DrawDebugPoint(calculateColorsInfo_Global.vertexPaintDetectionHitActor->GetWorld(), UKismetMathLibrary::TransformLocation(calculateColorsInfo_Global.vertexPaintDetectionHitActor->GetTransform(), UKismetMathLibrary::InverseTransformLocation(calculateColorsInfo_Global.vertexPaintDetectionHitActor->GetTransform(), UKismetMathLibrary::TransformLocation(calculateColorsInfo_Global.vertexPaintComponent->GetComponentTransform(), positionsInComponentLocalSpaceFromLOD_Local[currentLODVertex_Local]))), 5, FColor::Red, false, calculateColorsInfo_Global.drawVertexesDebugPointsDuration, 0);
							}
						}

						else {

							// If no clothing then gets the position in component space using our own GetTypedSkinnedVertexPosition, which is otherwise called on each vertex if running ComputeSkinnedPositions. The reason why we've made our own here is so we only have to loop through a mesh sections and vertices once, instead of atleast twice if running ComputeSkinnedPositions, since it also loops through sections and vertices, then if we where to use that here we would have to loop through it again. 
							positionsInComponentLocalSpaceFromLOD_Local[currentLODVertex_Local] = Modified_GetTypedSkinnedVertexPosition(calculateColorsInfo_Global.vertexPaintSkelComponent, skelMeshRenderData_Local.RenderSections[currentSection_Local], skelMeshRenderData_Local.StaticVertexBuffers.PositionVertexBuffer, skinWeightBuffer_Local, currentSectionVertex_Local, refToLocals_Local, currentBoneIndex_Local);
						}
					}

					else {

						positionsInComponentLocalSpaceFromLOD_Local[currentLODVertex_Local] = Modified_GetTypedSkinnedVertexPosition(calculateColorsInfo_Global.vertexPaintSkelComponent, skelMeshRenderData_Local.RenderSections[currentSection_Local], skelMeshRenderData_Local.StaticVertexBuffers.PositionVertexBuffer, skinWeightBuffer_Local, currentSectionVertex_Local, refToLocals_Local, currentBoneIndex_Local);
					}


					currentBoneName_Local = calculateColorsInfo_Global.vertexPaintSkelComponent->GetBoneName(currentBoneIndex_Local);
				}


				currentVertexActorSpace = UKismetMathLibrary::InverseTransformLocation(calculateColorsInfo_Global.vertexPaintDetectionHitActor->GetTransform(), UKismetMathLibrary::TransformLocation(calculateColorsInfo_Global.vertexPaintComponent->GetComponentTransform(), positionsInComponentLocalSpaceFromLOD_Local[currentLODVertex_Local]));

				currentVertexWorldSpace = UKismetMathLibrary::TransformLocation(calculateColorsInfo_Global.vertexPaintDetectionHitActor->GetActorTransform(), currentVertexActorSpace);

				positionsInWorldSpaceFromLOD_Local[currentLODVertex_Local] = currentVertexWorldSpace;

				normalsFromLOD_Local[currentLODVertex_Local] = UKismetMathLibrary::TransformDirection(calculateColorsInfo_Global.vertexPaintComponent->GetComponentTransform(), normalsFromLOD_Local[currentLODVertex_Local]);


				// Caches the vertex normal to hit normal dots so it's more comfortable to use later on
				if (calculateColorsInfo_Global.paintOnMeshAtLocationSettings.actor)
					cureentVertexNormalToHitNormalDot_Paint = FMath::Clamp(FVector::DotProduct(calculateColorsInfo_Global.paintOnMeshAtLocationSettings.hitFundementals.hitNormal, normalsFromLOD_Local[currentLODVertex_Local]), -1.0f, 1.0f);

				if (calculateColorsInfo_Global.detectInfoSettings.actor)
					cureentVertexNormalToHitNormalDot_Detect = FMath::Clamp(FVector::DotProduct(hitNormal_Detection_Local, normalsFromLOD_Local[currentLODVertex_Local]), -1.0f, 1.0f);


				if (currentLOD_Local == 0 && updateMeshVertexColorData && vertexDetectMeshData_Global.meshDataPerLOD.Num() > 1 && calculateColorsInfo_Global.propogateLOD0ToAllLODs) {

					paintedVertex_Local.Position = positionsInComponentLocalSpaceFromLOD_Local[currentLODVertex_Local];
					paintedVertex_Local.Normal = normalsFromLOD_Local[currentLODVertex_Local];

					currentLODBaseBounds_Local += positionsInComponentLocalSpaceFromLOD_Local[currentLODVertex_Local];
				}


				if (calculateColorsInfo_Global.paintOnEntireMeshSettings.actor) {

					// If set to randomize over the entire mesh
					if (calculateColorsInfo_Global.paintOnEntireMeshSettings.paintOnRandomVerticesSettings.paintAtRandomVerticesSpreadOutOverTheEntireMesh || calculateColorsInfo_Global.paintOnEntireMeshSettings.paintOnRandomVerticesSettings.onlyRandomizeWithinAreaOfEffectAtLocation) {


						// This array only gets filled if Motified Engine Method. If this vertex is a doublette of another that is on the Exact same position, then we want it to also get painted, so all doublettes at a position have the same color. This is so when we run our calculation so LOD1, 2 vertices get the same color as their closest and best LOD0 vertex, it shouldn't matter which of the doublettes the calculation chooses to be the best, as they all have the same color. Otherwise we had an issue where other LODs wouldn't get LOD0 color if using modified engine method since the result of it can be that the closest LOD0 vertex color to use for a LOD1 vertex could be a doublette of the randomized one that was on the exact same location
						if (currentLOD_Local == 0 && paintEntireMeshRandom_VerticesDoublettesPaintedAtLOD0_Local.Contains(positionsInComponentLocalSpaceFromLOD_Local[currentLODVertex_Local])) {

							paintEntireMesh_PaintCurrentVertex_Local = true;
						}

						// AmountOfVerticesLeftToRandomize_Local will reset if Not to propogate to other LODs, i.e. do a complete randomization for each LOD
						else if (paintEntireMesh_Random_AmountOfVerticesLeftToRandomize_Local > 0) {

							// For instance 5 left to randomize out of total 100 verts that's left, = 0.05, 5% chance. initial amountOfVerticesLeftToRandomize_Local Will be set depending on the paintAtRandomVerticesSpreadOutOverTheEntireMesh_PercentToPaint
							paintEntireMesh_Random_Probability_Local = paintEntireMesh_Random_AmountOfVerticesLeftToRandomize_Local / (meshLODTotalAmountOfVerts_Local - currentLODVertex_Local);


							// If set to randomize within AoE at Location then checks distance,if too far away then probability drops to 0 as we should never randomizee to them. 
							if (calculateColorsInfo_Global.paintOnEntireMeshSettings.paintOnRandomVerticesSettings.onlyRandomizeWithinAreaOfEffectAtLocation) {

								if ((calculateColorsInfo_Global.paintOnEntireMeshSettings.paintOnRandomVerticesSettings.onlyRandomizeWithinAreaOfEffectAtLocation_Location - currentVertexWorldSpace).Size() > calculateColorsInfo_Global.paintOnEntireMeshSettings.paintOnRandomVerticesSettings.onlyRandomizeWithinAreaOfEffectAtLocation_AreaOfEffect)
									paintEntireMesh_Random_Probability_Local = 0;

								else
									paintEntireMesh_Random_Probability_Local = calculateColorsInfo_Global.paintOnEntireMeshSettings.paintOnRandomVerticesSettings.onlyRandomizeWithinAreaOfEffectAtLocation_ProbabilityFactor;
							}


							// Used to test in combination with only randomizing 1 vert if it got propogated to the other LOD
							// paintEntireMesh_Random_Probability_Local = 1;

							
							if (paintEntireMesh_Random_Probability_Local > 0) {

#if ENGINE_MAJOR_VERSION == 5

#if ENGINE_MINOR_VERSION == 0
								if (UKismetMathLibrary::RandomBoolWithWeightFromStream(paintEntireMesh_Random_Probability_Local, paintEntireMesh_Random_Stream_Local)) {
#elif ENGINE_MINOR_VERSION == 1
								if (UKismetMathLibrary::RandomBoolWithWeightFromStream(paintEntireMesh_Random_Probability_Local, paintEntireMesh_Random_Stream_Local)) {
#else
								// From 5.2 it's changed with FRandomStream as first argument
								if (UKismetMathLibrary::RandomBoolWithWeightFromStream(paintEntireMesh_Random_Stream_Local, paintEntireMesh_Random_Probability_Local)) {
#endif

#else
								// If this vert was choosen, then set paintCurrentVertex to true so we actually paint on this and doesn't just run it
								if (UKismetMathLibrary::RandomBoolWithWeightFromStream(paintEntireMesh_Random_Probability_Local, paintEntireMesh_Random_Stream_Local)) {
#endif

									// UE_LOG(LogTemp, Warning, TEXT("Choose Vertex: %i  -   Amount Randomized left: %f   -   Total Verts left: %i   -   Random Weight: %f"), currentLODVertex_Local, paintEntireMesh_Random_AmountOfVerticesLeftToRandomize_Local, (meshLODTotalAmountOfVerts_Local - currentLODVertex_Local), paintEntireMesh_Random_Probability_Local);

									paintEntireMesh_Random_AmountOfVerticesLeftToRandomize_Local--;
									paintEntireMesh_PaintCurrentVertex_Local = true;
								}

								// If not choosen to paint on this vertex, we still let the loop continue, so we can get complete cloth data to apply etc. 
								else {

									paintEntireMesh_PaintCurrentVertex_Local = false;

									// UE_LOG(LogTemp, Warning, TEXT("Did Not choose Vertex. Amount Randomized left: %f   -   Total Verts left: %i   -   Random Weight: %f"), paintEntireMesh_Random_AmountOfVerticesLeftToRandomize_Local, (meshLODTotalAmountOfVerts_Local - currentLODVertex_Local), paintEntireMesh_Random_Probability_Local);
								}
							}

							else {

								paintEntireMesh_PaintCurrentVertex_Local = false;
							}
						}
					}

					else {

						paintEntireMesh_PaintCurrentVertex_Local = true;
					}
				}


				if (calculateColorsInfo_Global.drawVertexNormalDebugArrow && !calculateColorsInfo_Global.fundementalStruct.multiThreadSettings.useMultithreadingForCalculations)
					DrawDebugLine(calculateColorsInfo_Global.vertexPaintDetectionHitActor->GetWorld(), currentVertexWorldSpace, currentVertexWorldSpace + (normalsFromLOD_Local[currentLODVertex_Local] * 25), FColor::Red, false, calculateColorsInfo_Global.drawVertexesDebugPointsDuration, 0, .5);

				if (calculateColorsInfo_Global.drawVertexPositionDebugPoint && !calculateColorsInfo_Global.fundementalStruct.multiThreadSettings.useMultithreadingForCalculations)
					DrawDebugPoint(calculateColorsInfo_Global.vertexPaintDetectionHitActor->GetWorld(), currentVertexWorldSpace, 5, FColor::Red, false, calculateColorsInfo_Global.drawVertexesDebugPointsDuration, 0);


				// Saves the element that was closest only at LOD0
				if (currentLOD_Local == 0) {

					// If just get closest vertex data detection, or paint at location, or paint at location with detection combo
					if (calculateColorsInfo_Global.paintOnMeshAtLocationSettings.actor || (calculateColorsInfo_Global.detectInfoSettings.actor && !calculateColorsInfo_Global.getAllVertexColorsSettings.actor)) {


						if (calculateColorsInfo_Global.paintOnMeshAtLocationSettings.actor) {

							if ((hitLocation_WorldSpace_Paint_Local - currentVertexWorldSpace).Size() < closestDistanceToClosestVertex_Paint_Local) {


								// If its less then updates distanceFromTraceHitToVertPos, so in the next loop it will check against that value
								closestDistanceToClosestVertex_Paint_Local = (hitLocation_WorldSpace_Paint_Local - currentVertexWorldSpace).Size();

								closestVertexFound_Paint_Global = currentLODVertex_Local;
								closestVertexFound_Paint_Temp = currentLODVertex_Local;
								closestSectionFound_Paint_Global = currentSection_Local;

								directionToClosestVertex_Paint_Local = UKismetMathLibrary::GetDirectionUnitVector(hitLocation_WorldSpace_Paint_Local, currentVertexWorldSpace);
							}


							getEstimatedColorAtHitLocation_Local = false;

							if (calculateColorsInfo_Global.estimatedColorAtHitLocationSettings.getEstimatedColorAtHitLocation) {

								if (calculateColorsInfo_Global.estimatedColorAtHitLocationSettings.onlyGetIfMeshHasMaxAmountOfVertices) {

									if (vertexDetectMeshData_Global.meshDataPerLOD[0].amountOfVerticesAtLOD <= calculateColorsInfo_Global.estimatedColorAtHitLocationSettings.maxAmountOfVertices)
										getEstimatedColorAtHitLocation_Local = true;
								}
								else {

									getEstimatedColorAtHitLocation_Local = true;
								}
							}

							// Adds the closest 9 into a TMap containing the closest verts and the direction, index and distance to them. This is used when we've finished looping through LOD0 verts to determine which vertex we want to scale against to calculate the colors at the Hit Location. So based on the direction from the closest vertex, and the hit location, we can check which of the nearby vertices has the best Dot toward it. Then based on the distance to it we lerp from the closest vertex color, to the one that had the best dot, based on the distance to it, to get a close to accurate Color at Actual Hit Location of the Mesh, which is useful if you have meshes with few vertices. 
							if (getEstimatedColorAtHitLocation_Local)
								IsVertexCloseToEstimatedColorToHitLocation(directionFromHitToClosestVerticesInfo_LongestDistanceTemp_Paint, directionFromHitToClosestVerticesInfo_LongestDistanceIndexTemp_Paint, directionFromHitToClosestVerticesInfo_Paint, hitLocation_WorldSpace_Paint_Local, currentVertexWorldSpace, currentLODVertex_Local);
						}


						if (calculateColorsInfo_Global.detectInfoSettings.actor) {

							if (detectGetAverageColor_Local.areaRangeToGetAvarageColorFrom > 0) {

								// If within range
								if ((hitLocation_WorldSpace_Detection_Local - currentVertexWorldSpace).Size() <= detectGetAverageColor_Local.areaRangeToGetAvarageColorFrom) {


									// Vertex World Normal is within Dot to Hit Normal. So we can only detect and get average color from one side of a wall if set to 0 for instance
									if (cureentVertexNormalToHitNormalDot_Detect >= detectGetAverageColor_Local.VertexNormalToHitNormal_MinimumDotProductToBeAccountedFor) {

										amountOfVerticesWithinArea_Detect++;

										avarageColorWithinArea_Global_Detect_Before_Red += colorFromLOD_Local[currentLODVertex_Local].R;
										avarageColorWithinArea_Global_Detect_Before_Green += colorFromLOD_Local[currentLODVertex_Local].G;
										avarageColorWithinArea_Global_Detect_Before_Blue += colorFromLOD_Local[currentLODVertex_Local].B;
										avarageColorWithinArea_Global_Detect_Before_Alpha += colorFromLOD_Local[currentLODVertex_Local].A;
									}
								}
							}

							if ((hitLocation_WorldSpace_Detection_Local - currentVertexWorldSpace).Size() < closestDistanceToClosestVertex_Detect_Local) {

								// If its less then updates distanceFromTraceHitToVertPos, so in the next loop it will check against that value
								closestDistanceToClosestVertex_Detect_Local = (hitLocation_WorldSpace_Detection_Local - currentVertexWorldSpace).Size();

								closestVertexFound_Detection_Global = currentLODVertex_Local;
								closestSectionFound_Detection_Global = currentSection_Local;

								directionToClosestVertex_Detect_Local = UKismetMathLibrary::GetDirectionUnitVector(hitLocation_WorldSpace_Detection_Local, currentVertexWorldSpace);
							}

							getEstimatedColorAtHitLocation_Local = false;

							if (calculateColorsInfo_Global.estimatedColorAtHitLocationSettings.getEstimatedColorAtHitLocation) {

								if (calculateColorsInfo_Global.estimatedColorAtHitLocationSettings.onlyGetIfMeshHasMaxAmountOfVertices) {

									if (vertexDetectMeshData_Global.meshDataPerLOD[0].amountOfVerticesAtLOD <= calculateColorsInfo_Global.estimatedColorAtHitLocationSettings.maxAmountOfVertices)
										getEstimatedColorAtHitLocation_Local = true;
								}
								else {

									getEstimatedColorAtHitLocation_Local = true;
								}

								if (calculateColorsInfo_Global.paintOnMeshAtLocationSettings.actor) {

									if (calculateColorsInfo_Global.paintOnMeshAtLocationSettings.detection.useCustomHitFundementalsForDetection) {

										//
									}

									// If it's a paint at location with detect before/after, but at the same hit location the paint at location is run on, then there's no need to fill the detection directionFromHitToClosestVerticesInfo_Detect TMap since it can just use the same result the paint job gets. This should save some performance since there's less loops every vertex etc.
									else {

										getEstimatedColorAtHitLocation_Local = false;
									}
								}
							}

							// Adds the closest 9 into a TMap containing the closest verts and the direction, index and distance to them. This is used when we've finished looping through LOD0 verts to determine which vertex we want to scale against to calculate the colors at the Hit Location. So based on the direction from the closest vertex, and the hit location, we can check which of the nearby vertices has the best Dot toward it. Then based on the distance to it we lerp from the closest vertex color, to the one that had the best dot, based on the distance to it, to get a close to accurate Color at Actual Hit Location of the Mesh, which is useful if you have meshes with few vertices. 
							if (getEstimatedColorAtHitLocation_Local) {

								IsVertexCloseToEstimatedColorToHitLocation(directionFromHitToClosestVerticesInfo_LongestDistanceTemp_Detect, directionFromHitToClosestVerticesInfo_LongestDistanceIndexTemp_Detect, directionFromHitToClosestVerticesInfo_Detect, hitLocation_WorldSpace_Detection_Local, currentVertexWorldSpace, currentLODVertex_Local);
							}
						}
					}


					linearColor_Local = colorFromLOD_Local[currentLODVertex_Local].ReinterpretAsLinear();


					// Always get the total amount for each channel so we can after the task is done check if all is 0 or 1, if we're trying to add colors to an already fully painted mesh
					if (linearColor_Local.R > 0) {

						amountOfPaintedColorsOfEachChannel_Total_Local.amountOfVerticesWithAnyRedPaintedAtTheMinAmount++;
						amountOfPaintedColorsOfEachChannel_Total_Local.averageAmountOfRedColorAtTheMinAmount += linearColor_Local.R;
					}
					if (linearColor_Local.G > 0) {

						amountOfPaintedColorsOfEachChannel_Total_Local.amountOfVerticesWithAnyGreenPaintedAtTheMinAmount++;
						amountOfPaintedColorsOfEachChannel_Total_Local.averageAmountOfGreenColorAtTheMinAmount += linearColor_Local.G;
					}
					if (linearColor_Local.B > 0) {

						amountOfPaintedColorsOfEachChannel_Total_Local.amountOfVerticesWithAnyBluePaintedAtTheMinAmount++;
						amountOfPaintedColorsOfEachChannel_Total_Local.averageAmountOfBlueColorAtTheMinAmount += linearColor_Local.B;
					}
					if (linearColor_Local.A > 0) {

						amountOfPaintedColorsOfEachChannel_Total_Local.amountOfVerticesWithAnyAlphaPaintedAtTheMinAmount++;
						amountOfPaintedColorsOfEachChannel_Total_Local.averageAmountOfAlphaColorAtTheMinAmount += linearColor_Local.A;
					}


					if (calculateColorsInfo_Global.fundementalStruct.callbackSettings.includeAmountOfPaintedColorsOfEachChannel.includeAmountOfPaintedColorsOfEachChannel) {

						if (linearColor_Local.R >= calculateColorsInfo_Global.fundementalStruct.callbackSettings.includeAmountOfPaintedColorsOfEachChannel.includeAmountOfPaintedColorsOfEachChannel_IfMinColorAmount) {

							amountOfPaintedColorsOfEachChannel_BeforeApplyingColor_Local.amountOfVerticesWithAnyRedPaintedAtTheMinAmount++;
							amountOfPaintedColorsOfEachChannel_BeforeApplyingColor_Local.averageAmountOfRedColorAtTheMinAmount += linearColor_Local.R;
						}
						if (linearColor_Local.G >= calculateColorsInfo_Global.fundementalStruct.callbackSettings.includeAmountOfPaintedColorsOfEachChannel.includeAmountOfPaintedColorsOfEachChannel_IfMinColorAmount) {

							amountOfPaintedColorsOfEachChannel_BeforeApplyingColor_Local.amountOfVerticesWithAnyGreenPaintedAtTheMinAmount++;
							amountOfPaintedColorsOfEachChannel_BeforeApplyingColor_Local.averageAmountOfGreenColorAtTheMinAmount += linearColor_Local.G;
						}
						if (linearColor_Local.B >= calculateColorsInfo_Global.fundementalStruct.callbackSettings.includeAmountOfPaintedColorsOfEachChannel.includeAmountOfPaintedColorsOfEachChannel_IfMinColorAmount) {

							amountOfPaintedColorsOfEachChannel_BeforeApplyingColor_Local.amountOfVerticesWithAnyBluePaintedAtTheMinAmount++;
							amountOfPaintedColorsOfEachChannel_BeforeApplyingColor_Local.averageAmountOfBlueColorAtTheMinAmount += linearColor_Local.B;
						}
						if (linearColor_Local.A >= calculateColorsInfo_Global.fundementalStruct.callbackSettings.includeAmountOfPaintedColorsOfEachChannel.includeAmountOfPaintedColorsOfEachChannel_IfMinColorAmount) {

							amountOfPaintedColorsOfEachChannel_BeforeApplyingColor_Local.amountOfVerticesWithAnyAlphaPaintedAtTheMinAmount++;
							amountOfPaintedColorsOfEachChannel_BeforeApplyingColor_Local.averageAmountOfAlphaColorAtTheMinAmount += linearColor_Local.A;
						}
					}
				}


				// If Painting

				if (calculateColorsInfo_Global.vertexPaintSettings.actor && colorFromLOD_Local.IsValidIndex(currentLODVertex_Local)) {


					if (shouldApplyVertexColors_Local) {

						vertexColorGotChanged_Local = false;

						if (WillAppliedColorMakeChangeOnVertex(colorFromLOD_Local[currentLODVertex_Local], currentVertexWorldSpace, normalsFromLOD_Local[currentLODVertex_Local], calculateColorsInfo_Global.paintOnMeshColorSettings)) {


							if (calculateColorsInfo_Global.paintOnMeshAtLocationSettings.actor && calculateColorsInfo_Global.paintOnMeshAtLocationSettings.paintAtAreaSettings.areaOfEffect > 0) {

								// If distance from hit and the vertex is within the paint area off effect then gets the color updated with strength, falloffs etc. 
								if ((hitLocation_WorldSpace_Paint_Local - currentVertexWorldSpace).Size() <= calculateColorsInfo_Global.paintOnMeshAtLocationSettings.paintAtAreaSettings.areaOfEffect) {

									// Vertex World Normal is within Dot to Hit Normal, so the othersides of a house wall etc. doesn't get painted. Can get set to -1 if everything within area should be painted. 
									if (cureentVertexNormalToHitNormalDot_Paint >= vertexNormalMinDotToHitNormalToGetColored_MinDot_Local) {

										colorFromLOD_Local[currentLODVertex_Local] = GetColorToApplyOnVertex(colorFromLOD_Local[currentLODVertex_Local], currentVertexWorldSpace, normalsFromLOD_Local[currentLODVertex_Local], currentBoneName_Local, calculateColorsInfo_Global.paintOnMeshAtLocationSettings.paintAtAreaSettings.fallOffStrength, calculateColorsInfo_Global.paintOnMeshAtLocationSettings.paintAtAreaSettings.areaOfEffect, calculateColorsInfo_Global.paintOnMeshAtLocationSettings.paintAtAreaSettings.areaOfEffectWithFullColor, (hitLocation_WorldSpace_Paint_Local - currentVertexWorldSpace).Size(), calculateColorsInfo_Global.paintOnMeshColorSettings, vertexColorGotChanged_Local, applyColorsOnRedChannel_Local, applyColorsOnGreenChannel_Local, applyColorsOnBlueChannel_Local, applyColorsOnAlphaChannel_Local);

										triedToApplyColorOnVertex_Local = true;
									}
								}
							}


							else if (calculateColorsInfo_Global.paintOnMeshWithinAreaSettings.actor) {

								for (int i = 0; i < calculateColorsInfo_Global.paintOnMeshWithinArea_ComponentsToCheckIfIsWithinInfo.Num(); i++) {

									// If Complex shape then requires that the component is still valid as we trace for it
									if (calculateColorsInfo_Global.paintOnMeshWithinArea_ComponentsToCheckIfIsWithinInfo[i].paintWithinAreaShape == Enum_PaintWithinAreaShape::isComplexShape) {

										if (!IsValid(calculateColorsInfo_Global.paintOnMeshWithinArea_ComponentsToCheckIfIsWithinInfo[i].componentToCheckIfIsWithin)) return EVertexPaintDetectionTaskResult::FailWithInvalidMesh;
									}

									if (IsVertexWithinArea(currentVertexWorldSpace, calculateColorsInfo_Global.paintOnMeshWithinAreaSettings, calculateColorsInfo_Global.paintOnMeshWithinArea_ComponentsToCheckIfIsWithinInfo[i])) {


										if (calculateColorsInfo_Global.paintOnMeshWithinArea_ComponentsToCheckIfIsWithinInfo[i].paintWithinAreaShape == Enum_PaintWithinAreaShape::isComplexShape) {

											if (calculateColorsInfo_Global.paintOnMeshWithinAreaSettings.falloffType == EVertexPaintWithinAreaFallOffType::SphericalFromCenter) {

												paintWithinArea_DistanceFromVertex_Local = (calculateColorsInfo_Global.paintOnMeshWithinArea_ComponentsToCheckIfIsWithinInfo[i].componentToCheckIfIsWithin_ActorBounds_Origin - currentVertexWorldSpace).Size();
											}

											else if (calculateColorsInfo_Global.paintOnMeshWithinAreaSettings.falloffType == EVertexPaintWithinAreaFallOffType::GradiantUpward) {

												// Checks distance to the lowest point, the closer to it the stronger
												paintWithinArea_DistanceFromVertex_Local = FMath::Abs(currentVertexWorldSpace.Z - (calculateColorsInfo_Global.paintOnMeshWithinArea_ComponentsToCheckIfIsWithinInfo[i].componentToCheckIfIsWithin_ActorBounds_Origin.Z - calculateColorsInfo_Global.paintOnMeshWithinArea_ComponentsToCheckIfIsWithinInfo[i].componentToCheckIfIsWithin_ActorBounds_Extent.Z / 2));
											}

											else if (calculateColorsInfo_Global.paintOnMeshWithinAreaSettings.falloffType == EVertexPaintWithinAreaFallOffType::GradiantDownward) {

												// Checks distance to the Highest point, the closer to it the stronger
												paintWithinArea_DistanceFromVertex_Local = FMath::Abs(currentVertexWorldSpace.Z - (calculateColorsInfo_Global.paintOnMeshWithinArea_ComponentsToCheckIfIsWithinInfo[i].componentToCheckIfIsWithin_ActorBounds_Origin.Z + calculateColorsInfo_Global.paintOnMeshWithinArea_ComponentsToCheckIfIsWithinInfo[i].componentToCheckIfIsWithin_ActorBounds_Extent.Z / 2));
											}
										}

										else {

											if (calculateColorsInfo_Global.paintOnMeshWithinAreaSettings.falloffType == EVertexPaintWithinAreaFallOffType::SphericalFromCenter) {

												paintWithinArea_DistanceFromVertex_Local = (calculateColorsInfo_Global.paintOnMeshWithinArea_ComponentsToCheckIfIsWithinInfo[i].componentToCheckIfIsWithin_WorldTransform.GetLocation() - currentVertexWorldSpace).Size();
											}

											else if (calculateColorsInfo_Global.paintOnMeshWithinAreaSettings.falloffType == EVertexPaintWithinAreaFallOffType::GradiantUpward) {

												// Checks distance to the lowest point, the closer to it the stronger
												paintWithinArea_DistanceFromVertex_Local = FMath::Abs(currentVertexWorldSpace.Z - (calculateColorsInfo_Global.paintOnMeshWithinArea_ComponentsToCheckIfIsWithinInfo[i].componentToCheckIfIsWithin_WorldTransform.GetLocation().Z - calculateColorsInfo_Global.paintOnMeshWithinArea_ComponentsToCheckIfIsWithinInfo[i].componentToCheckIfIsWithin_IsVertexWithinArea_CubeOrRectangle_SquareZHalfSize));
											}

											else if (calculateColorsInfo_Global.paintOnMeshWithinAreaSettings.falloffType == EVertexPaintWithinAreaFallOffType::GradiantDownward) {

												// Checks distance to the Highest point, the closer to it the stronger
												paintWithinArea_DistanceFromVertex_Local = FMath::Abs(currentVertexWorldSpace.Z - (calculateColorsInfo_Global.paintOnMeshWithinArea_ComponentsToCheckIfIsWithinInfo[i].componentToCheckIfIsWithin_WorldTransform.GetLocation().Z + calculateColorsInfo_Global.paintOnMeshWithinArea_ComponentsToCheckIfIsWithinInfo[i].componentToCheckIfIsWithin_IsVertexWithinArea_CubeOrRectangle_SquareZHalfSize));

											}
										}


										colorFromLOD_Local[currentLODVertex_Local] = GetColorToApplyOnVertex(colorFromLOD_Local[currentLODVertex_Local], currentVertexWorldSpace, normalsFromLOD_Local[currentLODVertex_Local], currentBoneName_Local, calculateColorsInfo_Global.paintOnMeshWithinAreaSettings.fallOffStrength, paintWithinArea_AreaOfEffects_Local[i], calculateColorsInfo_Global.paintOnMeshWithinAreaSettings.areaOfEffectWithFullColor, paintWithinArea_DistanceFromVertex_Local, calculateColorsInfo_Global.paintOnMeshColorSettings, vertexColorGotChanged_Local, applyColorsOnRedChannel_Local, applyColorsOnGreenChannel_Local, applyColorsOnBlueChannel_Local, applyColorsOnAlphaChannel_Local);


										triedToApplyColorOnVertex_Local = true;

										if (!paintWithinArea_TriedToApplyColorOnAnyVertex_Global)
											paintWithinArea_TriedToApplyColorOnAnyVertex_Global = triedToApplyColorOnVertex_Local;


										break;
									}
								}
							}


							else if (calculateColorsInfo_Global.paintOnEntireMeshSettings.actor) {

								if (paintEntireMesh_PaintCurrentVertex_Local) {

									colorFromLOD_Local[currentLODVertex_Local] = GetColorToApplyOnVertex(colorFromLOD_Local[currentLODVertex_Local], currentVertexWorldSpace, normalsFromLOD_Local[currentLODVertex_Local], currentBoneName_Local, 0, 0, 0, 0, calculateColorsInfo_Global.paintOnMeshColorSettings, vertexColorGotChanged_Local, applyColorsOnRedChannel_Local, applyColorsOnGreenChannel_Local, applyColorsOnBlueChannel_Local, applyColorsOnAlphaChannel_Local);

									triedToApplyColorOnVertex_Local = true;
								}
							}


							if (triedToApplyColorOnVertex_Local) {

								if (calculateColorsInfo_Global.drawVertexPositionDebugPointOnVertexThatGotPaintApplied && !calculateColorsInfo_Global.fundementalStruct.multiThreadSettings.useMultithreadingForCalculations)
									DrawDebugPoint(calculateColorsInfo_Global.vertexPaintDetectionHitActor->GetWorld(), currentVertexWorldSpace, 7, FColor::Green, false, calculateColorsInfo_Global.drawVertexesDebugPointsDuration, 0);
							}
						}

						if (!shouldApplyVertexColors_Global) {

							shouldApplyVertexColors_Global = vertexColorGotChanged_Local;

							// UE_LOG(LogTemp, Warning, TEXT("shouldApplyVertexColors_Global: %i"), shouldApplyVertexColors_Global);
						}
					}

					// UE_LOG(LogTemp, Warning, TEXT("Color to Apply Index: %i  -  Color: %s"), currentLODVertex_Local, *colorFromLOD_Local[currentLODVertex_Local].ToString());

					// Updates color buffer vertex color, so when we apply the colors we can just set to use the new buffer and don't have to run InitFromColorArray which loops through all of the verts. Runs this even if shouldApplyVertexColors_Local is false, since the newColorBuffer still has be set with the current color, otherwise the sections that we're not applying color on will look weird. 
					if (newColorVertexBufferPerLOD_Global.IsValidIndex(currentLOD_Local)) {

						newColorVertexBufferPerLOD_Global[currentLOD_Local]->VertexColor(currentLODVertex_Local) = colorFromLOD_Local[currentLODVertex_Local];
					}
				}


				if (calculateColorsInfo_Global.vertexPaintSkelComponent && currentLOD_Local == 0 && colorFromLOD_Local.IsValidIndex(currentLODVertex_Local)) {

					// Had to have an array of a color array that i add to instead of adding directly to the TMap since then i had to use .Contain and .FindRef which made it so each task took longer to finish
					if (boneIndexColors_Local.IsValidIndex(currentBoneIndex_Local) && calculateColorsInfo_Global.fundementalStruct.callbackSettings.includeColorsOfEachBone) {

						boneIndexColors_Local[currentBoneIndex_Local].Add(colorFromLOD_Local[currentLODVertex_Local]);
					}
				}


				// If painting then sets a cached avarageColorWithinArea After we've applied colors. 
				if (currentLOD_Local == 0) {


					if (calculateColorsInfo_Global.paintOnMeshAtLocationSettings.actor) {

						// if within range
						if ((hitLocation_WorldSpace_Paint_Local - currentVertexWorldSpace).Size() <= calculateColorsInfo_Global.paintOnMeshAtLocationSettings.paintAtAreaSettings.areaOfEffect) {

							// Vertex World Normal is within Dot to Hit Normal. So we can only detect and get average color from one side of a wall if set to 0 for instance
							if (cureentVertexNormalToHitNormalDot_Paint >= vertexNormalMinDotToHitNormalToGetColored_MinDot_Local) {

								amountOfVerticesWithinArea_Paint++;

								avarageColorWithinArea_Global_Red += colorFromLOD_Local[currentLODVertex_Local].R;
								avarageColorWithinArea_Global_Green += colorFromLOD_Local[currentLODVertex_Local].G;
								avarageColorWithinArea_Global_Blue += colorFromLOD_Local[currentLODVertex_Local].B;
								avarageColorWithinArea_Global_Alpha += colorFromLOD_Local[currentLODVertex_Local].A;
							}
						}

						// If detecting while painting
						if (calculateColorsInfo_Global.detectInfoSettings.actor) {

							if ((hitLocation_WorldSpace_Detection_Local - currentVertexWorldSpace).Size() <= detectGetAverageColor_Local.areaRangeToGetAvarageColorFrom) {

								if (cureentVertexNormalToHitNormalDot_Detect >= detectGetAverageColor_Local.VertexNormalToHitNormal_MinimumDotProductToBeAccountedFor) {

									amountOfVerticesWithinArea_Detect_After++;

									avarageColorWithinArea_Global_Detect_After_Red += colorFromLOD_Local[currentLODVertex_Local].R;
									avarageColorWithinArea_Global_Detect_After_Green += colorFromLOD_Local[currentLODVertex_Local].G;
									avarageColorWithinArea_Global_Detect_After_Blue += colorFromLOD_Local[currentLODVertex_Local].B;
									avarageColorWithinArea_Global_Detect_After_Alpha += colorFromLOD_Local[currentLODVertex_Local].A;
								}
							}
						}
					}

					if (calculateColorsInfo_Global.fundementalStruct.callbackSettings.includeAmountOfPaintedColorsOfEachChannel.includeAmountOfPaintedColorsOfEachChannel) {


						linearColor_Local = colorFromLOD_Local[currentLODVertex_Local].ReinterpretAsLinear();

						if (linearColor_Local.R >= calculateColorsInfo_Global.fundementalStruct.callbackSettings.includeAmountOfPaintedColorsOfEachChannel.includeAmountOfPaintedColorsOfEachChannel_IfMinColorAmount) {

							amountOfPaintedColorsOfEachChannel_Local.amountOfVerticesWithAnyRedPaintedAtTheMinAmount++;
							amountOfPaintedColorsOfEachChannel_Local.averageAmountOfRedColorAtTheMinAmount += linearColor_Local.R;
						}
						if (linearColor_Local.G >= calculateColorsInfo_Global.fundementalStruct.callbackSettings.includeAmountOfPaintedColorsOfEachChannel.includeAmountOfPaintedColorsOfEachChannel_IfMinColorAmount) {

							amountOfPaintedColorsOfEachChannel_Local.amountOfVerticesWithAnyGreenPaintedAtTheMinAmount++;
							amountOfPaintedColorsOfEachChannel_Local.averageAmountOfGreenColorAtTheMinAmount += linearColor_Local.G;
						}
						if (linearColor_Local.B >= calculateColorsInfo_Global.fundementalStruct.callbackSettings.includeAmountOfPaintedColorsOfEachChannel.includeAmountOfPaintedColorsOfEachChannel_IfMinColorAmount) {

							amountOfPaintedColorsOfEachChannel_Local.amountOfVerticesWithAnyBluePaintedAtTheMinAmount++;
							amountOfPaintedColorsOfEachChannel_Local.averageAmountOfBlueColorAtTheMinAmount += linearColor_Local.B;
						}
						if (linearColor_Local.A >= calculateColorsInfo_Global.fundementalStruct.callbackSettings.includeAmountOfPaintedColorsOfEachChannel.includeAmountOfPaintedColorsOfEachChannel_IfMinColorAmount) {

							amountOfPaintedColorsOfEachChannel_Local.amountOfVerticesWithAnyAlphaPaintedAtTheMinAmount++;
							amountOfPaintedColorsOfEachChannel_Local.averageAmountOfAlphaColorAtTheMinAmount += linearColor_Local.A;
						}
					}


					// If set to randomize over the entire mesh, and to use Modified Engine Method, then we get the things it requires. If this vertex is a doublette of another that is on the Exact same position, then we want it to also get painted, so all doublettes at a position have the same color. This is so when we run our calculation so LOD1, 2 vertices get the same color as their closest and best LOD0 vertex, it shouldn't matter which of the doublettes the calculation chooses to be the best, as they all have the same color. Otherwise we had an issue where other LODs wouldn't get LOD0 color if using modified engine method since the result of it can be that the closest LOD0 vertex color to use for a LOD1 vertex could be a doublette of the randomized one that was on the exact same location

					if (updateMeshVertexColorData && vertexDetectMeshData_Global.meshDataPerLOD.Num() > 1 && calculateColorsInfo_Global.propogateLOD0ToAllLODs) {

						paintedVertex_Local.Color = colorFromLOD_Local[currentLODVertex_Local];
						paintedVerticesAtLOD0_Local.Add(paintedVertex_Local);

						if (paintEntireMesh_PaintCurrentVertex_Local)
							paintEntireMeshRandom_VerticesDoublettesPaintedAtLOD0_Local.Add(positionsInComponentLocalSpaceFromLOD_Local[currentLODVertex_Local], colorFromLOD_Local[currentLODVertex_Local]);
					}
				}


				// Affected Cloth is exclusive to UE5 and ChaosCloth
#if ENGINE_MAJOR_VERSION == 5


				if (calculateColorsInfo_Global.vertexPaintSettings.affectClothPhysics && calculateColorsInfo_Global.vertexPaintSkelComponent && clothsToColorAndPhysicsSettings_Local.Num() > 0 && currentLOD_Local == calculateColorsInfo_Global.vertexPaintSkelComponent->GetPredictedLODLevel()) {



					const FSkeletalMeshLODRenderData& skelMeshRenderDataTemp = calculateColorsInfo_Global.vertexPaintSkelComponent->GetSkeletalMeshRenderData()->LODRenderData[currentLOD_Local];
					FSkinWeightVertexBuffer& skinWeightBufferTemp = *calculateColorsInfo_Global.vertexPaintSkelComponent->GetSkinWeightBuffer(currentLOD_Local);

					// If the current vertexes section has cloth. Note not necessarily the one within painting area but just the vertex we're at 
					if (skelMeshRenderDataTemp.RenderSections[currentSection_Local].HasClothingData()) {


						currentClothingAssetBase_Local = clothingAssets_Local[skelMeshRenderDataTemp.RenderSections[currentSection_Local].CorrespondClothAssetIndex];

						// If not set to affect physics on this cloth then skips it
						if (!clothsToColorAndPhysicsSettings_Local.Contains(currentClothingAssetBase_Local)) continue;


						if (!clothPaintedColors_Local.IsValidIndex(skelMeshRenderDataTemp.RenderSections[currentSection_Local].CorrespondClothAssetIndex)) {

							clothPaintedColors_Local.SetNum((int32)skelMeshRenderDataTemp.RenderSections[currentSection_Local].CorrespondClothAssetIndex + 1, false);

							paintedClothsAndAvarageColor_Local.SetNum((int32)skelMeshRenderDataTemp.RenderSections[currentSection_Local].CorrespondClothAssetIndex + 1, false);
						}

						clothPaintedColors_Local[skelMeshRenderDataTemp.RenderSections[currentSection_Local].CorrespondClothAssetIndex].Add(colorFromLOD_Local[currentLODVertex_Local]);



						if (!paintedCloths_Local.IsValidIndex(skelMeshRenderDataTemp.RenderSections[currentSection_Local].CorrespondClothAssetIndex)) {

							paintedCloths_Local.SetNum((int32)skelMeshRenderDataTemp.RenderSections[currentSection_Local].CorrespondClothAssetIndex + 1);
						}

						// Adds Painted Cloths in the order they got painted as. Only sets the cloth ptr if haven't been set before
						if (!IsValid(paintedCloths_Local[(int32)skelMeshRenderDataTemp.RenderSections[currentSection_Local].CorrespondClothAssetIndex])) {

							paintedCloths_Local[(int32)skelMeshRenderDataTemp.RenderSections[currentSection_Local].CorrespondClothAssetIndex] = currentClothingAssetBase_Local;
						}


						FVertexDetectRGBAStruct avarageClothColorFloat_Local = paintedClothsAndAvarageColor_Local[skelMeshRenderDataTemp.RenderSections[currentSection_Local].CorrespondClothAssetIndex];

						if (colorFromLOD_Local[currentLODVertex_Local].R > 0)
							avarageClothColorFloat_Local.redColor += colorFromLOD_Local[currentLODVertex_Local].R;

						if (colorFromLOD_Local[currentLODVertex_Local].G > 0)
							avarageClothColorFloat_Local.greenColor += colorFromLOD_Local[currentLODVertex_Local].G;

						if (colorFromLOD_Local[currentLODVertex_Local].B > 0)
							avarageClothColorFloat_Local.blueColor += colorFromLOD_Local[currentLODVertex_Local].B;

						if (colorFromLOD_Local[currentLODVertex_Local].A > 0)
							avarageClothColorFloat_Local.alphaColor += colorFromLOD_Local[currentLODVertex_Local].A;

						paintedClothsAndAvarageColor_Local[skelMeshRenderDataTemp.RenderSections[currentSection_Local].CorrespondClothAssetIndex] = avarageClothColorFloat_Local;
					}
				}
#endif


				// Serializes each element while we're looping through the verts, so we don't have to loop through them again for the LOD after
				if (calculateColorsInfo_Global.fundementalStruct.callbackSettings.includeSerializedVertexColorData) {

					bool serializeLOD_Local = (calculateColorsInfo_Global.fundementalStruct.callbackSettings.includeVertexDataOnlyForLOD0 && currentLOD_Local != 0) ? false : true;

					if (serializeLOD_Local) {

						// Has to be bookended with [] so it matches our serialized TArray format
						if (currentLODVertex_Local == 0)
							serializedColorData_Local += "[";

						// Needs , inbetween to match the regular serialized TArray format, but not on the first one
						if (currentLODVertex_Local > 0)
							serializedColorData_Local += ",";

						serializedColorData_Local += URapidJsonFunctionLibrary::SerializeFColor_Wrapper(colorFromLOD_Local[currentLODVertex_Local]);


						if (currentLODVertex_Local == colorFromLOD_Local.Num() - 1)
							serializedColorData_Local += "]";

					}
				}


				if (currentLOD_Local == 0 && calculateColorsInfo_Global.fundementalStruct.callbackSettings.compareMeshColorsToColorArray.compareMeshColorsToColorArrayAtLOD0) {

					// If mesh color array amount matches, then we add 0.25 for each channel that is matches, so if it matches on each channel, on each vertex, it should be 100% match
					if (colorFromLOD_Local.Num() == calculateColorsInfo_Global.fundementalStruct.callbackSettings.compareMeshColorsToColorArray.colorArrayToCompareWith.Num()) {

						if (CompareVertexColor(currentLODVertex_Local, colorFromLOD_Local, calculateColorsInfo_Global.fundementalStruct.callbackSettings.compareMeshColorsToColorArray.colorArrayToCompareWith)) {

							compareMeshColorsToColorArrayResult_Global.matchingPercent++;
						}
					}
				}


				// Finished Vertex Loop
				currentLODVertex_Local++;
			}


			// UE_LOG(LogTemp, Warning, TEXT("Finished Looping through Section: %i"), currentSection_Local);

			// Finished Looping Through Section
		}


		// UE_LOG(LogTemp, Warning, TEXT("Finished Looping through LOD: %i"), currentLOD_Local);

		// Finished Looping Through LOD

		if (calculateColorsInfo_Global.fundementalStruct.callbackSettings.includeSerializedVertexColorData) {

			bool serializeLOD_Local = (calculateColorsInfo_Global.fundementalStruct.callbackSettings.includeVertexDataOnlyForLOD0 && currentLOD_Local != 0) ? false : true;

			if (serializeLOD_Local) {

				FVertexDetectSerializedColorsPerLODStruct serializedColors;
				serializedColors.lod = currentLOD_Local;
				serializedColors.colorsAtLODAsJSonString = serializedColorData_Local;

				vertexDetectMeshData_Global.meshDataPerLOD[currentLOD_Local].serializedVertexColorsData = serializedColors;
			}
		}


		if (currentLOD_Local == 0) {


			if (calculateColorsInfo_Global.fundementalStruct.callbackSettings.compareMeshColorsToColorArray.compareMeshColorsToColorArrayAtLOD0) {

				if (colorFromLOD_Local.Num() == calculateColorsInfo_Global.fundementalStruct.callbackSettings.compareMeshColorsToColorArray.colorArrayToCompareWith.Num()) {

					compareMeshColorsToColorArrayResult_Global.successfullyComparedMeshColorsToColorArray = true;

					compareMeshColorsToColorArrayResult_Global.matchingPercent = compareMeshColorsToColorArrayResult_Global.matchingPercent / colorFromLOD_Local.Num();
					compareMeshColorsToColorArrayResult_Global.matchingPercent *= 100; // So it goes from 0-100 instead of 0-1
				}
			}


			if (closestVertexFound_Paint_Global >= 0) {

				closestVertexColor_AfterApplyingColors_Paint_Global = colorFromLOD_Local[closestVertexFound_Paint_Global];

				closestVertexPositionToHitLocationAtMinLOD_World_Paint_Global = positionsInWorldSpaceFromLOD_Local[closestVertexFound_Paint_Global];

				closestVertexNormalToHitLocationAtMinLOD_World_Paint_Global = normalsFromLOD_Local[closestVertexFound_Paint_Global];


				// Getting closest materials
				if (calculateColorsInfo_Global.vertexPaintStaticMeshComponent) {

					materialAtClosestPaintVertex_Global = calculateColorsInfo_Global.vertexPaintStaticMeshComponent->GetMaterial(calculateColorsInfo_Global.vertexPaintStaticMeshComponent->GetStaticMesh()->GetRenderData()->LODResources[0].Sections[closestSectionFound_Paint_Global].MaterialIndex);
				}

				else if (calculateColorsInfo_Global.vertexPaintSkelComponent) {

					const FSkeletalMeshLODRenderData& skelMeshRenderData_Local = calculateColorsInfo_Global.vertexPaintSkelComponent->GetSkeletalMeshRenderData()->LODRenderData[0];

					materialAtClosestPaintVertex_Global = calculateColorsInfo_Global.vertexPaintSkelComponent->GetMaterial(skelMeshRenderData_Local.RenderSections[closestSectionFound_Paint_Global].MaterialIndex);
				}


				// Estimated Color at Hit Location
				int closestVertexToLerpTowardIndex_Paint_Temp = -1;

				if (directionFromHitToClosestVerticesInfo_Paint.Num() > 0) {

					closestVertexToLerpTowardIndex_Paint_Temp = GetEstimatedColorToHitLocationVertexToLerpColorToward(directionFromHitToClosestVerticesInfo_Paint, estimatedColorAtHitLocation_ClosestVerticesToTheHitLocation_Paint_Global, directionToClosestVertex_Paint_Local, closestVertexFound_Paint_Global, 0.2f);

					if (closestVertexToLerpTowardIndex_Paint_Temp >= 0) {

						estimatedColorAtHitLocation_VertexToLerpToward_Paint_Global = closestVertexToLerpTowardIndex_Paint_Temp;


						estimatedColorAtHitLocation_VertexToLerpToward_LocationInWorldSpace_Paint_Global = positionsInWorldSpaceFromLOD_Local[closestVertexToLerpTowardIndex_Paint_Temp];

						GetEstimatedColorToHitLocationValues(closestVertexToLerpTowardIndex_Paint_Temp, colorFromLOD_Local[closestVertexFound_Paint_Global], closestVertexPositionToHitLocationAtMinLOD_World_Paint_Global, hitLocation_WorldSpace_Paint_Local, colorFromLOD_Local[closestVertexToLerpTowardIndex_Paint_Temp], estimatedColorAtHitLocation_VertexToLerpToward_LocationInWorldSpace_Paint_Global, estimatedColorAtHitLocation_AfterApplyingColors_Paint_Global, estimatedColorAtHitLocation_LocationInWorldSpace_Paint_Global);

						// Also sets a estimatedColorAtHitLocation_BeforeApplyingColors_Paint_Global which is used if we're also running a detection before applying colors but not on custom hit result but based on the same hit location as the paint job. 
						GetEstimatedColorToHitLocationValues(closestVertexToLerpTowardIndex_Paint_Temp, defaultColorFromLOD_Local[closestVertexFound_Paint_Global], closestVertexPositionToHitLocationAtMinLOD_World_Paint_Global, hitLocation_WorldSpace_Paint_Local, defaultColorFromLOD_Local[closestVertexToLerpTowardIndex_Paint_Temp], estimatedColorAtHitLocation_VertexToLerpToward_LocationInWorldSpace_Paint_Global, estimatedColorAtHitLocation_BeforeApplyingColors_Paint_Global, estimatedColorAtHitLocation_LocationInWorldSpace_Paint_Global);
					}
				}
			}


			if (closestVertexFound_Detection_Global >= 0) {

				closestVertexColor_BeforeApplyingColors_Detect_Global = defaultColorFromLOD_Local[closestVertexFound_Detection_Global];
				closestVertexColor_AfterApplyingColors_Detect_Global = colorFromLOD_Local[closestVertexFound_Detection_Global];


				closestVertexPositionToHitLocationAtMinLOD_World_Detect_Global = positionsInWorldSpaceFromLOD_Local[closestVertexFound_Detection_Global];

				closestVertexNormalToHitLocationAtMinLOD_World_Detect_Global = normalsFromLOD_Local[closestVertexFound_Detection_Global];


				// Getting closest materials
				if (calculateColorsInfo_Global.vertexPaintStaticMeshComponent) {

					materialAtClosestDetectVertex_Global = calculateColorsInfo_Global.vertexPaintStaticMeshComponent->GetMaterial(calculateColorsInfo_Global.vertexPaintStaticMeshComponent->GetStaticMesh()->GetRenderData()->LODResources[0].Sections[closestSectionFound_Detection_Global].MaterialIndex);
				}

				else if (calculateColorsInfo_Global.vertexPaintSkelComponent) {

					const FSkeletalMeshLODRenderData& skelMeshRenderData_Local = calculateColorsInfo_Global.vertexPaintSkelComponent->GetSkeletalMeshRenderData()->LODRenderData[0];

					materialAtClosestDetectVertex_Global = calculateColorsInfo_Global.vertexPaintSkelComponent->GetMaterial(skelMeshRenderData_Local.RenderSections[closestSectionFound_Detection_Global].MaterialIndex);
				}


				// Estimated Color at Hit Location. If just a detection, or a paint at location with detection at a custom hit location, then uses the results of the detection directionFromHitToClosestVerticesInfo_Detect TMap, since it can have a different nearby vertices than the paint hit location
				if ((calculateColorsInfo_Global.detectInfoSettings.actor && !calculateColorsInfo_Global.paintOnMeshAtLocationSettings.actor) || (calculateColorsInfo_Global.paintOnMeshAtLocationSettings.actor && calculateColorsInfo_Global.paintOnMeshAtLocationSettings.detection.useCustomHitFundementalsForDetection)) {


					int closestVertexToLerpTowardIndex_Detect_Temp = -1;

					if (directionFromHitToClosestVerticesInfo_Detect.Num() > 0) {

						closestVertexToLerpTowardIndex_Detect_Temp = GetEstimatedColorToHitLocationVertexToLerpColorToward(directionFromHitToClosestVerticesInfo_Detect, estimatedColorAtHitLocation_ClosestVerticesToTheHitLocation_Detect_Global, directionToClosestVertex_Detect_Local, closestVertexFound_Detection_Global, 0.2f);


						if (closestVertexToLerpTowardIndex_Detect_Temp >= 0) {

							estimatedColorAtHitLocation_VertexToLerpToward_Detect_Global = closestVertexToLerpTowardIndex_Detect_Temp;

							estimatedColorAtHitLocation_VertexToLerpToward_LocationInWorldSpace_Detect_Global = positionsInWorldSpaceFromLOD_Local[closestVertexToLerpTowardIndex_Detect_Temp];

							// If just a detection, or a paint at location with detection set to run before it, then bases the colors on defaultColorFromLOD_Local
							if (calculateColorsInfo_Global.paintOnMeshAtLocationSettings.detection.runGetClosestVertexDataOnMeshBeforeApplyingPaint || calculateColorsInfo_Global.detectInfoSettings.actor)
								GetEstimatedColorToHitLocationValues(closestVertexToLerpTowardIndex_Detect_Temp, defaultColorFromLOD_Local[closestVertexFound_Detection_Global], closestVertexPositionToHitLocationAtMinLOD_World_Detect_Global, hitLocation_WorldSpace_Detection_Local, defaultColorFromLOD_Local[closestVertexToLerpTowardIndex_Detect_Temp], estimatedColorAtHitLocation_VertexToLerpToward_LocationInWorldSpace_Detect_Global, estimatedColorAtHitLocation_BeforeApplyingColors_Detect_Global, estimatedColorAtHitLocation_LocationInWorldSpace_Detect_Global);


							// If paint at location with set to run detection after colors have been changed, then uses colorFromLOD_Local instead defaultColorFromLOD_Local
							if (calculateColorsInfo_Global.paintOnMeshAtLocationSettings.detection.runGetClosestVertexDataOnMeshAfterApplyingPaint)
								GetEstimatedColorToHitLocationValues(closestVertexToLerpTowardIndex_Detect_Temp, colorFromLOD_Local[closestVertexFound_Detection_Global], closestVertexPositionToHitLocationAtMinLOD_World_Detect_Global, hitLocation_WorldSpace_Detection_Local, colorFromLOD_Local[closestVertexToLerpTowardIndex_Detect_Temp], estimatedColorAtHitLocation_VertexToLerpToward_LocationInWorldSpace_Detect_Global, estimatedColorAtHitLocation_AfterApplyingColors_Detect_Global, estimatedColorAtHitLocation_LocationInWorldSpace_Detect_Global);
						}
					}
				}

				// If it's a paint at location with detect before/after, at getEstimatedColorAtHitLocation, but at the same hit location the paint at location is run on, then we can just use its estimatedColorAtHitLocation result
				else if (directionFromHitToClosestVerticesInfo_Paint.Num() > 0) {


					estimatedColorAtHitLocation_LocationInWorldSpace_Detect_Global = estimatedColorAtHitLocation_LocationInWorldSpace_Paint_Global;
					estimatedColorAtHitLocation_ClosestVerticesToTheHitLocation_Detect_Global = estimatedColorAtHitLocation_ClosestVerticesToTheHitLocation_Paint_Global;

					if (calculateColorsInfo_Global.paintOnMeshAtLocationSettings.detection.runGetClosestVertexDataOnMeshBeforeApplyingPaint)
						estimatedColorAtHitLocation_BeforeApplyingColors_Detect_Global = estimatedColorAtHitLocation_BeforeApplyingColors_Paint_Global;

					else if (calculateColorsInfo_Global.paintOnMeshAtLocationSettings.detection.runGetClosestVertexDataOnMeshAfterApplyingPaint)
						estimatedColorAtHitLocation_AfterApplyingColors_Detect_Global = estimatedColorAtHitLocation_AfterApplyingColors_Paint_Global;
				}
			}


			amountOfPaintedColorsOfEachChannel_Total_Local.averageAmountOfRedColorAtTheMinAmount = amountOfPaintedColorsOfEachChannel_Total_Local.averageAmountOfRedColorAtTheMinAmount / colorFromLOD_Local.Num();
			amountOfPaintedColorsOfEachChannel_Total_Local.averageAmountOfGreenColorAtTheMinAmount = amountOfPaintedColorsOfEachChannel_Total_Local.averageAmountOfGreenColorAtTheMinAmount / colorFromLOD_Local.Num();
			amountOfPaintedColorsOfEachChannel_Total_Local.averageAmountOfBlueColorAtTheMinAmount = amountOfPaintedColorsOfEachChannel_Total_Local.averageAmountOfBlueColorAtTheMinAmount / colorFromLOD_Local.Num();
			amountOfPaintedColorsOfEachChannel_Total_Local.averageAmountOfAlphaColorAtTheMinAmount = amountOfPaintedColorsOfEachChannel_Total_Local.averageAmountOfAlphaColorAtTheMinAmount / colorFromLOD_Local.Num();

			amountOfPaintedColorsOfEachChannel_Total_Local.amountOfRedChannelPercentPaintedAtTheMinAmount = amountOfPaintedColorsOfEachChannel_Total_Local.amountOfVerticesWithAnyRedPaintedAtTheMinAmount / colorFromLOD_Local.Num();
			amountOfPaintedColorsOfEachChannel_Total_Local.amountOfGreenChannelPercentPaintedAtTheMinAmount = amountOfPaintedColorsOfEachChannel_Total_Local.amountOfVerticesWithAnyGreenPaintedAtTheMinAmount / colorFromLOD_Local.Num();
			amountOfPaintedColorsOfEachChannel_Total_Local.amountOfBlueChannelPercentPaintedAtTheMinAmount = amountOfPaintedColorsOfEachChannel_Total_Local.amountOfVerticesWithAnyBluePaintedAtTheMinAmount / colorFromLOD_Local.Num();
			amountOfPaintedColorsOfEachChannel_Total_Local.amountOfAlphaChannelPercentPaintedAtTheMinAmount = amountOfPaintedColorsOfEachChannel_Total_Local.amountOfVerticesWithAnyAlphaPaintedAtTheMinAmount / colorFromLOD_Local.Num();

			amountOfPaintedColorsOfEachChannel_Total_Local.amountOfRedChannelPercentPaintedAtTheMinAmount *= 100;
			amountOfPaintedColorsOfEachChannel_Total_Local.amountOfGreenChannelPercentPaintedAtTheMinAmount *= 100;
			amountOfPaintedColorsOfEachChannel_Total_Local.amountOfBlueChannelPercentPaintedAtTheMinAmount *= 100;
			amountOfPaintedColorsOfEachChannel_Total_Local.amountOfAlphaChannelPercentPaintedAtTheMinAmount *= 100;

			amountOfPaintedColorsOfEachChannel_Total_Local.successfullyGotVerticesOfEachChannelAtTheMinAmount = true;

			amountOfPaintedColorsOfEachChannelAbove0_Global = amountOfPaintedColorsOfEachChannel_Total_Local;


			if (calculateColorsInfo_Global.fundementalStruct.callbackSettings.includeAmountOfPaintedColorsOfEachChannel.includeAmountOfPaintedColorsOfEachChannel) {


				amountOfPaintedColorsOfEachChannel_BeforeApplyingColor_Local.averageAmountOfRedColorAtTheMinAmount = amountOfPaintedColorsOfEachChannel_BeforeApplyingColor_Local.averageAmountOfRedColorAtTheMinAmount / colorFromLOD_Local.Num();
				amountOfPaintedColorsOfEachChannel_BeforeApplyingColor_Local.averageAmountOfGreenColorAtTheMinAmount = amountOfPaintedColorsOfEachChannel_BeforeApplyingColor_Local.averageAmountOfGreenColorAtTheMinAmount / colorFromLOD_Local.Num();
				amountOfPaintedColorsOfEachChannel_BeforeApplyingColor_Local.averageAmountOfBlueColorAtTheMinAmount = amountOfPaintedColorsOfEachChannel_BeforeApplyingColor_Local.averageAmountOfBlueColorAtTheMinAmount / colorFromLOD_Local.Num();
				amountOfPaintedColorsOfEachChannel_BeforeApplyingColor_Local.averageAmountOfAlphaColorAtTheMinAmount = amountOfPaintedColorsOfEachChannel_BeforeApplyingColor_Local.averageAmountOfAlphaColorAtTheMinAmount / colorFromLOD_Local.Num();

				amountOfPaintedColorsOfEachChannel_BeforeApplyingColor_Local.amountOfRedChannelPercentPaintedAtTheMinAmount = amountOfPaintedColorsOfEachChannel_BeforeApplyingColor_Local.amountOfVerticesWithAnyRedPaintedAtTheMinAmount / colorFromLOD_Local.Num();
				amountOfPaintedColorsOfEachChannel_BeforeApplyingColor_Local.amountOfGreenChannelPercentPaintedAtTheMinAmount = amountOfPaintedColorsOfEachChannel_BeforeApplyingColor_Local.amountOfVerticesWithAnyGreenPaintedAtTheMinAmount / colorFromLOD_Local.Num();
				amountOfPaintedColorsOfEachChannel_BeforeApplyingColor_Local.amountOfBlueChannelPercentPaintedAtTheMinAmount = amountOfPaintedColorsOfEachChannel_BeforeApplyingColor_Local.amountOfVerticesWithAnyBluePaintedAtTheMinAmount / colorFromLOD_Local.Num();
				amountOfPaintedColorsOfEachChannel_BeforeApplyingColor_Local.amountOfAlphaChannelPercentPaintedAtTheMinAmount = amountOfPaintedColorsOfEachChannel_BeforeApplyingColor_Local.amountOfVerticesWithAnyAlphaPaintedAtTheMinAmount / colorFromLOD_Local.Num();

				amountOfPaintedColorsOfEachChannel_BeforeApplyingColor_Local.amountOfRedChannelPercentPaintedAtTheMinAmount *= 100;
				amountOfPaintedColorsOfEachChannel_BeforeApplyingColor_Local.amountOfGreenChannelPercentPaintedAtTheMinAmount *= 100;
				amountOfPaintedColorsOfEachChannel_BeforeApplyingColor_Local.amountOfBlueChannelPercentPaintedAtTheMinAmount *= 100;
				amountOfPaintedColorsOfEachChannel_BeforeApplyingColor_Local.amountOfAlphaChannelPercentPaintedAtTheMinAmount *= 100;

				amountOfPaintedColorsOfEachChannel_BeforeApplyingColor_Local.successfullyGotVerticesOfEachChannelAtTheMinAmount = true;

				amountOfPaintedColorsOfEachChannelAtMinAmount_BeforeApplyingColor_Global = amountOfPaintedColorsOfEachChannel_BeforeApplyingColor_Local;


				amountOfPaintedColorsOfEachChannel_Local.averageAmountOfRedColorAtTheMinAmount = amountOfPaintedColorsOfEachChannel_Local.averageAmountOfRedColorAtTheMinAmount / colorFromLOD_Local.Num();
				amountOfPaintedColorsOfEachChannel_Local.averageAmountOfGreenColorAtTheMinAmount = amountOfPaintedColorsOfEachChannel_Local.averageAmountOfGreenColorAtTheMinAmount / colorFromLOD_Local.Num();
				amountOfPaintedColorsOfEachChannel_Local.averageAmountOfBlueColorAtTheMinAmount = amountOfPaintedColorsOfEachChannel_Local.averageAmountOfBlueColorAtTheMinAmount / colorFromLOD_Local.Num();
				amountOfPaintedColorsOfEachChannel_Local.averageAmountOfAlphaColorAtTheMinAmount = amountOfPaintedColorsOfEachChannel_Local.averageAmountOfAlphaColorAtTheMinAmount / colorFromLOD_Local.Num();

				amountOfPaintedColorsOfEachChannel_Local.amountOfRedChannelPercentPaintedAtTheMinAmount = amountOfPaintedColorsOfEachChannel_Local.amountOfVerticesWithAnyRedPaintedAtTheMinAmount / colorFromLOD_Local.Num();
				amountOfPaintedColorsOfEachChannel_Local.amountOfGreenChannelPercentPaintedAtTheMinAmount = amountOfPaintedColorsOfEachChannel_Local.amountOfVerticesWithAnyGreenPaintedAtTheMinAmount / colorFromLOD_Local.Num();
				amountOfPaintedColorsOfEachChannel_Local.amountOfBlueChannelPercentPaintedAtTheMinAmount = amountOfPaintedColorsOfEachChannel_Local.amountOfVerticesWithAnyBluePaintedAtTheMinAmount / colorFromLOD_Local.Num();
				amountOfPaintedColorsOfEachChannel_Local.amountOfAlphaChannelPercentPaintedAtTheMinAmount = amountOfPaintedColorsOfEachChannel_Local.amountOfVerticesWithAnyAlphaPaintedAtTheMinAmount / colorFromLOD_Local.Num();

				amountOfPaintedColorsOfEachChannel_Local.amountOfRedChannelPercentPaintedAtTheMinAmount *= 100;
				amountOfPaintedColorsOfEachChannel_Local.amountOfGreenChannelPercentPaintedAtTheMinAmount *= 100;
				amountOfPaintedColorsOfEachChannel_Local.amountOfBlueChannelPercentPaintedAtTheMinAmount *= 100;
				amountOfPaintedColorsOfEachChannel_Local.amountOfAlphaChannelPercentPaintedAtTheMinAmount *= 100;

				amountOfPaintedColorsOfEachChannel_Local.successfullyGotVerticesOfEachChannelAtTheMinAmount = true;

				amountOfPaintedColorsOfEachChannelAtMinAmount_Global = amountOfPaintedColorsOfEachChannel_Local;
			}
		}


		// Updates the global vertexDetectMeshData which it should but only if it hasn't been set before. For instance if Paint Color Snippet, Paint Entire Mesh Set etc. should have this to false as they've already set it, and if they're running this function it is purely to get colors of each bones, or get positions or normals etc. 
		if (updateMeshVertexColorData && colorFromLOD_Local.Num() == meshLODTotalAmountOfVerts_Local)
			vertexDetectMeshData_Global.meshDataPerLOD[currentLOD_Local].meshVertexColorsPerLODArray = colorFromLOD_Local;

		vertexDetectMeshData_Global.meshDataPerLOD[currentLOD_Local].lod = currentLOD_Local;
		vertexDetectMeshData_Global.meshDataPerLOD[currentLOD_Local].meshVertexPositionsInComponentSpacePerLODArray = positionsInComponentLocalSpaceFromLOD_Local;
		vertexDetectMeshData_Global.meshDataPerLOD[currentLOD_Local].meshVertexNormalsPerLODArray = normalsFromLOD_Local;


		if (calculateColorsInfo_Global.vertexPaintSkelComponent) {

			// Had to have an array of a color array that i add to instead of adding directly to the TMap since then i had to use .Contain and .FindRef which made it so each task took longer to finish
			if (currentLOD_Local == 0 && calculateColorsInfo_Global.fundementalStruct.callbackSettings.includeColorsOfEachBone) {

				TMap<FName, FVertexDetectBoneColorsStruct> nameAndColorOfEachBone_Local;

				for (int i = 0; i < boneIndexColors_Local.Num(); i++) {

					// If have added colors, i.e. a valid bone
					if (boneIndexColors_Local[i].Num() > 0) {

						FVertexDetectBoneColorsStruct boneColorsStructTemp;
						boneColorsStructTemp.boneVertexColors = boneIndexColors_Local[i];

						nameAndColorOfEachBone_Local.Add(calculateColorsInfo_Global.vertexPaintSkelComponent->GetBoneName(i), boneColorsStructTemp);
					}
				}

				nameAndColorOfEachBone_Global = nameAndColorOfEachBone_Local;
				successFullyGotColorsForEachBone_Global = true;
			}

			// Affected Cloth Physics is exclusive to UE5 and ChaosCloth
#if ENGINE_MAJOR_VERSION == 5


			// If finished painting on LOD0 on skeletal meshes then checks if any cloth has been painted, and if more than half of the cloths vertexes are painted with a color
			if (calculateColorsInfo_Global.vertexPaintSettings.affectClothPhysics && shouldApplyVertexColors_Global && clothPaintedColors_Local.Num() > 0 && paintedCloths_Local.Num() > 0 && currentLOD_Local == calculateColorsInfo_Global.vertexPaintSkelComponent->GetPredictedLODLevel()) {


				const FSkeletalMeshLODRenderData& skelMeshRenderDataTemp = calculateColorsInfo_Global.vertexPaintSkelComponent->GetSkeletalMeshRenderData()->LODRenderData[currentLOD_Local];
				FSkinWeightVertexBuffer& skinWeightBufferTemp = *calculateColorsInfo_Global.vertexPaintSkelComponent->GetSkinWeightBuffer(currentLOD_Local);


				if (skelMeshRenderDataTemp.HasClothData()) {


					for (int j = 0; j < paintedCloths_Local.Num(); j++) {

						if (!paintedCloths_Local[j]) continue;

						TArray<FColor> paintedClothsColors_Local = clothPaintedColors_Local[j];
						FVertexDetectClothSettingsStruct physicsSettingsAtColorTemp = clothsToColorAndPhysicsSettings_Local.FindRef(paintedCloths_Local[j]);


						// While we loop through the verts we also get the avarage color for each cloth, so when we here at the end want to affect cloth physics depending on the avarage color, we already got it and doesn't have to loop through it again
						FVertexDetectRGBAStruct avarageClothColorFloat_Local = paintedClothsAndAvarageColor_Local[j];


						// Divides total vertex colors with the cloths total amount of verts to get the avarage
						avarageClothColorFloat_Local.redColor = avarageClothColorFloat_Local.redColor / paintedClothsColors_Local.Num();
						avarageClothColorFloat_Local.greenColor = avarageClothColorFloat_Local.greenColor / paintedClothsColors_Local.Num();
						avarageClothColorFloat_Local.blueColor = avarageClothColorFloat_Local.blueColor / paintedClothsColors_Local.Num();
						avarageClothColorFloat_Local.alphaColor = avarageClothColorFloat_Local.alphaColor / paintedClothsColors_Local.Num();


						FLinearColor avarageClothColorLinearColor_Local;
						avarageClothColorLinearColor_Local.R = UKismetMathLibrary::MapRangeClamped(avarageClothColorFloat_Local.redColor, 0, 255, 0, 1);
						avarageClothColorLinearColor_Local.G = UKismetMathLibrary::MapRangeClamped(avarageClothColorFloat_Local.greenColor, 0, 255, 0, 1);
						avarageClothColorLinearColor_Local.B = UKismetMathLibrary::MapRangeClamped(avarageClothColorFloat_Local.blueColor, 0, 255, 0, 1);
						avarageClothColorLinearColor_Local.A = UKismetMathLibrary::MapRangeClamped(avarageClothColorFloat_Local.alphaColor, 0, 255, 0, 1);


						auto chaosClothPhysicsSettings_Local = UVertexPaintFunctionLibrary::GetChaosClothPhysicsSettingsBasedOnVertexColors(paintedCloths_Local[j], paintedClothsColors_Local, physicsSettingsAtColorTemp, calculateColorsInfo_Global.vertexPaintSkelComponent, true, avarageClothColorLinearColor_Local);


						clothPhysicsSettings_Global.Add(paintedCloths_Local[j], chaosClothPhysicsSettings_Local);
					}
				}
			}
#endif

		}

		if (updateMeshVertexColorData && vertexDetectMeshData_Global.meshDataPerLOD.Num() > 1 && calculateColorsInfo_Global.propogateLOD0ToAllLODs) {

			completeLODsBaseBounds_Local[currentLOD_Local] = currentLODBaseBounds_Local;
		}
	}


	// Finished Looping Through All LODs


	calculateColorsResultMessage.Add(FString::Printf(TEXT("Runtime Vertex Color Paint and Detection Plugin - CalculateColorsToApply() - Finished Looping through all LODs")));


	// If > 0 LODs and paint entire mesh at random vertices we can propogate LOD0 Colors to the rest of the LODs so the random vertices that was selected gets the closest LOD1 colors painted as well etc. so they match. 
	if (updateMeshVertexColorData && vertexDetectMeshData_Global.meshDataPerLOD.Num() > 1 && calculateColorsInfo_Global.propogateLOD0ToAllLODs) {

		calculateColorsResultMessage.Add(FString::Printf(TEXT("Runtime Vertex Color Paint and Detection Plugin - Propogating LOD0 Colors to LODs using Modified Engine Method on Mesh Component: %s"), *calculateColorsInfo_Global.vertexPaintComponent->GetName()));

		PropogateLOD0VertexColorsToLODs(paintedVerticesAtLOD0_Local, vertexDetectMeshData_Global, completeLODsBaseBounds_Local, newColorVertexBufferPerLOD_Global, calculateColorsInfo_Global.vertexPaintComponent);
	}


	// Another final check 
	if (IsTaskValid(0) != EVertexPaintDetectionTaskResult::Success) return IsTaskValid(0);


	return EVertexPaintDetectionTaskResult::Success;
}


//-------------------------------------------------------

// Get Vertex Colors At LOD

TArray<FColor> FVertexPaintDetectionAsyncTasks::GetVertexColorsAtLOD(int lod, UPrimitiveComponent * component) {

	if (auto skeletalMeshComponent = Cast<USkeletalMeshComponent>(component)) {

		return UVertexPaintFunctionLibrary::VertexPaintDetectionPlugin_GetSkeletalMeshVertexColorsAtLOD(skeletalMeshComponent, lod);
	}

	else if (auto staticMeshComponent = Cast<UStaticMeshComponent>(component)) {

		return UVertexPaintFunctionLibrary::VertexPaintDetectionPlugin_GetStaticMeshVertexColorsAtLOD(staticMeshComponent, lod);
	}

	return TArray<FColor>();
}


//-------------------------------------------------------

// Should Run Calculate Colors To Apply

bool FVertexPaintDetectionAsyncTasks::ShouldLoopThroughVerticesAndLODs(FVertexDetectEventSettings callbackSettings, UPrimitiveComponent * component) {

	return (callbackSettings.compareMeshColorsToColorArray.compareMeshColorsToColorArrayAtLOD0 || callbackSettings.includeSerializedVertexColorData || callbackSettings.includeVertexPositionData || callbackSettings.includeVertexNormalData || (callbackSettings.includeColorsOfEachBone && Cast<USkeletalMeshComponent>(component)));
}


//-------------------------------------------------------

// Paint Job Should Run Calculate Colors To Apply

bool FVertexPaintDetectionAsyncTasks::PaintJobShouldLoopThroughVerticesAndLODS(FVertexPaintColorSettingStruct paintColorSettings) {

	return (paintColorSettings.limitPaintColor.usePaintLimits || paintColorSettings.onlyAffectVerticesWithDirectionToLocation.onlyAffectVerticesWithDirectionToActorOrLocation || ShouldLoopThroughVerticesAndLODs(paintColorSettings.callbackSettings, paintColorSettings.meshComponent));
}


//-------------------------------------------------------

// Modified Get Typed Skinned Vertex Position - If no clothing then gets the position in component space using our own GetTypedSkinnedVertexPosition, which is otherwise called on each vertex if running ComputeSkinnedPositions. The reason why we've made our own here is so we only have to loop through a mesh sections and vertices once, instead of atleast twice if running ComputeSkinnedPositions, since it also loops through sections and vertices, then if we where to use that here we would have to loop through it again. 


#if ENGINE_MAJOR_VERSION == 4

FVector FVertexPaintDetectionAsyncTasks::Modified_GetTypedSkinnedVertexPosition(const USkinnedMeshComponent * SkinnedComp, const FSkelMeshRenderSection & Section, const FPositionVertexBuffer & PositionVertexBuffer, const FSkinWeightVertexBuffer & SkinWeightVertexBuffer, const int32 VertIndex, const TArray<FMatrix>&RefToLocals, uint32 & boneIndex) {

	boneIndex = 0;

#elif ENGINE_MAJOR_VERSION == 5

FVector FVertexPaintDetectionAsyncTasks::Modified_GetTypedSkinnedVertexPosition(const USkinnedMeshComponent * SkinnedComp, const FSkelMeshRenderSection & Section, const FPositionVertexBuffer & PositionVertexBuffer, const FSkinWeightVertexBuffer & SkinWeightVertexBuffer, const int32 VertIndex, const TArray<FMatrix44f>&RefToLocals, uint32 & boneIndex) {

#endif


	GetTypedSkinnedVertexPosition_SkinnedPos_Global = FVector(ForceInitToZero);



#if ENGINE_MAJOR_VERSION == 4

	const USkinnedMeshComponent* const MasterPoseComponentInst = SkinnedComp->MasterPoseComponent.Get();

#elif ENGINE_MAJOR_VERSION == 5

#if ENGINE_MINOR_VERSION == 0

	const USkinnedMeshComponent* const MasterPoseComponentInst = SkinnedComp->MasterPoseComponent.Get();


#else

	const USkinnedMeshComponent* const MasterPoseComponentInst = SkinnedComp->LeaderPoseComponent.Get();

#endif
#endif


	const USkinnedMeshComponent* BaseComponent = MasterPoseComponentInst ? MasterPoseComponentInst : SkinnedComp;


	// Do soft skinning for this vertex.
	GetTypedSkinnedVertexPosition_BufferVertIndex_Global = Section.GetVertexBufferIndex() + VertIndex;
	GetTypedSkinnedVertexPosition_MaxBoneInfluences_Global = SkinWeightVertexBuffer.GetMaxBoneInfluences();


#if !PLATFORM_LITTLE_ENDIAN
	// uint8[] elements in LOD.VertexBufferGPUSkin have been swapped for VET_UBYTE4 vertex stream use
	for (int32 InfluenceIndex = MAX_INFLUENCES - 1; InfluenceIndex >= MAX_INFLUENCES - MaxBoneInfluences; InfluenceIndex--)
#else
	for (int32 InfluenceIndex = 0; InfluenceIndex < GetTypedSkinnedVertexPosition_MaxBoneInfluences_Global; InfluenceIndex++)
#endif
	{

		GetTypedSkinnedVertexPosition_MeshBoneIndex_Global = 0;

		if (Section.BoneMap.IsValidIndex(SkinWeightVertexBuffer.GetBoneIndex(GetTypedSkinnedVertexPosition_BufferVertIndex_Global, InfluenceIndex)))
			GetTypedSkinnedVertexPosition_MeshBoneIndex_Global = Section.BoneMap[SkinWeightVertexBuffer.GetBoneIndex(GetTypedSkinnedVertexPosition_BufferVertIndex_Global, InfluenceIndex)];

		if (InfluenceIndex == 0)
			boneIndex = GetTypedSkinnedVertexPosition_MeshBoneIndex_Global;

		GetTypedSkinnedVertexPosition_TransformBoneIndex_Global = GetTypedSkinnedVertexPosition_MeshBoneIndex_Global;

		if (MasterPoseComponentInst) {


#if ENGINE_MAJOR_VERSION == 4

			const TArray<int32>& MasterBoneMap = SkinnedComp->GetMasterBoneMap();

#elif ENGINE_MAJOR_VERSION == 5

#if ENGINE_MINOR_VERSION == 0

			const TArray<int32>& MasterBoneMap = SkinnedComp->GetMasterBoneMap();

#else

			const TArray<int32>& MasterBoneMap = SkinnedComp->GetLeaderBoneMap();

#endif

#endif


#if ENGINE_MAJOR_VERSION == 4

			check(MasterBoneMap.Num() == SkinnedComp->SkeletalMesh->GetRefSkeleton().GetNum());

#elif ENGINE_MAJOR_VERSION == 5

#if ENGINE_MINOR_VERSION == 0

			check(MasterBoneMap.Num() == SkinnedComp->SkeletalMesh.Get()->GetRefSkeleton().GetNum());

#else

			check(MasterBoneMap.Num() == SkinnedComp->GetSkinnedAsset()->GetRefSkeleton().GetNum());

#endif
#endif

			GetTypedSkinnedVertexPosition_TransformBoneIndex_Global = MasterBoneMap[GetTypedSkinnedVertexPosition_MeshBoneIndex_Global];
		}


		GetTypedSkinnedVertexPosition_Weight_Global = (float)(SkinWeightVertexBuffer.GetBoneWeight(GetTypedSkinnedVertexPosition_BufferVertIndex_Global, InfluenceIndex) / 255.0f);;

#if ENGINE_MAJOR_VERSION == 5 
#if ENGINE_MINOR_VERSION >= 2

		// From 5.2 it seems it uses AnimationCore::InvMaxRawBoneWeightFloat to calculate the weight
		GetTypedSkinnedVertexPosition_Weight_Global = (float)SkinWeightVertexBuffer.GetBoneWeight(GetTypedSkinnedVertexPosition_BufferVertIndex_Global, InfluenceIndex) * UE::AnimationCore::InvMaxRawBoneWeightFloat;

#endif
#endif


		// Could get a crash here very rarely if i painted like crazy while switching meshes, skeletal meshes, this seems to have fixed that issue!
		if (GetTypedSkinnedVertexPosition_BufferVertIndex_Global < (int32)PositionVertexBuffer.GetNumVertices() && GetTypedSkinnedVertexPosition_MeshBoneIndex_Global < RefToLocals.Num()) {

			GetTypedSkinnedVertexPosition_SkinnedPos_Global += (FVector)RefToLocals[GetTypedSkinnedVertexPosition_MeshBoneIndex_Global].TransformPosition(PositionVertexBuffer.VertexPosition(GetTypedSkinnedVertexPosition_BufferVertIndex_Global)) * GetTypedSkinnedVertexPosition_Weight_Global;
		}
	}

	return GetTypedSkinnedVertexPosition_SkinnedPos_Global;
}


//-------------------------------------------------------

// Is Vertex Within Area

bool FVertexPaintDetectionAsyncTasks::IsVertexWithinArea(const FVector & vertexWorldPosition, const FVertexPainthWithinAreaStruct & vertexPaintWithinAreaStruct, const FVertexPainthWithinArea_ComponentToCheckIfIsWithinInfo & componentToCheckIfIsWithinInfo) {


	switch (componentToCheckIfIsWithinInfo.paintWithinAreaShape) {

	case Enum_PaintWithinAreaShape::isSquareOrRectangleShape:


		isVertexWithinArea_SquareXSize_Global = componentToCheckIfIsWithinInfo.componentToCheckIfIsWithin_IsVertexWithinArea_CubeOrRectangle_SquareXHalfSize + vertexPaintWithinAreaStruct.extraExtentToApplyPaintOn;
		isVertexWithinArea_SquareYSize_Global = componentToCheckIfIsWithinInfo.componentToCheckIfIsWithin_IsVertexWithinArea_CubeOrRectangle_SquareYHalfSize + vertexPaintWithinAreaStruct.extraExtentToApplyPaintOn;
		isVertexWithinArea_SquareZSize_Global = componentToCheckIfIsWithinInfo.componentToCheckIfIsWithin_IsVertexWithinArea_CubeOrRectangle_SquareZHalfSize + vertexPaintWithinAreaStruct.extraExtentToApplyPaintOn;


		// Gets Mesh Local Position to the mesh we're checking if within
		isVertexWithinArea_Vertex_LocalPos_Global = UKismetMathLibrary::InverseTransformLocation(componentToCheckIfIsWithinInfo.componentToCheckIfIsWithin_WorldTransform, vertexWorldPosition);


		// Checks each side so even if it's a tilted rectangle we can still know if within area

		// Forward
		isVertexWithinArea_Square_ForwardPosWorld_Global = componentToCheckIfIsWithinInfo.componentToCheckIfIsWithin_WorldTransform.GetLocation() + componentToCheckIfIsWithinInfo.componentToCheckIfIsWithin_ForwardVector * isVertexWithinArea_SquareXSize_Global;
		isVertexWithinArea_Square_ForwardPosLocal_Global = UKismetMathLibrary::InverseTransformLocation(componentToCheckIfIsWithinInfo.componentToCheckIfIsWithin_WorldTransform, isVertexWithinArea_Square_ForwardPosWorld_Global);

		// Back
		isVertexWithinArea_Square_BackPosWorld_Global = componentToCheckIfIsWithinInfo.componentToCheckIfIsWithin_WorldTransform.GetLocation() + (componentToCheckIfIsWithinInfo.componentToCheckIfIsWithin_ForwardVector * -1) * isVertexWithinArea_SquareXSize_Global;
		isVertexWithinArea_Square_BackPosLocal_Global = UKismetMathLibrary::InverseTransformLocation(componentToCheckIfIsWithinInfo.componentToCheckIfIsWithin_WorldTransform, isVertexWithinArea_Square_BackPosWorld_Global);

		// Left
		isVertexWithinArea_Square_LeftPosWorld_Global = componentToCheckIfIsWithinInfo.componentToCheckIfIsWithin_WorldTransform.GetLocation() + (componentToCheckIfIsWithinInfo.componentToCheckIfIsWithin_RightVector * -1) * isVertexWithinArea_SquareYSize_Global;
		isVertexWithinArea_Square_LeftPosLocal_Global = UKismetMathLibrary::InverseTransformLocation(componentToCheckIfIsWithinInfo.componentToCheckIfIsWithin_WorldTransform, isVertexWithinArea_Square_LeftPosWorld_Global);

		// Right
		isVertexWithinArea_Square_RightPosWorld_Global = componentToCheckIfIsWithinInfo.componentToCheckIfIsWithin_WorldTransform.GetLocation() + componentToCheckIfIsWithinInfo.componentToCheckIfIsWithin_RightVector * isVertexWithinArea_SquareYSize_Global;
		isVertexWithinArea_Square_RightPosLocal_Global = UKismetMathLibrary::InverseTransformLocation(componentToCheckIfIsWithinInfo.componentToCheckIfIsWithin_WorldTransform, isVertexWithinArea_Square_RightPosWorld_Global);

		// Down
		isVertexWithinArea_Square_DownPosWorld_Global = componentToCheckIfIsWithinInfo.componentToCheckIfIsWithin_WorldTransform.GetLocation() + (componentToCheckIfIsWithinInfo.componentToCheckIfIsWithin_UpVector * -1) * isVertexWithinArea_SquareZSize_Global;
		isVertexWithinArea_Square_DownPosLocal_Global = UKismetMathLibrary::InverseTransformLocation(componentToCheckIfIsWithinInfo.componentToCheckIfIsWithin_WorldTransform, isVertexWithinArea_Square_DownPosWorld_Global);

		// Up
		isVertexWithinArea_Square_UpPosWorld_Global = componentToCheckIfIsWithinInfo.componentToCheckIfIsWithin_WorldTransform.GetLocation() + componentToCheckIfIsWithinInfo.componentToCheckIfIsWithin_UpVector * isVertexWithinArea_SquareZSize_Global;
		isVertexWithinArea_Square_UpPosLocal_Global = UKismetMathLibrary::InverseTransformLocation(componentToCheckIfIsWithinInfo.componentToCheckIfIsWithin_WorldTransform, isVertexWithinArea_Square_UpPosWorld_Global);


		// Is within X, Y and Z Relative Limits of the Shape
		if (isVertexWithinArea_Vertex_LocalPos_Global.X >= isVertexWithinArea_Square_BackPosLocal_Global.X && isVertexWithinArea_Vertex_LocalPos_Global.X <= isVertexWithinArea_Square_ForwardPosLocal_Global.X) {

			if (isVertexWithinArea_Vertex_LocalPos_Global.Y >= isVertexWithinArea_Square_LeftPosLocal_Global.Y && isVertexWithinArea_Vertex_LocalPos_Global.Y <= isVertexWithinArea_Square_RightPosLocal_Global.Y) {

				if (isVertexWithinArea_Vertex_LocalPos_Global.Z >= isVertexWithinArea_Square_DownPosLocal_Global.Z && isVertexWithinArea_Vertex_LocalPos_Global.Z <= isVertexWithinArea_Square_UpPosLocal_Global.Z) {

					return true;
				}
			}
		}

		break;

	case Enum_PaintWithinAreaShape::isSphereShape:

		// If within Radius
		if ((componentToCheckIfIsWithinInfo.componentToCheckIfIsWithin_WorldTransform.GetLocation() - vertexWorldPosition).Size() <= componentToCheckIfIsWithinInfo.componentToCheckIfIsWithin_IsVertexWithinArea_SphereShape + vertexPaintWithinAreaStruct.extraExtentToApplyPaintOn) {

			return true;
		}

		break;

		// Note this is very expensive, it will give a 100% check and work on meshes on any shapes unlike the simple Cube and Spheres below. 
	case Enum_PaintWithinAreaShape::isComplexShape:

		if (componentToCheckIfIsWithinInfo.componentToCheckIfIsWithin) {

			isVertexWithinArea_ShouldTrace = false;
			isVertexWithinArea_TraceEndLocation = vertexWorldPosition + FVector(0.001f, 0.001f, 0.001f);


			// If lansdcape then we also run the trace. This was necessary because the landscape bounds on the flat part didn't start until 50cm above it and it became difficult to make sure that the entire mesh got painted if it was below a flat part of the landscape. This just makes everything so much easier UX wise because otherwise we had to require the user to make a hole somewhere on the landscape to extend the bounds etc. 
			if (componentToCheckIfIsWithinInfo.componentToCheckIfIsWithin_IsLandscape) {

				// To check if we're within the landscape, we trace upwards, so if the vertex is underneath it, it can get a hit on the opposite side. 
				isVertexWithinArea_TraceEndLocation = (componentToCheckIfIsWithinInfo.componentToCheckIfIsWithin->GetUpVector() * 100000.0f) + vertexWorldPosition;
				isVertexWithinArea_ShouldTrace = true;
			}

			// If not a landscape we only run a trace if the vertex is within the bounds. Unlike Cube/Rectangle that has to be Exactly where the mesh is even when rotated, we can use this here even when rotated. Should save some performance so we don't run unnecessary traces. 
			else if (componentToCheckIfIsWithinInfo.componentToCheckIfIsWithin_Bounds.GetBox().IsInsideOrOn(vertexWorldPosition)) {

				isVertexWithinArea_ShouldTrace = true;
			}

			// NOTE it was very expensive if for instance spline meshes that also demand complex shapes Always traced if you for instance ran a paint within area with spline on a expensive mesh. So any further changes has to make sure that logic works, otherwise a task can take a really long time to finish


			if (isVertexWithinArea_ShouldTrace) {

				// DrawDebugLine(calculateColorsInfo_Global.vertexPaintDetectionHitActor->GetWorld(), vertexWorldPosition, isVertexWithinArea_TraceEndLocation, FColor::Red, false, 1, 0, .5);

				isVertexWithinArea_HitResults_Global.Empty();

				// If the vertex is inside the component. Multi trace in case there's something else inside where we're doing the trace
				calculateColorsInfo_Global.worldTaskWasCreatedIn->LineTraceMultiByObjectType(isVertexWithinArea_HitResults_Global, vertexWorldPosition, isVertexWithinArea_TraceEndLocation, componentToCheckIfIsWithinInfo.componentToCheckIfIsWithin->GetCollisionObjectType(), isVertexWithinArea_ComplexShape_TraceParams_Global);

				for (auto hitResultTemp : isVertexWithinArea_HitResults_Global) {

					if (componentToCheckIfIsWithinInfo.componentToCheckIfIsWithin == hitResultTemp.Component.Get()) {

						return true;
					}
				}
			}
		}

		break;

	default:
		break;
	}

	return false;
}


//-------------------------------------------------------

// Is Task And World Valid

EVertexPaintDetectionTaskResult FVertexPaintDetectionAsyncTasks::IsTaskValid(const int& currentLOD) {

	// Necessary in case you stopped playing while tasks being calculated, got crash otherwise
	if (!UVertexPaintFunctionLibrary::IsWorldValid(calculateColorsInfo_Global.worldTaskWasCreatedIn)) return EVertexPaintDetectionTaskResult::Fail;
	if (!IsValid(calculateColorsInfo_Global.vertexPaintDetectionHitActor)) return EVertexPaintDetectionTaskResult::FailWithInvalidMesh;
	if (!IsValid(calculateColorsInfo_Global.vertexPaintComponent)) return EVertexPaintDetectionTaskResult::FailWithInvalidMesh;


	// If the mesh has been changed mid task to something else then abort the calculation immediately
	if (IsValid(calculateColorsInfo_Global.vertexPaintStaticMeshComponent)) {

		if (calculateColorsInfo_Global.vertexPaintSourceMesh != calculateColorsInfo_Global.vertexPaintStaticMeshComponent->GetStaticMesh()) {

			return EVertexPaintDetectionTaskResult::FailWithInvalidMesh;
		}
	}

	else if (calculateColorsInfo_Global.vertexPaintSkelComponent) {


		USkeletalMesh* skelMesh_Local = nullptr;

#if ENGINE_MAJOR_VERSION == 4

		skelMesh_Local = calculateColorsInfo_Global.vertexPaintSkelComponent->SkeletalMesh;

#elif ENGINE_MAJOR_VERSION == 5

#if ENGINE_MINOR_VERSION == 0

		skelMesh_Local = calculateColorsInfo_Global.vertexPaintSkelComponent->SkeletalMesh.Get();

#else

		skelMesh_Local = calculateColorsInfo_Global.vertexPaintSkelComponent->GetSkeletalMeshAsset();

#endif
#endif

		if (calculateColorsInfo_Global.vertexPaintSourceMesh != skelMesh_Local) {

			return EVertexPaintDetectionTaskResult::FailWithInvalidMesh;
		}

		if (!skelMesh_Local->GetResourceForRendering()) {

			return EVertexPaintDetectionTaskResult::Fail;
		}

		if (!calculateColorsInfo_Global.vertexPaintSkelComponent->GetSkeletalMeshRenderData()->IsInitialized()) {

			return EVertexPaintDetectionTaskResult::Fail;
		}

		if (!skelMesh_Local->GetResourceForRendering()->LODRenderData.IsValidIndex(currentLOD)) {

			calculateColorsResultMessage.Add(FString::Printf(TEXT("Runtime Vertex Color Paint and Detection Plugin - Async Task Failed, trying to access invalid LOD from GetResourceForRendering->LODRenderData!")));

			return EVertexPaintDetectionTaskResult::Fail;
		}

		if (!calculateColorsInfo_Global.vertexPaintSkelComponent->GetSkeletalMeshRenderData()->LODRenderData.IsValidIndex(currentLOD)) {

			calculateColorsResultMessage.Add(FString::Printf(TEXT("Runtime Vertex Color Paint and Detection Plugin - Async Task Failed, trying to access invalid LOD from GetSkeletalMeshRenderData->LODRenderData")));

			return EVertexPaintDetectionTaskResult::Fail;
		}


		const FSkeletalMeshLODRenderData& skelMeshRenderData_Local = calculateColorsInfo_Global.vertexPaintSkelComponent->GetSkeletalMeshRenderData()->LODRenderData[currentLOD];

		// Extremely rarely got crashes when run .VertexTangentZ below on StaticMeshVertexBuffer ln 217 when switching skeletal meshes and painting every frame. This seems to have fixed that issue. 
		if (!skelMeshRenderData_Local.StaticVertexBuffers.StaticMeshVertexBuffer.IsInitialized() || !skelMeshRenderData_Local.StaticVertexBuffers.StaticMeshVertexBuffer.TangentsVertexBuffer.IsInitialized() || !skelMeshRenderData_Local.StaticVertexBuffers.StaticMeshVertexBuffer.GetTangentData()) {


			calculateColorsResultMessage.Add(FString::Printf(TEXT("Runtime Vertex Color Paint and Detection Plugin - Async Task Failed because static mesh tangent vertex buffer isn't initialized!")));

			return EVertexPaintDetectionTaskResult::Fail;
		}
	}

	return EVertexPaintDetectionTaskResult::Success;
}


//-------------------------------------------------------

// Is Vertex Close To Estimated Color To Hit Location

void FVertexPaintDetectionAsyncTasks::IsVertexCloseToEstimatedColorToHitLocation(float& currentLongestDistance, int& currentLongestDistanceIndex, TMap<int, FDirectionFromHitLocationToClosestVerticesInfo>&directionFromHitToClosestVertices, const FVector & hitLocationInWorldSpace, const FVector & vertexInWorldSpace, const int& vertexLODIndex) {

	if (vertexLODIndex < 0) return;


	int directionFromHitToClosestVerticesInfo_IndexAddedTemp = -1;

	// If less than 9 then always adds then and doesn't require a distance check
	if (directionFromHitToClosestVertices.Num() < 9) {

		directionFromHitToClosestVerticesInfo_IndexAddedTemp = directionFromHitToClosestVertices.Num();
	}

	// If we've already filled up 9 closest verts then we require that the current vertex has to be at least shorter than the longest in the TMap for it to replace it. 
	else if ((hitLocationInWorldSpace - vertexInWorldSpace).Size() < currentLongestDistance) {

		// Replaces the index where the longest distance used to be, so it's not a part of the TMap
		directionFromHitToClosestVerticesInfo_IndexAddedTemp = currentLongestDistanceIndex;
	}


	// If we should replace any index because we either had < 9 in the TMap, or the current vertex had shorter distance than the longest in the TMap
	if (directionFromHitToClosestVerticesInfo_IndexAddedTemp != -1) {


		FDirectionFromHitLocationToClosestVerticesInfo directionFromHitToClosestVerticesInfo_NewEntry;
		directionFromHitToClosestVerticesInfo_NewEntry.lodVertexIndex = vertexLODIndex;
		directionFromHitToClosestVerticesInfo_NewEntry.vertexWorldPosition = vertexInWorldSpace;
		directionFromHitToClosestVerticesInfo_NewEntry.directionFromHitDirectionToVertex = UKismetMathLibrary::GetDirectionUnitVector(hitLocationInWorldSpace, vertexInWorldSpace);
		directionFromHitToClosestVerticesInfo_NewEntry.distanceFromHitLocation = (hitLocationInWorldSpace - vertexInWorldSpace).Size();


		directionFromHitToClosestVertices.Add(directionFromHitToClosestVerticesInfo_IndexAddedTemp, directionFromHitToClosestVerticesInfo_NewEntry);

		// Loops through the current in the TMap when a new one gets added so we can update which is of them has the longest distance, so we can compare against that the next loop
		TArray<FDirectionFromHitLocationToClosestVerticesInfo> directionFromHitToClosestVerticesInfo_ValuesTemp;
		directionFromHitToClosestVertices.GenerateValueArray(directionFromHitToClosestVerticesInfo_ValuesTemp);

		int indexWithLongestDistanceTemp = 0;
		float longestDistanceToVertexTemp = directionFromHitToClosestVerticesInfo_ValuesTemp[0].distanceFromHitLocation;

		for (int i = 0; i < directionFromHitToClosestVerticesInfo_ValuesTemp.Num(); i++) {


			if (directionFromHitToClosestVerticesInfo_ValuesTemp[i].distanceFromHitLocation > longestDistanceToVertexTemp) {

				indexWithLongestDistanceTemp = i;
				longestDistanceToVertexTemp = directionFromHitToClosestVerticesInfo_ValuesTemp[i].distanceFromHitLocation;
			}
		}

		// Updates the cached values so these are the new longestDistance and index, so we can check against the distance the next loop
		currentLongestDistanceIndex = indexWithLongestDistanceTemp;
		currentLongestDistance = longestDistanceToVertexTemp;
	}
}


//-------------------------------------------------------

// Get Estimated Color To Hit Location Vertex To Lerp Color Toward

int FVertexPaintDetectionAsyncTasks::GetEstimatedColorToHitLocationVertexToLerpColorToward(const TMap<int, FDirectionFromHitLocationToClosestVerticesInfo>&directionFromHitToClosestVertices, TArray<FVector>&closestVerticesToTheHitLocationInWorldLocation, const FVector & directionToClosestVertex, const int& closestVertexBased, const float& dotGraceRange) {

	if (closestVertexBased <= 0) return -1;
	if (directionFromHitToClosestVertices.Num() <= 0) return -1;


	int closestVertexToLerpTowardIndex_Temp = -1;
	float closestVertexToLerpTowardDistance_Temp = 1000000;
	float closestVertexToLerpTowardDot_Temp = -1;

	float currentDotTemp = -1;
	float currentDistanceTemp = 0;

	// The direction from the hit location to the closest vertex * -1, i.e the direction we want to get the vertex to scale to
	FVector directionFromClosestVertexToHitLocation_Temp = directionToClosestVertex * -1;

	TArray< FDirectionFromHitLocationToClosestVerticesInfo> directionFromHitLocInfoTemp;
	directionFromHitToClosestVertices.GenerateValueArray(directionFromHitLocInfoTemp);


	for (int i = 0; i < directionFromHitLocInfoTemp.Num(); i++) {


		closestVerticesToTheHitLocationInWorldLocation.Add(directionFromHitLocInfoTemp[i].vertexWorldPosition);

		currentDotTemp = FVector::DotProduct(directionFromClosestVertexToHitLocation_Temp, directionFromHitLocInfoTemp[i].directionFromHitDirectionToVertex);
		currentDistanceTemp = directionFromHitLocInfoTemp[i].distanceFromHitLocation;


		if (currentDotTemp > closestVertexToLerpTowardDot_Temp) {

			// If has better dot than the current best dot, but the current best with added grace range is equal or better, and has shorter range, then we keep it since short distance is high prio
			if ((closestVertexToLerpTowardDot_Temp + dotGraceRange) >= currentDotTemp && closestVertexToLerpTowardDistance_Temp < currentDistanceTemp) {

				//
			}
			else {

				closestVertexToLerpTowardDot_Temp = currentDotTemp;
				closestVertexToLerpTowardDistance_Temp = currentDistanceTemp;
				closestVertexToLerpTowardIndex_Temp = directionFromHitLocInfoTemp[i].lodVertexIndex;
			}
		}

		// If doesn't have better dot, but with added grace period is better or equal, and has shorter range. So if something is closer, but has slightly worse dot, then it may still be choosen since distance has high prio
		else if ((currentDotTemp + dotGraceRange) >= closestVertexToLerpTowardDot_Temp && currentDistanceTemp < closestVertexToLerpTowardDistance_Temp) {

			closestVertexToLerpTowardDot_Temp = currentDotTemp;
			closestVertexToLerpTowardDistance_Temp = currentDistanceTemp;
			closestVertexToLerpTowardIndex_Temp = directionFromHitLocInfoTemp[i].lodVertexIndex;
		}
	}

	return closestVertexToLerpTowardIndex_Temp;
}


//-------------------------------------------------------

// Get Estimated Color To Hit Location Values

void FVertexPaintDetectionAsyncTasks::GetEstimatedColorToHitLocationValues(const int& vertexToLerpToward, const FColor & closestVertexColor, const FVector & closestVertexInWorldSpace, const FVector & hitLocationInWorldSpace, const FColor & vertexToLerpTowardColor, const FVector & vertexToLerpTowardPositionInWorldSpace, FLinearColor & estimatedColorAtHitLocation, FVector & estimatedHitLocation) {


	// If got vertex to lerp toward, then sets colorToHitLocation_AfterApplyingColors_Paint_Global 
	if (vertexToLerpToward >= 0) {

		// Distance from Closest Paint to Hit Location
		float distanceFromClosestVertexToHitLocationTemp = (hitLocationInWorldSpace - closestVertexInWorldSpace).Size();

		// Distance to from closest vertex paint to vertex to lerp toward
		float distanceFromClosestVertexToVertexWeLerpTowardTemp = (closestVertexInWorldSpace - vertexToLerpTowardPositionInWorldSpace).Size();


		// Depending on the distance from the closest vertex position to the vertex we lerp toward position, we convert to Linear and scale the color so it should pretty accurate to what the actual Hit Location Color Should be. 
		estimatedColorAtHitLocation = UKismetMathLibrary::LinearColorLerp(closestVertexColor.ReinterpretAsLinear(), vertexToLerpTowardColor.ReinterpretAsLinear(), UKismetMathLibrary::MapRangeClamped(distanceFromClosestVertexToHitLocationTemp, 0, distanceFromClosestVertexToVertexWeLerpTowardTemp, 0, 1));

		estimatedHitLocation = UKismetMathLibrary::VLerp(closestVertexInWorldSpace, vertexToLerpTowardPositionInWorldSpace, UKismetMathLibrary::MapRangeClamped(distanceFromClosestVertexToHitLocationTemp, 0, distanceFromClosestVertexToVertexWeLerpTowardTemp, 0, 1));
	}
}


//-------------------------------------------------------

// Get Spline Mesh Vertex Position In Mesh Space

FVector FVertexPaintDetectionAsyncTasks::GetSplineMeshVertexPositionInMeshSpace(const FVector & vertexPos, USplineMeshComponent * splineMeshComponent) {

	if (!IsValid(splineMeshComponent)) return FVector(0, 0, 0);

	FVector slicePos = vertexPos;
	slicePos[splineMeshComponent->ForwardAxis] = 0;
	const FVector SplineMeshSpaceVector = splineMeshComponent->CalcSliceTransform(vertexPos[splineMeshComponent->ForwardAxis]).TransformPosition(slicePos);

	return SplineMeshSpaceVector;
}


//-------------------------------------------------------

// Get Spline Mesh Vertex Normal In Mesh Space

FVector FVertexPaintDetectionAsyncTasks::GetSplineMeshVertexNormalInMeshSpace(const FVector & normal, USplineMeshComponent * splineMeshComponent) {

	if (!IsValid(splineMeshComponent)) return FVector(0, 0, 0);

	FVector sliceNormal = normal;
	sliceNormal[splineMeshComponent->ForwardAxis] = 0;
	const FVector SplineMeshSpaceVector = splineMeshComponent->CalcSliceTransform(sliceNormal[splineMeshComponent->ForwardAxis]).TransformVector(sliceNormal);

	return SplineMeshSpaceVector;
}


//-------------------------------------------------------

// Is Current Vertex Color Within Conditions

bool FVertexPaintDetectionAsyncTasks::IsCurrentVertexWithinPaintColorConditions(const FColor & currentVertexColor, const FVector & currentVertexPosition, const FVector & currentVertexNormal, const FName & currentBoneName, const FVertexDetectPaintColorConditionStruct & paintConditions, bool& applyColorStrengthIfConditionIsNotMet, float& colorStrengthIfConditionsIsNotMet) {

	applyColorStrengthIfConditionIsNotMet = false;
	colorStrengthIfConditionsIsNotMet = 0;

	applyColorStrengthIfConditionIsNotMet = paintConditions.applyColorStrengthIfConditionIsNotMet;

	if (applyColorStrengthIfConditionIsNotMet)
		colorStrengthIfConditionsIsNotMet = paintConditions.colorStrengthIfConditionIsNotMet;



	for (auto withinRangeCondition : paintConditions.ifVertexColorIsWithinRange) {


		switch (withinRangeCondition.ifVertexColorChannelWithinColorRange) {

		case Enum_SurfaceAtChannel::RedChannel:

			if (currentVertexColor.R >= UKismetMathLibrary::MapRangeClamped(withinRangeCondition.ifCurrentVertexColorValueIsHigherOrEqualThan, 0, 1, 0, 255) && currentVertexColor.R <= UKismetMathLibrary::MapRangeClamped(withinRangeCondition.ifCurrentVertexColorValueIsLessOrEqualThan, 0, 1, 0, 255)) {

				// UE_LOG(LogTemp, Warning, TEXT("IsCurrentVertexWithinPaintColorConditions() If Red Channel is Within Color Range! Higher or EqualThan: %f  -   ifCurrentVertexColorValueIsLessOrEqualThan: %f"), withinRangeCondition.ifCurrentVertexColorValueIsHigherOrEqualThan, withinRangeCondition.ifCurrentVertexColorValueIsLessOrEqualThan);
			}
			else {

				return false;
			}

			break;

		case Enum_SurfaceAtChannel::GreenChannel:

			if (currentVertexColor.G >= UKismetMathLibrary::MapRangeClamped(withinRangeCondition.ifCurrentVertexColorValueIsHigherOrEqualThan, 0, 1, 0, 255) && currentVertexColor.G <= UKismetMathLibrary::MapRangeClamped(withinRangeCondition.ifCurrentVertexColorValueIsLessOrEqualThan, 0, 1, 0, 255)) {

				// UE_LOG(LogTemp, Warning, TEXT("IsCurrentVertexWithinPaintColorConditions() If Green Channel is Within Color Range! Higher or EqualThan: %f  -   ifCurrentVertexColorValueIsLessOrEqualThan: %f"), withinRangeCondition.ifCurrentVertexColorValueIsHigherOrEqualThan, withinRangeCondition.ifCurrentVertexColorValueIsLessOrEqualThan);
			}
			else {

				return false;
			}

			break;

		case Enum_SurfaceAtChannel::BlueChannel:

			if (currentVertexColor.B >= UKismetMathLibrary::MapRangeClamped(withinRangeCondition.ifCurrentVertexColorValueIsHigherOrEqualThan, 0, 1, 0, 255) && currentVertexColor.B <= UKismetMathLibrary::MapRangeClamped(withinRangeCondition.ifCurrentVertexColorValueIsLessOrEqualThan, 0, 1, 0, 255)) {

				// UE_LOG(LogTemp, Warning, TEXT("IsCurrentVertexWithinPaintColorConditions() If Blue Channel is Within Color Range! Higher or EqualThan: %f  -   ifCurrentVertexColorValueIsLessOrEqualThan: %f"), withinRangeCondition.ifCurrentVertexColorValueIsHigherOrEqualThan, withinRangeCondition.ifCurrentVertexColorValueIsLessOrEqualThan);
			}
			else {

				return false;
			}

			break;

		case Enum_SurfaceAtChannel::AlphaChannel:

			if (currentVertexColor.A >= UKismetMathLibrary::MapRangeClamped(withinRangeCondition.ifCurrentVertexColorValueIsHigherOrEqualThan, 0, 1, 0, 255) && currentVertexColor.A <= UKismetMathLibrary::MapRangeClamped(withinRangeCondition.ifCurrentVertexColorValueIsLessOrEqualThan, 0, 1, 0, 255)) {

				// UE_LOG(LogTemp, Warning, TEXT("IsCurrentVertexWithinPaintColorConditions() If Alpha Channel is Within Color Range! Higher or EqualThan: %f  -   ifCurrentVertexColorValueIsLessOrEqualThan: %f"), withinRangeCondition.ifCurrentVertexColorValueIsHigherOrEqualThan, withinRangeCondition.ifCurrentVertexColorValueIsLessOrEqualThan);
			}
			else {

				return false;
			}

			break;

		default:
			break;
		}
	}

	for (auto withinLineOfSightCondition : paintConditions.ifVertexHasLineOfSightTo) {

		EDrawDebugTrace::Type debugTrace_Local = EDrawDebugTrace::None;
		if (calculateColorsInfo_Global.drawPaintConditionsDebugSymbols && !calculateColorsInfo_Global.fundementalStruct.multiThreadSettings.useMultithreadingForCalculations)
			debugTrace_Local = EDrawDebugTrace::ForDuration;


		FHitResult hitResult_Local;
		if (UKismetSystemLibrary::LineTraceSingleForObjects(calculateColorsInfo_Global.vertexPaintComponent, currentVertexPosition + (currentVertexNormal * withinLineOfSightCondition.distanceFromVertexPositionToStartTrace), withinLineOfSightCondition.ifVertexHasLineOfSightToPosition, withinLineOfSightCondition.checkLineOfSightAgainstObjectTypes, false, withinLineOfSightCondition.lineOfSightTraceActorsToIgnore, debugTrace_Local, hitResult_Local, withinLineOfSightCondition.ignoreMeshActorTryingToApplyPaintOn, FLinearColor::Green, FLinearColor::Red, calculateColorsInfo_Global.drawPaintConditionsDebugSymbolsDuration)) {

			// If something is blocking, i.e. the vertex doesn't have line of sight
			return false;
		}
	}


	if (paintConditions.ifVertexIsOnBone.Num() > 0 && calculateColorsInfo_Global.vertexPaintSkelComponent) {

		paintCondition_AtAnyBone_Global = false;

		for (auto onBoneCondition : paintConditions.ifVertexIsOnBone) {

			if (onBoneCondition.ifVertexIsAtBone == currentBoneName) {

				paintCondition_AtAnyBone_Global = true;
				break;
			}
		}

		// If still false if we had bone conditions it means that we're not on the right bone
		if (!paintCondition_AtAnyBone_Global)
			return false;
	}

	// If haven't returned false by now, i.e. haven't failed a condition, then we return true.
	return true;
}


//-------------------------------------------------------

// Will Applied Paint Make Change

bool FVertexPaintDetectionAsyncTasks::WillAppliedColorMakeChangeOnVertex(const FColor & currentColorOnVertex, const FVector & currentLocationOnVertex, const FVector & currentNormalOnVertex, const FVertexPaintColorSettingStruct & paintColorSettings) {


	if (paintColorSettings.onlyAffectVerticesWithDirectionToLocation.onlyAffectVerticesWithDirectionToActorOrLocation) {

		if (UKismetMathLibrary::Dot_VectorVector(currentNormalOnVertex, UKismetMathLibrary::GetDirectionUnitVector(currentLocationOnVertex, paintColorSettings.onlyAffectVerticesWithDirectionToLocation.location)) > paintColorSettings.onlyAffectVerticesWithDirectionToLocation.minDotProductToActorOrLocation) {

			// Doesn't return true since we want it to do the Amount of Color Check below, in case it fails there where no channel is going to make adifference, eventhough it has the correct direction
		}
		else {

			return false;
		}
	}


	if (paintColorSettings.applyVertexColorSettings.applyVertexColorSetting == EApplyVertexColorSetting::EAddVertexColor) {

		// If color is maxed then the strength has to be -, i.e. the player has to try to remove it. If the color is at 0, then the player has to add to it. 

		// Red
		if (paintColorSettings.applyVertexColorSettings.redColor > 0 || paintColorSettings.applyVertexColorSettings.redColor < 0) {


			// If in between min and max and we're trying to Add/Remove
			if (currentColorOnVertex.R > 0 || currentColorOnVertex.R < 255) {

				return true;
			}

			// If 0 and trying to Add, or 255 and trying to Remove
			else if ((currentColorOnVertex.R <= 0 && paintColorSettings.applyVertexColorSettings.redColor > 0) || (currentColorOnVertex.R >= 255 && paintColorSettings.applyVertexColorSettings.redColor < 0)) {

				return true;
			}
		}

		// Green
		if (paintColorSettings.applyVertexColorSettings.greenColor > 0 || paintColorSettings.applyVertexColorSettings.greenColor < 0) {


			// If in between min and max and we're trying to Add/Remove
			if (currentColorOnVertex.G > 0 || currentColorOnVertex.G < 255) {

				return true;
			}

			// If 0 and trying to Add, or 255 and trying to Remove
			else if ((currentColorOnVertex.G <= 0 && paintColorSettings.applyVertexColorSettings.greenColor > 0) || (currentColorOnVertex.G >= 255 && paintColorSettings.applyVertexColorSettings.greenColor < 0)) {

				return true;
			}
		}

		// Blue
		if (paintColorSettings.applyVertexColorSettings.blueColor > 0 || paintColorSettings.applyVertexColorSettings.blueColor < 0) {


			// If in between min and max and we're trying to Add/Remove
			if (currentColorOnVertex.B > 0 || currentColorOnVertex.B < 255) {

				return true;
			}

			// If 0 and trying to Add, or 255 and trying to Remove
			else if ((currentColorOnVertex.B <= 0 && paintColorSettings.applyVertexColorSettings.blueColor > 0) || (currentColorOnVertex.B >= 255 && paintColorSettings.applyVertexColorSettings.blueColor < 0)) {

				return true;
			}
		}

		// Alpha
		if (paintColorSettings.applyVertexColorSettings.alphaColor > 0 || paintColorSettings.applyVertexColorSettings.alphaColor < 0) {


			// If in between min and max and we're trying to Add/Remove
			if (currentColorOnVertex.A > 0 || currentColorOnVertex.A < 255) {

				return true;
			}

			// If 0 and trying to Add, or 255 and trying to Remove
			else if ((currentColorOnVertex.A <= 0 && paintColorSettings.applyVertexColorSettings.alphaColor > 0) || (currentColorOnVertex.A >= 255 && paintColorSettings.applyVertexColorSettings.alphaColor < 0)) {

				return true;
			}
		}

	}

	else if (paintColorSettings.applyVertexColorSettings.applyVertexColorSetting == EApplyVertexColorSetting::ESetVertexColor) {

		return true;
	}

	return false;
}


//-------------------------------------------------------

// Get Vertex Color After Any Limitation

void FVertexPaintDetectionAsyncTasks::GetVertexColorAfterAnyLimitation(const bool& limitIfColorWasAlreadyOverLimit, const float& originalRedVertexColor, float newRedVertexColor, const float& redVertexColorLimit, const float& originalGreenVertexColor, float newGreenVertexColor, const float& greenVertexColorLimit, const float& originalBlueVertexColor, float newBlueVertexColor, const float& blueVertexColorLimit, const float& originalAlphaVertexColor, float newAlphaVertexColor, const float& alphaVertexColorLimit, float& redColorOUT, float& greenColorOUT, float& blueColorOUT, float& alphaColorOUT) {


	// Red
	if (limitIfColorWasAlreadyOverLimit)
		newRedVertexColor = redVertexColorLimit;

	else if (originalRedVertexColor <= redVertexColorLimit && newRedVertexColor >= redVertexColorLimit)
		newRedVertexColor = redVertexColorLimit; // If we originally was below, but with the added colors we're now at or above the limit


	// Green
	if (limitIfColorWasAlreadyOverLimit)
		newGreenVertexColor = greenVertexColorLimit;
	else if (originalGreenVertexColor <= greenVertexColorLimit && newGreenVertexColor >= greenVertexColorLimit)
		newGreenVertexColor = greenVertexColorLimit;


	// Blue
	if (limitIfColorWasAlreadyOverLimit)
		newBlueVertexColor = blueVertexColorLimit;
	else if (originalBlueVertexColor <= blueVertexColorLimit && newBlueVertexColor >= blueVertexColorLimit)
		newBlueVertexColor = blueVertexColorLimit;


	// Alpha
	if (limitIfColorWasAlreadyOverLimit)
		newAlphaVertexColor = alphaVertexColorLimit;
	else if (originalAlphaVertexColor <= alphaVertexColorLimit && newAlphaVertexColor >= alphaVertexColorLimit)
		newAlphaVertexColor = alphaVertexColorLimit;


	redColorOUT = newRedVertexColor;
	greenColorOUT = newGreenVertexColor;
	blueColorOUT = newBlueVertexColor;
	alphaColorOUT = newAlphaVertexColor;
}


//-------------------------------------------------------

// Get Color To Apply On Vertex - Calculates strength depending on distance and falloff etc. 

FColor FVertexPaintDetectionAsyncTasks::GetColorToApplyOnVertex(const FColor & currentVertexColor, const FVector & currentVertexWorldPosition, const FVector & currentVertexNormal, const FName & currentBoneName, const float& fallOffStrength, const float& areaOfEffect, const float& areaOfEffectWithFullColor, const float& distanceToVertexFromPaintLocation, const FVertexPaintColorSettingStruct & paintOnMeshColorSetting, bool& vertexGotColorChanged, const bool& applyColorOnRedChannel, const bool& applyColorOnGreenChannel, const bool& applyColorOnBlueChannel, const bool& applyColorOnAlphaChannel) {

	vertexGotColorChanged = false;

	tryingToApplyColor_Red_Global = 0;
	tryingToApplyColor_Green_Global = 0;
	tryingToApplyColor_Blue_Global = 0;
	tryingToApplyColor_Alpha_Global = 0;

	shouldApplyColorOnAnyChannel_Global = false;
	paintCondition_ApplyColorStrengthIfConditionIsNotMet_Global = false;
	paintCondition_ColorStrengthIfConditionIsNotMet_Global = 0;


	// Takes in bools whether to even change the channel at all. This was necessary because if Painting with Set and using Physics Surface, then we Don't want to change any of the other channels that doesn't have the physics surface, unlike if we Paint with Set with just the values set in RGBA. 
	if (applyColorOnRedChannel) {

		if (IsCurrentVertexWithinPaintColorConditions(currentVertexColor, currentVertexWorldPosition, currentVertexNormal, currentBoneName, paintOnMeshColorSetting.applyVertexColorSettings.redColorCondition, paintCondition_ApplyColorStrengthIfConditionIsNotMet_Global, paintCondition_ColorStrengthIfConditionIsNotMet_Global)) {

			shouldApplyColorOnAnyChannel_Global = true;
			tryingToApplyColor_Red_Global = paintOnMeshColorSetting.applyVertexColorSettings.redColor;
		}
		else if (paintCondition_ApplyColorStrengthIfConditionIsNotMet_Global) { // If false and it's not 0, i.e. we're suppose to use another color strength if condition failed

			shouldApplyColorOnAnyChannel_Global = true;
			tryingToApplyColor_Red_Global = paintCondition_ColorStrengthIfConditionIsNotMet_Global;
		}
	}

	if (applyColorOnGreenChannel) {

		if (IsCurrentVertexWithinPaintColorConditions(currentVertexColor, currentVertexWorldPosition, currentVertexNormal, currentBoneName, paintOnMeshColorSetting.applyVertexColorSettings.greenColorCondition, paintCondition_ApplyColorStrengthIfConditionIsNotMet_Global, paintCondition_ColorStrengthIfConditionIsNotMet_Global)) {

			shouldApplyColorOnAnyChannel_Global = true;
			tryingToApplyColor_Green_Global = paintOnMeshColorSetting.applyVertexColorSettings.greenColor;
		}
		else if (paintCondition_ApplyColorStrengthIfConditionIsNotMet_Global) {

			shouldApplyColorOnAnyChannel_Global = true;
			tryingToApplyColor_Green_Global = paintCondition_ColorStrengthIfConditionIsNotMet_Global;
		}
	}

	if (applyColorOnBlueChannel) {

		if (IsCurrentVertexWithinPaintColorConditions(currentVertexColor, currentVertexWorldPosition, currentVertexNormal, currentBoneName, paintOnMeshColorSetting.applyVertexColorSettings.blueColorCondition, paintCondition_ApplyColorStrengthIfConditionIsNotMet_Global, paintCondition_ColorStrengthIfConditionIsNotMet_Global)) {

			shouldApplyColorOnAnyChannel_Global = true;
			tryingToApplyColor_Blue_Global = paintOnMeshColorSetting.applyVertexColorSettings.blueColor;
		}
		else if (paintCondition_ApplyColorStrengthIfConditionIsNotMet_Global) {

			shouldApplyColorOnAnyChannel_Global = true;
			tryingToApplyColor_Blue_Global = paintCondition_ColorStrengthIfConditionIsNotMet_Global;
		}
	}

	if (applyColorOnAlphaChannel) {

		if (IsCurrentVertexWithinPaintColorConditions(currentVertexColor, currentVertexWorldPosition, currentVertexNormal, currentBoneName, paintOnMeshColorSetting.applyVertexColorSettings.alphaColorCondition, paintCondition_ApplyColorStrengthIfConditionIsNotMet_Global, paintCondition_ColorStrengthIfConditionIsNotMet_Global)) {

			shouldApplyColorOnAnyChannel_Global = true;
			tryingToApplyColor_Alpha_Global = paintOnMeshColorSetting.applyVertexColorSettings.alphaColor;
		}
		else if (paintCondition_ApplyColorStrengthIfConditionIsNotMet_Global) {

			shouldApplyColorOnAnyChannel_Global = true;
			tryingToApplyColor_Alpha_Global = paintCondition_ColorStrengthIfConditionIsNotMet_Global;
		}
	}


	// If no color is going to be applied since none is within any condition, or none is going to be set to anything even if didn't reach the condition
	if (!shouldApplyColorOnAnyChannel_Global) {

		return currentVertexColor;
	}


	// Resets globalvars. Using globals as this runs for each vertex, which can be thousands of times
	getColorToApplyOnVertex_Distance_Global = 0;
	getColorToApplyOnVertex_FallOffClamped_Global = 0;
	getColorToApplyOnVertex_Falloff_Global = 0;
	getColorToApplyOnVertex_ColorToApply_Global = currentVertexColor;
	getColorToApplyOnVertex_StrengthAfterFalloff_Global = 1;

	// Current Color at the vertex point transformed to float
	getColorToApplyOnVertex_ColorRed_Global = (float)currentVertexColor.R;
	getColorToApplyOnVertex_ColorGreen_Global = (float)currentVertexColor.G;
	getColorToApplyOnVertex_ColorBlue_Global = (float)currentVertexColor.B;
	getColorToApplyOnVertex_ColorAlpha_Global = (float)currentVertexColor.A;



	// If outside off full color area and is set to have falloff
	if (distanceToVertexFromPaintLocation <= areaOfEffect && distanceToVertexFromPaintLocation > areaOfEffectWithFullColor && fallOffStrength > 0) {

		// Depending on the vertex pos, sets a distance temp between 0-1. Starts the falloff where the full area effect ends
		getColorToApplyOnVertex_Distance_Global = UKismetMathLibrary::MapRangeClamped((distanceToVertexFromPaintLocation - areaOfEffectWithFullColor), 0, areaOfEffect - areaOfEffectWithFullColor, 0, 1);

		// Clamps falloff to 0-1 in case user has set something else
		getColorToApplyOnVertex_FallOffClamped_Global = UKismetMathLibrary::FClamp(fallOffStrength, 0, 1);

		getColorToApplyOnVertex_Falloff_Global = UKismetMathLibrary::MapRangeClamped(getColorToApplyOnVertex_FallOffClamped_Global, 0, 1, 1, 0);

		// If falloff is 0, then it gives 1 even at the longest distance, if falloff is 1, then it scales to 0 at longest distance. 
		getColorToApplyOnVertex_StrengthAfterFalloff_Global = UKismetMathLibrary::MapRangeClamped(getColorToApplyOnVertex_Distance_Global, 0, 1, 1, getColorToApplyOnVertex_Falloff_Global);
	}


	if (paintOnMeshColorSetting.applyVertexColorSettings.applyVertexColorSetting == EApplyVertexColorSetting::EAddVertexColor) {

		// Strength in Color values * fall off
		if (applyColorOnRedChannel)
			getColorToApplyOnVertex_ColorRed_Global += UKismetMathLibrary::MapRangeClamped(tryingToApplyColor_Red_Global, -1, 1, -255, 255) * getColorToApplyOnVertex_StrengthAfterFalloff_Global;

		if (applyColorOnGreenChannel)
			getColorToApplyOnVertex_ColorGreen_Global += UKismetMathLibrary::MapRangeClamped(tryingToApplyColor_Green_Global, -1, 1, -255, 255) * getColorToApplyOnVertex_StrengthAfterFalloff_Global;

		if (applyColorOnBlueChannel)
			getColorToApplyOnVertex_ColorBlue_Global += UKismetMathLibrary::MapRangeClamped(tryingToApplyColor_Blue_Global, -1, 1, -255, 255) * getColorToApplyOnVertex_StrengthAfterFalloff_Global;

		if (applyColorOnAlphaChannel)
			getColorToApplyOnVertex_ColorAlpha_Global += UKismetMathLibrary::MapRangeClamped(tryingToApplyColor_Alpha_Global, -1, 1, -255, 255) * getColorToApplyOnVertex_StrengthAfterFalloff_Global;
	}

	else if (paintOnMeshColorSetting.applyVertexColorSettings.applyVertexColorSetting == EApplyVertexColorSetting::ESetVertexColor) {

		if (applyColorOnRedChannel)
			getColorToApplyOnVertex_ColorRed_Global = UKismetMathLibrary::MapRangeClamped(tryingToApplyColor_Red_Global, 0, 1, 0, 255);

		if (applyColorOnGreenChannel)
			getColorToApplyOnVertex_ColorGreen_Global = UKismetMathLibrary::MapRangeClamped(tryingToApplyColor_Green_Global, 0, 1, 0, 255);

		if (applyColorOnBlueChannel)
			getColorToApplyOnVertex_ColorBlue_Global = UKismetMathLibrary::MapRangeClamped(tryingToApplyColor_Blue_Global, 0, 1, 0, 255);

		if (applyColorOnAlphaChannel)
			getColorToApplyOnVertex_ColorAlpha_Global = UKismetMathLibrary::MapRangeClamped(tryingToApplyColor_Alpha_Global, 0, 1, 0, 255);
	}


	if (paintOnMeshColorSetting.limitPaintColor.usePaintLimits) {

		GetVertexColorAfterAnyLimitation(paintOnMeshColorSetting.limitPaintColor.limitColorIfTheColorWasAlreadyOverTheLimit, (float)currentVertexColor.R, getColorToApplyOnVertex_ColorRed_Global, UKismetMathLibrary::MapRangeClamped(paintOnMeshColorSetting.limitPaintColor.paintLimitOnRedChannel, 0, 1, 0, 255), (float)currentVertexColor.G, getColorToApplyOnVertex_ColorGreen_Global, UKismetMathLibrary::MapRangeClamped(paintOnMeshColorSetting.limitPaintColor.paintLimitOnGreenChannel, 0, 1, 0, 255), (float)currentVertexColor.B, getColorToApplyOnVertex_ColorBlue_Global, UKismetMathLibrary::MapRangeClamped(paintOnMeshColorSetting.limitPaintColor.paintLimitOnBlueChannel, 0, 1, 0, 255), (float)currentVertexColor.A, getColorToApplyOnVertex_ColorAlpha_Global, UKismetMathLibrary::MapRangeClamped(paintOnMeshColorSetting.limitPaintColor.paintLimitOnAlphaChannel, 0, 1, 0, 255), getColorToApplyOnVertex_ColorRed_Global, getColorToApplyOnVertex_ColorGreen_Global, getColorToApplyOnVertex_ColorBlue_Global, getColorToApplyOnVertex_ColorAlpha_Global);
	}


	if (applyColorOnRedChannel)
		getColorToApplyOnVertex_ColorToApply_Global.R = (uint8)UKismetMathLibrary::FClamp(getColorToApplyOnVertex_ColorRed_Global, 0, 255);

	if (applyColorOnGreenChannel)
		getColorToApplyOnVertex_ColorToApply_Global.G = (uint8)UKismetMathLibrary::FClamp(getColorToApplyOnVertex_ColorGreen_Global, 0, 255);

	if (applyColorOnBlueChannel)
		getColorToApplyOnVertex_ColorToApply_Global.B = (uint8)UKismetMathLibrary::FClamp(getColorToApplyOnVertex_ColorBlue_Global, 0, 255);

	if (applyColorOnAlphaChannel)
		getColorToApplyOnVertex_ColorToApply_Global.A = (uint8)UKismetMathLibrary::FClamp(getColorToApplyOnVertex_ColorAlpha_Global, 0, 255);


	// If the current color and the color applied isn't the same it means that the paint resulted in change, which can be used in the Paint Event Dispatcher to only do things if a change occured
	if (currentVertexColor != getColorToApplyOnVertex_ColorToApply_Global) {

		vertexGotColorChanged = true;
	}

	return getColorToApplyOnVertex_ColorToApply_Global;
}


//-------------------------------------------------------

// Is There Any Paint Conditions

bool FVertexPaintDetectionAsyncTasks::IsThereAnyPaintConditions(const FVertexDetectRGBAStruct & applyColorSettings) {


	if (applyColorSettings.applyColorsUsingPhysicsSurface.applyVertexColorUsingPhysicsSurface) {

		return (applyColorSettings.applyColorsUsingPhysicsSurface.paintUsingPhysicsSurfaceConditions.ifVertexColorIsWithinRange.Num() > 0 || applyColorSettings.applyColorsUsingPhysicsSurface.paintUsingPhysicsSurfaceConditions.ifVertexHasLineOfSightTo.Num() > 0 || applyColorSettings.applyColorsUsingPhysicsSurface.paintUsingPhysicsSurfaceConditions.ifVertexIsOnBone.Num() > 0 || applyColorSettings.applyColorsUsingPhysicsSurface.paintConditionsOnChannelsWithoutThePhysicsSurface.ifVertexColorIsWithinRange.Num() > 0 || applyColorSettings.applyColorsUsingPhysicsSurface.paintConditionsOnChannelsWithoutThePhysicsSurface.ifVertexHasLineOfSightTo.Num() > 0 || applyColorSettings.applyColorsUsingPhysicsSurface.paintConditionsOnChannelsWithoutThePhysicsSurface.ifVertexIsOnBone.Num() > 0);
	}
	else {

		return (applyColorSettings.redColorCondition.ifVertexColorIsWithinRange.Num() > 0 || applyColorSettings.redColorCondition.ifVertexHasLineOfSightTo.Num() > 0 || applyColorSettings.redColorCondition.ifVertexIsOnBone.Num() > 0 || applyColorSettings.greenColorCondition.ifVertexColorIsWithinRange.Num() > 0 || applyColorSettings.greenColorCondition.ifVertexHasLineOfSightTo.Num() > 0 || applyColorSettings.greenColorCondition.ifVertexIsOnBone.Num() > 0 || applyColorSettings.blueColorCondition.ifVertexColorIsWithinRange.Num() > 0 || applyColorSettings.blueColorCondition.ifVertexHasLineOfSightTo.Num() > 0 || applyColorSettings.blueColorCondition.ifVertexIsOnBone.Num() > 0 || applyColorSettings.alphaColorCondition.ifVertexColorIsWithinRange.Num() > 0 || applyColorSettings.alphaColorCondition.ifVertexHasLineOfSightTo.Num() > 0 || applyColorSettings.alphaColorCondition.ifVertexIsOnBone.Num() > 0);
	}

	return false;
}


//-------------------------------------------------------

// Compare Vertex Color

bool FVertexPaintDetectionAsyncTasks::CompareVertexColor(const int& vertexIndex, const TArray<FColor>&meshColors, const TArray<FColor>&compareWithColors) {

	if (vertexIndex < 0) return false;
	if (meshColors.Num() != compareWithColors.Num()) return false;


	float meshColorValueTemp = 0;
	float compareColorValueTemp = 0;

	for (int i = 0; i < 4; i++) {

		switch (i) {

		case 0:

			meshColorValueTemp = meshColors[vertexIndex].R;
			compareColorValueTemp = compareWithColors[vertexIndex].R;

			break;
		case 1:

			meshColorValueTemp = meshColors[vertexIndex].G;
			compareColorValueTemp = compareWithColors[vertexIndex].G;

			break;
		case 2:

			meshColorValueTemp = meshColors[vertexIndex].B;
			compareColorValueTemp = compareWithColors[vertexIndex].B;

			break;
		case 3:

			meshColorValueTemp = meshColors[vertexIndex].A;
			compareColorValueTemp = compareWithColors[vertexIndex].A;

			break;
		default:
			break;
		}


		// If color array to compare with has any color, then mesh color has to be within certain range to that
		if (compareColorValueTemp > 0) {

			// Mesh color also has to be above 0 to check if within range
			if (meshColorValueTemp > 0) {

				if (UKismetMathLibrary::NearlyEqual_FloatFloat(compareColorValueTemp, meshColorValueTemp, calculateColorsInfo_Global.fundementalStruct.callbackSettings.compareMeshColorsToColorArray.rangeMeshColorHasToBeWithin)) {

					// 
				}
				else {

					return false;
				}
			}
			else {

				return false;
			}
		}

		// Otherwise if color array we compare with is 0
		else {

			// then mesh color has to be 0 as well
			if (meshColorValueTemp == 0) {

				//
			}
			else {

				return false;
			}
		}
	}

	// If haven't returned false by now all of the channels must've been matched
	return true;
}


//-------------------------------------------------------

// Get Paint Condition Adjusted If Set To Use Physics Surface

FVertexDetectPaintColorConditionStruct FVertexPaintDetectionAsyncTasks::GetPaintConditionAdjustedIfSetToUsePhysicsSurface(FVertexDetectPaintColorConditionStruct channelsPaintCondition, UMaterialInterface * material) {

	if (!IsValid(material)) return channelsPaintCondition;


	if (channelsPaintCondition.ifVertexColorIsWithinRange.Num() > 0) {

		for (int i = 0; i < channelsPaintCondition.ifVertexColorIsWithinRange.Num(); i++) {

			if (channelsPaintCondition.ifVertexColorIsWithinRange[i].ifChannelWithPhysicsSurfaceIsWithinColorRange == EPhysicalSurface::SurfaceType_Default) continue;


			// If set to use the Within Color Range Condition using Physics Surface instead of Vertex Color Channel 
			TArray<Enum_SurfaceAtChannel> physicsSurfaceAtVertexColorChannelsTemp;


			// Gets the Parents of the physicSurfaceWithinColorRange we want to use, so we can check if any of the registered physics surface is a part of the same family, and which channel. If so we can have the condition take effect on the channel that physics surface is registered to. 
			TArray<TEnumAsByte<EPhysicalSurface>> parentsOfifChannelWithPhysicsSurfaceIsWithinColorRange = UVertexPaintFunctionLibrary::GetVertexPaintMaterialDataAsset()->GetPhysicsSurfaceFamilyParentsOfPhysicsSurface(channelsPaintCondition.ifVertexColorIsWithinRange[i].ifChannelWithPhysicsSurfaceIsWithinColorRange);


			// Checks if ifChannelWithPhysicsSurfaceIsWithinColorRange and what physics surface that's registered on each vertex color channel is a part of the same family
			for (auto parensOfRedChannelTemp : UVertexPaintFunctionLibrary::GetVertexPaintMaterialDataAsset()->GetPhysicsSurfaceFamilyParentsOfPhysicsSurface(UVertexPaintFunctionLibrary::GetVertexPaintMaterialDataAsset()->GetVertexPaintMaterialInterface().FindRef(material).paintedAtRed)) {

				if (parentsOfifChannelWithPhysicsSurfaceIsWithinColorRange.Contains(parensOfRedChannelTemp)) {

					physicsSurfaceAtVertexColorChannelsTemp.Add(Enum_SurfaceAtChannel::RedChannel);
					break;
				}
			}

			for (auto parensOfGreenChannelTemp : UVertexPaintFunctionLibrary::GetVertexPaintMaterialDataAsset()->GetPhysicsSurfaceFamilyParentsOfPhysicsSurface(UVertexPaintFunctionLibrary::GetVertexPaintMaterialDataAsset()->GetVertexPaintMaterialInterface().FindRef(material).paintedAtGreen)) {

				if (parentsOfifChannelWithPhysicsSurfaceIsWithinColorRange.Contains(parensOfGreenChannelTemp)) {

					physicsSurfaceAtVertexColorChannelsTemp.Add(Enum_SurfaceAtChannel::GreenChannel);
					break;
				}
			}

			for (auto parensOfBlueChannelTemp : UVertexPaintFunctionLibrary::GetVertexPaintMaterialDataAsset()->GetPhysicsSurfaceFamilyParentsOfPhysicsSurface(UVertexPaintFunctionLibrary::GetVertexPaintMaterialDataAsset()->GetVertexPaintMaterialInterface().FindRef(material).paintedAtBlue)) {

				if (parentsOfifChannelWithPhysicsSurfaceIsWithinColorRange.Contains(parensOfBlueChannelTemp)) {

					physicsSurfaceAtVertexColorChannelsTemp.Add(Enum_SurfaceAtChannel::BlueChannel);
					break;
				}
			}

			for (auto parensOfAlphaChannelTemp : UVertexPaintFunctionLibrary::GetVertexPaintMaterialDataAsset()->GetPhysicsSurfaceFamilyParentsOfPhysicsSurface(UVertexPaintFunctionLibrary::GetVertexPaintMaterialDataAsset()->GetVertexPaintMaterialInterface().FindRef(material).paintedAtAlpha)) {

				if (parentsOfifChannelWithPhysicsSurfaceIsWithinColorRange.Contains(parensOfAlphaChannelTemp)) {

					physicsSurfaceAtVertexColorChannelsTemp.Add(Enum_SurfaceAtChannel::AlphaChannel);
					break;
				}
			}


			if (physicsSurfaceAtVertexColorChannelsTemp.Num() > 0) {


				// In some instance a physics surface may share the same parent on multiple vertex color channels, for example if Cobble-Sand is on Red Channel, and Cobble-Puddle is on Blue Channel. Then we will just use the channel based on which within range index we're at. So if for instance Green and Blue have a Physics Surface, and we have 2 Is Within Range Conditions, then the first will get set to use the Green, and the second the Blue. 
				if (physicsSurfaceAtVertexColorChannelsTemp.IsValidIndex(i)) {

					channelsPaintCondition.ifVertexColorIsWithinRange[i].ifVertexColorChannelWithinColorRange = physicsSurfaceAtVertexColorChannelsTemp[i];
				}

				// If it's not valid index, i.e. maybe just 1 vertex color channel is using the physics surface, but we have several within range conditions, then they will all use that one vertex color channel. 
				else if (physicsSurfaceAtVertexColorChannelsTemp.IsValidIndex(0)) {

					channelsPaintCondition.ifVertexColorIsWithinRange[i].ifVertexColorChannelWithinColorRange = physicsSurfaceAtVertexColorChannelsTemp[0];
				}
			}
		}
	}

	return channelsPaintCondition;
}



/** Helper struct for the mesh component vert position octree - Copied from MeshPaintHelpers.cpp ApplyVertexColorsToAllLODs */
struct FStaticMeshComponentVertPosOctreeSemantics {
	enum { MaxElementsPerLeaf = 16 };
	enum { MinInclusiveElementsPerNode = 7 };
	enum { MaxNodeDepth = 12 };

	typedef TInlineAllocator<MaxElementsPerLeaf> ElementAllocator;

	/**
	 * Get the bounding box of the provided octree element. In this case, the box
	 * is merely the point specified by the element.
	 *
	 * @param	Element	Octree element to get the bounding box for
	 *
	 * @return	Bounding box of the provided octree element
	 */
	FORCEINLINE static FBoxCenterAndExtent GetBoundingBox(const FPaintedVertex& Element)
	{
		return FBoxCenterAndExtent(Element.Position, FVector::ZeroVector);
	}

	/**
	 * Determine if two octree elements are equal
	 *
	 * @param	A	First octree element to check
	 * @param	B	Second octree element to check
	 *
	 * @return	true if both octree elements are equal, false if they are not
	 */
	FORCEINLINE static bool AreElementsEqual(const FPaintedVertex& A, const FPaintedVertex& B)
	{
		return (A.Position == B.Position && A.Normal == B.Normal && A.Color == B.Color);
	}

	/** Ignored for this implementation */
	FORCEINLINE static void SetElementId(const FPaintedVertex& Element, FOctreeElementId2 Id)
	{
	}
};

typedef TOctree2<FPaintedVertex, FStaticMeshComponentVertPosOctreeSemantics> TSMCVertPosOctree;


//-------------------------------------------------------

// Propogate LOD 0 Vertex Colors to the Rest of the Mesh

void FVertexPaintDetectionAsyncTasks::PropogateLOD0VertexColorsToLODs(const TArray<FPaintedVertex>&lod0PaintedVerts, FVertexDetectMeshDataStruct & vertexDetectMeshData, const TArray<FBox>&lodBaseBounds, TArray<FColorVertexBuffer*>&colorVertexBufferToUpdate, UPrimitiveComponent * component) {

	if (vertexDetectMeshData.meshDataPerLOD.Num() <= 1) return; // Has to have more than 1 LOD to build color data for the other LODs
	if (vertexDetectMeshData.meshDataPerLOD[0].meshVertexColorsPerLODArray.Num() <= 0)  return; // And the LOD0 has to have some colors that we can propogate
	if (!IsValid(component)) return;


	FVector CurPosition = FVector(ForceInitToZero);
	FVector CurNormal = FVector(ForceInitToZero);
	TArray<FPaintedVertex> PointsToConsider;
	const float DistanceOverNormalThreshold = KINDA_SMALL_NUMBER;

	FPaintedVertex BestVertex = FPaintedVertex();
	FVector BestVertexNormal = FVector(ForceInitToZero);
	float BestDistanceSquared = 0;
	float BestNormalDot = 0;
	FPaintedVertex VertexToConsider = FPaintedVertex();
	FVector VertexNormalToConsider = FVector(ForceInitToZero);
	float DistSqrdToConsider = 0;
	float NormalDotToConsider = 0;


	for (uint32 i = 1; i < (uint32)vertexDetectMeshData.meshDataPerLOD.Num(); ++i) {

		if (!colorVertexBufferToUpdate.IsValidIndex(i) || !vertexDetectMeshData.meshDataPerLOD.IsValidIndex(i)) break;


		// UMeshPaintingSubsystem::ApplyVertexColorsToAllLODs -> RemapPaintedVertexColors in StaticMeshBuild

		// This was copied from StaticMeshBuild->RemapPaintedVertexColors() and adjusted a bit so it works for both Static and Skeletal Meshes in Runtime and gets what each vertex color on LOD1, 2 etc. should have based on LOD0. It's a bit more expensive and I didn't see a noticable better result using it so i opted to use my own solution which allows me to break out from the main loop once all the vertices has been randomized, since it doesn't build Bounds and doesn't need All LOD0 positions at once. Also my own solution only loops through the as many LOD0 vertices that was actually randomized, instead of looping through all of them for every LOD1 vertex etc.


		FBox Bounds = lodBaseBounds[0]; // First sets BaseBounds for LOD0
		Bounds += lodBaseBounds[i]; // Then adds the current LOD for a Combined Bounds


		TSMCVertPosOctree VertPosOctree(Bounds.GetCenter(), Bounds.GetExtent().GetMax());

		for (int32 PaintedVertexIndex = 0; PaintedVertexIndex < lod0PaintedVerts.Num(); ++PaintedVertexIndex) {

			VertPosOctree.AddElement(lod0PaintedVerts[PaintedVertexIndex]);
		}


		CurPosition = FVector(ForceInitToZero);
		CurNormal = FVector(ForceInitToZero);
		PointsToConsider.Reset();

		BestVertex = FPaintedVertex();
		BestVertexNormal = FVector(ForceInitToZero);
		BestDistanceSquared = 0;
		BestNormalDot = 0;
		VertexToConsider = FPaintedVertex();
		VertexNormalToConsider = FVector(ForceInitToZero);
		DistSqrdToConsider = 0;
		NormalDotToConsider = 0;


		for (uint32 NewVertIndex = 0; NewVertIndex < (uint32)vertexDetectMeshData.meshDataPerLOD[i].meshVertexPositionsInComponentSpacePerLODArray.Num(); ++NewVertIndex) {

			// Necessary in case it gets destroyed will running, otherwise the task will not be removed immediately
			if (!IsValid(component)) return;

			PointsToConsider.Reset();
			CurPosition = vertexDetectMeshData.meshDataPerLOD[i].meshVertexPositionsInComponentSpacePerLODArray[NewVertIndex];
			CurNormal = vertexDetectMeshData.meshDataPerLOD[i].meshVertexNormalsPerLODArray[NewVertIndex];


			// Iterate through the octree attempting to find the vertices closest to the current new point
			VertPosOctree.FindNearbyElements(CurPosition, [&PointsToConsider](const FPaintedVertex& Vertex) {

				PointsToConsider.Add(Vertex);
				});

			// If any points to consider were found, iterate over each and find which one is the closest to the new point 
			if (PointsToConsider.Num() > 0) {

				BestVertex = PointsToConsider[0];
				BestVertexNormal = BestVertex.Normal;

				BestDistanceSquared = (BestVertex.Position - CurPosition).SizeSquared();
				BestNormalDot = BestVertexNormal | CurNormal;

				for (int32 ConsiderationIndex = 1; ConsiderationIndex < PointsToConsider.Num(); ++ConsiderationIndex) {

					VertexToConsider = PointsToConsider[ConsiderationIndex];
					VertexNormalToConsider = VertexToConsider.Normal;

					DistSqrdToConsider = (VertexToConsider.Position - CurPosition).SizeSquared();
					NormalDotToConsider = VertexNormalToConsider | CurNormal;

					if (DistSqrdToConsider < BestDistanceSquared - DistanceOverNormalThreshold) {

						BestVertex = VertexToConsider;
						BestDistanceSquared = DistSqrdToConsider;
						BestNormalDot = NormalDotToConsider;
					}

					else if (DistSqrdToConsider < BestDistanceSquared + DistanceOverNormalThreshold && NormalDotToConsider > BestNormalDot) {

						BestVertex = VertexToConsider;
						BestDistanceSquared = DistSqrdToConsider;
						BestNormalDot = NormalDotToConsider;
					}
				}

				vertexDetectMeshData.meshDataPerLOD[i].meshVertexColorsPerLODArray[NewVertIndex] = BestVertex.Color;
				colorVertexBufferToUpdate[i]->VertexColor(NewVertIndex) = BestVertex.Color;
			}
		}
	}
}
