// Copyright 2022 Alexander Floden, Alias Alex River. All Rights Reserved. 

#include "VertexPaintDetectionComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/AssetManager.h"
#include "PhysicsEngine/BodySetup.h"
#include "PhysicsEngine/PhysicsAsset.h"

#include "VertexPaintDetectionGISubSystem.h"
#include "VertexPaintFunctionLibrary.h"
#include "VertexPaintDetectionSettings.h"
#include "VertexPaintColorSnippetDataAsset.h"

#include "Engine/StaticMesh.h"
#include "StaticMeshResources.h"
#include "Engine/SkeletalMesh.h"
#include "Rendering/SkeletalMeshRenderData.h"

#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Landscape.h"


//-------------------------------------------------------

// Construct

UVertexPaintDetectionComponent::UVertexPaintDetectionComponent() {

	PrimaryComponentTick.bCanEverTick = false;

	SetComponentTickEnabled(false);
}


//--------------------------------------------------------

// Begin Play

void UVertexPaintDetectionComponent::BeginPlay() {

	Super::BeginPlay();

	if (UGameplayStatics::GetGameInstance(GetWorld())) {

		vertexPaintGameInstanceSubsystem_Global = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UVertexPaintDetectionGISubSystem>();
	}

	if (GetDefault<UVertexPaintDetectionSettings>())
		maxAllowedTasksPerMesh = GetDefault<UVertexPaintDetectionSettings>()->vertexPaintAndDetectionPlugin_MaxAmountOfAllowedTasksPerMesh;
}


//-------------------------------------------------------

// Get Closest Vertex Data On Mesh

void UVertexPaintDetectionComponent::GetClosestVertexDataOnMesh(FVertexDetectInfoStruct getClosestVertexDataStruct, FVertexDetectAdditionalDataToPassThrough additionalDataToPassThrough) {

	amountOfGetClosestVertexDataTasksCache++;

	if (IsValid(getClosestVertexDataStruct.meshComponent)) {

		if (IsValid(getClosestVertexDataStruct.meshComponent->GetOwner())) {

			getClosestVertexDataStruct.actor = getClosestVertexDataStruct.meshComponent->GetOwner();

			if (printLogsToScreen || printLogsToOutputLog)
				UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Runtime Vertex Color Paint and Detection Plugin - Trying to Get Closest Vertex Data on Actor: %s and Component: %s"), *getClosestVertexDataStruct.meshComponent->GetOwner()->GetName(), *getClosestVertexDataStruct.meshComponent->GetName()), printLogsToScreen, printLogsToOutputLog, FColor::Cyan, printLogsToScreen_Duration);
		}

		getClosestVertexDataStruct.hitFundementals.hitLocationInComponentSpace = UKismetMathLibrary::InverseTransformLocation(getClosestVertexDataStruct.meshComponent->GetComponentTransform(), getClosestVertexDataStruct.hitFundementals.hitLocation);
	}

	// Clamps these between 0-1 in case the user set a different value
	getClosestVertexDataStruct.callbackSettings.includeAmountOfPaintedColorsOfEachChannel = UVertexPaintFunctionLibrary::ClampAmountOfPaintedColorsOfEachChannel(getClosestVertexDataStruct.callbackSettings.includeAmountOfPaintedColorsOfEachChannel);


	FCalculateColorsInfo calculateColorsInfoTemp;
	calculateColorsInfoTemp.paintDetectionType = EVertexPaintDetectionType::Detection;
	calculateColorsInfoTemp.detectInfoSettings = getClosestVertexDataStruct;
	calculateColorsInfoTemp.initiatedByComponent = this;
	calculateColorsInfoTemp.additionalDataToPassThrough = additionalDataToPassThrough;
	calculateColorsInfoTemp.estimatedColorAtHitLocationSettings = getClosestVertexDataStruct.getEstimatedColorAtHitLocationSettings;

	FillCalculateColorsInfoDebugSettings(calculateColorsInfoTemp);
	UVertexPaintFunctionLibrary::FillCalculateColorsInfoFundementals(getClosestVertexDataStruct, calculateColorsInfoTemp);
	

	bool passedChecks_Local = true;

	if (!UVertexPaintFunctionLibrary::CheckValidFundementals(getClosestVertexDataStruct.meshComponent, printLogsToScreen, printLogsToOutputLog, printLogsToScreen_Duration))
		passedChecks_Local = false;

	if (passedChecks_Local && UVertexPaintFunctionLibrary::CheckHaveReachedMaxDetectionTask(getClosestVertexDataStruct, printLogsToScreen, printLogsToOutputLog, printLogsToScreen_Duration))
		passedChecks_Local = false;


	// If passed fundamental checks
	if (passedChecks_Local) {

		// 
	}

	else {

		if (printLogsToScreen || printLogsToOutputLog)
			UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Runtime Vertex Color Paint and Detection Plugin - Check Valid Failed at Get Closest Vertex Data On Mesh!")), printLogsToScreen, printLogsToOutputLog, FColor::Red, printLogsToScreen_Duration);

		UVertexPaintFunctionLibrary::RunGetClosestVertexDataCallbacks(calculateColorsInfoTemp, FVertexDetectClosestVertexDataResultStruct(), FVertexDetectEstimatedColorAtHitLocationResultStruct(), FVertexDetectAvarageColorInAreaInfo());

		return;
	}


	if (printLogsToScreen || printLogsToOutputLog)
		UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Runtime Vertex Color Paint and Detection Plugin - Get Closest Vertex Data On Mesh CheckValid Successfull. ")), printLogsToScreen, printLogsToOutputLog, FColor::Green, printLogsToScreen_Duration);


	vertexPaintGameInstanceSubsystem_Global->AddCalculateColorsTaskToQueue(calculateColorsInfoTemp);
}

void UVertexPaintDetectionComponent::GetClosestVertexDataOnMeshTaskFinished() {

	if (amountOfGetClosestVertexDataTasksCache > 0)
		amountOfGetClosestVertexDataTasksCache--;
}


//-------------------------------------------------------

// Get Vertex Colors Only

void UVertexPaintDetectionComponent::GetAllVertexColorsOnly(FVertexDetectGetColorsOnlyStruct getAllVertexColorsStruct, FVertexDetectAdditionalDataToPassThrough additionalDataToPassThrough) {


	amountOfGetAllVertexColorsOnlyTasksCache++;

	if (IsValid(getAllVertexColorsStruct.meshComponent)) {

		if (IsValid(getAllVertexColorsStruct.meshComponent->GetOwner())) {

			getAllVertexColorsStruct.actor = getAllVertexColorsStruct.meshComponent->GetOwner();

			if (printLogsToScreen || printLogsToOutputLog)
				UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Runtime Vertex Color Paint and Detection Plugin - Trying to Get Colors Only on Actor: %s and Component: %s"), *getAllVertexColorsStruct.meshComponent->GetOwner()->GetName(), *getAllVertexColorsStruct.meshComponent->GetName()), printLogsToScreen, printLogsToOutputLog, FColor::Cyan, printLogsToScreen_Duration);
		}
	}

	// Clamps these between 0-1 in case the user set a different value
	getAllVertexColorsStruct.callbackSettings.includeAmountOfPaintedColorsOfEachChannel = UVertexPaintFunctionLibrary::ClampAmountOfPaintedColorsOfEachChannel(getAllVertexColorsStruct.callbackSettings.includeAmountOfPaintedColorsOfEachChannel);


	// Re-uses the detectStruct but just sets the DetectVertexColor Setting. Was safest since FVertexDetectInfoStruct is used everywhere and checks if it's .actor is valid. If we where to make a new struct just for Get All Vertex Colors we have to add checks just for that as well, which comes at a risk of creating bugs 
	FVertexDetectInfoStruct detectStruct;
	detectStruct.meshComponent = getAllVertexColorsStruct.meshComponent;
	detectStruct.callbackSettings = getAllVertexColorsStruct.callbackSettings;
	detectStruct.multiThreadSettings = getAllVertexColorsStruct.multiThreadSettings;
	detectStruct.actor = getAllVertexColorsStruct.actor;


	FCalculateColorsInfo calculateColorsInfoTemp;
	calculateColorsInfoTemp.paintDetectionType = EVertexPaintDetectionType::Detection;
	calculateColorsInfoTemp.detectInfoSettings = detectStruct;
	calculateColorsInfoTemp.getAllVertexColorsSettings = getAllVertexColorsStruct;
	calculateColorsInfoTemp.additionalDataToPassThrough = additionalDataToPassThrough;
	calculateColorsInfoTemp.initiatedByComponent = this;

	FillCalculateColorsInfoDebugSettings(calculateColorsInfoTemp);
	UVertexPaintFunctionLibrary::FillCalculateColorsInfoFundementals(detectStruct, calculateColorsInfoTemp);


	bool passedChecks_Local = true;

	if (!UVertexPaintFunctionLibrary::CheckValidFundementals(getAllVertexColorsStruct.meshComponent, printLogsToScreen, printLogsToOutputLog, printLogsToScreen_Duration))
		passedChecks_Local = false;

	if (passedChecks_Local && UVertexPaintFunctionLibrary::CheckHaveReachedMaxDetectionTask(getAllVertexColorsStruct, printLogsToScreen, printLogsToOutputLog, printLogsToScreen_Duration))
		passedChecks_Local = false;


	if (!passedChecks_Local) {

		if (printLogsToScreen || printLogsToOutputLog)
			UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Runtime Vertex Color Paint and Detection Plugin - Check Valid Failed at Get All Vertex Colors Only!")), printLogsToScreen, printLogsToOutputLog, FColor::Red, printLogsToScreen_Duration);

		UVertexPaintFunctionLibrary::RunGetAllColorsOnlyCallbacks(calculateColorsInfoTemp);
		return;
	}


	if (printLogsToScreen || printLogsToOutputLog)
		UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Runtime Vertex Color Paint and Detection Plugin - Get All Vertex Colors Only CheckValid Successfull. ")), printLogsToScreen, printLogsToOutputLog, FColor::Green, printLogsToScreen_Duration);


	vertexPaintGameInstanceSubsystem_Global->AddCalculateColorsTaskToQueue(calculateColorsInfoTemp);
}

void UVertexPaintDetectionComponent::GetAllVertexColorsOnlyTaskFinished() {

	if (amountOfGetAllVertexColorsOnlyTasksCache > 0)
		amountOfGetAllVertexColorsOnlyTasksCache--;
}


//-------------------------------------------------------

// Paint On Mesh At Location

void UVertexPaintDetectionComponent::PaintOnMeshAtLocation(FVertexPaintAtLocationStruct paintAtLocationStruct, FVertexDetectAdditionalDataToPassThrough additionalDataToPassThrough) {

	amountOfPaintAtLocationTasksCache++;

	if (IsValid(paintAtLocationStruct.meshComponent)) {

		if (IsValid(paintAtLocationStruct.meshComponent->GetOwner())) {

			paintAtLocationStruct.actor = paintAtLocationStruct.meshComponent->GetOwner();

			if (printLogsToScreen || printLogsToOutputLog)
				UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Runtime Vertex Color Paint and Detection Plugin - Trying to Paint on Mesh at Location on Actor: %s and Component: %s"), *paintAtLocationStruct.meshComponent->GetOwner()->GetName(), *paintAtLocationStruct.meshComponent->GetName()), printLogsToScreen, printLogsToOutputLog, FColor::Cyan, printLogsToScreen_Duration);
		}

		paintAtLocationStruct.hitFundementals.hitLocationInComponentSpace = UKismetMathLibrary::InverseTransformLocation(paintAtLocationStruct.meshComponent->GetComponentTransform(), paintAtLocationStruct.hitFundementals.hitLocation);
	}


	// Clamps these between 0-1 in case the user set a different value
	paintAtLocationStruct.callbackSettings.includeAmountOfPaintedColorsOfEachChannel = UVertexPaintFunctionLibrary::ClampAmountOfPaintedColorsOfEachChannel(paintAtLocationStruct.callbackSettings.includeAmountOfPaintedColorsOfEachChannel);
	paintAtLocationStruct.limitPaintColor = UVertexPaintFunctionLibrary::ClampPaintLimits(paintAtLocationStruct.limitPaintColor);


	FVertexDetectInfoStruct detectSettingsTemp;

	// Sets these if gonna detect before painting so can use them in case we need to do any checks in check valid etc. 
	if (paintAtLocationStruct.detection.runGetClosestVertexDataOnMeshBeforeApplyingPaint || paintAtLocationStruct.detection.runGetClosestVertexDataOnMeshAfterApplyingPaint) {

		detectSettingsTemp.actor = paintAtLocationStruct.actor;
		detectSettingsTemp.meshComponent = paintAtLocationStruct.meshComponent;
		detectSettingsTemp.multiThreadSettings = paintAtLocationStruct.multiThreadSettings;
		detectSettingsTemp.callbackSettings = paintAtLocationStruct.callbackSettings;
		detectSettingsTemp.getEstimatedColorAtHitLocationSettings = paintAtLocationStruct.getEstimatedColorAtHitLocationSettings;
		detectSettingsTemp.getAverageColorInAreaSettings = paintAtLocationStruct.detection.getAverageColorInAreaSettings;
		detectSettingsTemp.getEstimatedColorAtHitLocationSettings = paintAtLocationStruct.getEstimatedColorAtHitLocationSettings;

		if (paintAtLocationStruct.detection.useCustomHitFundementalsForDetection) {

			detectSettingsTemp.hitFundementals.hitLocation = paintAtLocationStruct.detection.detectHitFundementals.hitLocation;
			detectSettingsTemp.hitFundementals.hitLocationInComponentSpace = UKismetMathLibrary::InverseTransformLocation(paintAtLocationStruct.meshComponent->GetComponentTransform(), paintAtLocationStruct.detection.detectHitFundementals.hitLocation);
			detectSettingsTemp.hitFundementals.hitNormal = paintAtLocationStruct.detection.detectHitFundementals.hitNormal;
			detectSettingsTemp.hitFundementals.runTaskFor = paintAtLocationStruct.detection.detectHitFundementals.runTaskFor;
			detectSettingsTemp.hitFundementals.hitBone = paintAtLocationStruct.detection.detectHitFundementals.hitBone;
		}

		else {

			detectSettingsTemp.hitFundementals.hitLocation = paintAtLocationStruct.hitFundementals.hitLocation;
			detectSettingsTemp.hitFundementals.hitLocationInComponentSpace = UKismetMathLibrary::InverseTransformLocation(paintAtLocationStruct.meshComponent->GetComponentTransform(), paintAtLocationStruct.hitFundementals.hitLocation);
			detectSettingsTemp.hitFundementals.hitNormal = paintAtLocationStruct.hitFundementals.hitNormal;
			detectSettingsTemp.hitFundementals.runTaskFor = paintAtLocationStruct.hitFundementals.runTaskFor;
			detectSettingsTemp.hitFundementals.hitBone = paintAtLocationStruct.hitFundementals.hitBone;
		}
	}


	FCalculateColorsInfo calculateColorsInfoTemp;
	calculateColorsInfoTemp.paintDetectionType = EVertexPaintDetectionType::PaintAtLocation;
	calculateColorsInfoTemp.paintOnMeshAtLocationSettings = paintAtLocationStruct;
	calculateColorsInfoTemp.estimatedColorAtHitLocationSettings = paintAtLocationStruct.getEstimatedColorAtHitLocationSettings;
	calculateColorsInfoTemp.detectInfoSettings = detectSettingsTemp;
	calculateColorsInfoTemp.initiatedByComponent = this;
	calculateColorsInfoTemp.additionalDataToPassThrough = additionalDataToPassThrough;
	calculateColorsInfoTemp.paintOnMeshColorSettings = paintAtLocationStruct;
	calculateColorsInfoTemp.vertexPaintSettings = paintAtLocationStruct;

	FillCalculateColorsInfoDebugSettings(calculateColorsInfoTemp);
	UVertexPaintFunctionLibrary::FillCalculateColorsInfoFundementals(paintAtLocationStruct, calculateColorsInfoTemp);


	bool passedChecks_Local = true;

	if (!UVertexPaintFunctionLibrary::CheckValidFundementals(paintAtLocationStruct.meshComponent, printLogsToScreen, printLogsToOutputLog, printLogsToScreen_Duration))
		passedChecks_Local = false;

	if (passedChecks_Local && !UVertexPaintFunctionLibrary::CheckValidVertexPaintSettings(paintAtLocationStruct, printLogsToScreen, printLogsToOutputLog, printLogsToScreen_Duration))
		passedChecks_Local = false;

	if (passedChecks_Local && !UVertexPaintFunctionLibrary::CheckValidVertexPaintColorSettings(paintAtLocationStruct, printLogsToScreen, printLogsToOutputLog, printLogsToScreen_Duration))
		passedChecks_Local = false;


	// If passed fundamental checks, then checks paint at location specific ones
	if (passedChecks_Local) {

		if (paintAtLocationStruct.paintAtAreaSettings.areaOfEffect <= 0) {

			if (printLogsToScreen || printLogsToOutputLog)
				UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Runtime Vertex Color Paint and Detection Plugin - Check Valid, Paint at Location Fail: Paint Area of Effect is <= 0")), printLogsToScreen, printLogsToOutputLog, FColor::Red, printLogsToScreen_Duration);

			passedChecks_Local = false;
		}
	}


	if (!passedChecks_Local) {

		if (printLogsToScreen || printLogsToOutputLog)
			UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Runtime Vertex Color Paint and Detection Plugin - Check Valid Failed at Paint on Mesh at Location!")), printLogsToScreen, printLogsToOutputLog, FColor::Red, printLogsToScreen_Duration);

		// If fail and set to run detections then we expect fail callbacks on the get closest vertex data delegates as well
		if (paintAtLocationStruct.detection.runGetClosestVertexDataOnMeshBeforeApplyingPaint)
			UVertexPaintFunctionLibrary::RunGetClosestVertexDataCallbacks(calculateColorsInfoTemp, FVertexDetectClosestVertexDataResultStruct(), FVertexDetectEstimatedColorAtHitLocationResultStruct(), FVertexDetectAvarageColorInAreaInfo());


		UVertexPaintFunctionLibrary::RunPaintAtLocationCallbacks(calculateColorsInfoTemp, FVertexDetectClosestVertexDataResultStruct(), FVertexDetectEstimatedColorAtHitLocationResultStruct(), FVertexDetectAvarageColorInAreaInfo());


		if (paintAtLocationStruct.detection.runGetClosestVertexDataOnMeshAfterApplyingPaint)
			UVertexPaintFunctionLibrary::RunGetClosestVertexDataCallbacks(calculateColorsInfoTemp, FVertexDetectClosestVertexDataResultStruct(), FVertexDetectEstimatedColorAtHitLocationResultStruct(), FVertexDetectAvarageColorInAreaInfo());

		return;
	}

	if (paintAtLocationStruct.applyVertexColorSettings.applyColorsUsingPhysicsSurface.applyVertexColorUsingPhysicsSurface) {

		if (printLogsToScreen || printLogsToOutputLog)
			UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Runtime Vertex Color Paint and Detection Plugin - Paint on Mesh at Location CheckValid Successfull. Trying to Apply Color with EPhysicalSurface: %i  with Paint Strength: %f  -  Fall Back to RGBA Colors if Failed to Apply Colors Using Physics Surface: %i  -  Paint Strength on Surfaces Without the Physical Surface: %f"), paintAtLocationStruct.applyVertexColorSettings.applyColorsUsingPhysicsSurface.physicsSurfaceToApply, paintAtLocationStruct.applyVertexColorSettings.applyColorsUsingPhysicsSurface.paintStrengthToApply, paintAtLocationStruct.applyVertexColorSettings.applyColorsUsingPhysicsSurface.fallbackToRGBAColorsIfFailedToApplyColorsUsingPhysicsSurface, paintAtLocationStruct.applyVertexColorSettings.applyColorsUsingPhysicsSurface.paintStrengtOnChannelsWithoutThePhysicsSurface), printLogsToScreen, printLogsToOutputLog, FColor::Green, printLogsToScreen_Duration);
	}

	else {

		if (printLogsToScreen || printLogsToOutputLog)
			UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Runtime Vertex Color Paint and Detection Plugin - Paint on Mesh at Location CheckValid Successfull. Trying to Apply Color: %s  Area of Effect: %f  Falloff Strength: %f"), *FLinearColor(paintAtLocationStruct.applyVertexColorSettings.redColor, paintAtLocationStruct.applyVertexColorSettings.greenColor, paintAtLocationStruct.applyVertexColorSettings.blueColor, paintAtLocationStruct.applyVertexColorSettings.alphaColor).ToString(), paintAtLocationStruct.paintAtAreaSettings.areaOfEffect, paintAtLocationStruct.paintAtAreaSettings.fallOffStrength), printLogsToScreen, printLogsToOutputLog, FColor::Green, printLogsToScreen_Duration);
	}


	vertexPaintGameInstanceSubsystem_Global->AddCalculateColorsTaskToQueue(calculateColorsInfoTemp);
}

void UVertexPaintDetectionComponent::PaintOnMeshAtLocationTaskFinished() {

	if (amountOfPaintAtLocationTasksCache > 0)
		amountOfPaintAtLocationTasksCache--;
}


//-------------------------------------------------------

// Paint Within Area

void UVertexPaintDetectionComponent::PaintOnMeshWithinArea(FVertexPainthWithinAreaStruct paintWithinAreaStruct, FVertexDetectAdditionalDataToPassThrough additionalDataToPassThrough) {


	amountOfPaintWithinAreaTasksCache++;

	if (IsValid(paintWithinAreaStruct.meshComponent)) {

		if (IsValid(paintWithinAreaStruct.meshComponent->GetOwner())) {

			paintWithinAreaStruct.actor = paintWithinAreaStruct.meshComponent->GetOwner();

			if (printLogsToScreen || printLogsToOutputLog)
				UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Runtime Vertex Color Paint and Detection Plugin - Trying to Paint On Mesh Within Area on Actor: %s and Component: %s"), *paintWithinAreaStruct.meshComponent->GetOwner()->GetName(), *paintWithinAreaStruct.meshComponent->GetName()), printLogsToScreen, printLogsToOutputLog, FColor::Cyan, printLogsToScreen_Duration);
		}
	}


	// Clamps these between 0-1 in case the user set a different value
	paintWithinAreaStruct.callbackSettings.includeAmountOfPaintedColorsOfEachChannel = UVertexPaintFunctionLibrary::ClampAmountOfPaintedColorsOfEachChannel(paintWithinAreaStruct.callbackSettings.includeAmountOfPaintedColorsOfEachChannel);
	paintWithinAreaStruct.limitPaintColor = UVertexPaintFunctionLibrary::ClampPaintLimits(paintWithinAreaStruct.limitPaintColor);


	FCalculateColorsInfo calculateColorsInfoTemp;
	calculateColorsInfoTemp.paintDetectionType = EVertexPaintDetectionType::PaintWithinArea;
	calculateColorsInfoTemp.paintOnMeshWithinAreaSettings = paintWithinAreaStruct;
	calculateColorsInfoTemp.additionalDataToPassThrough = additionalDataToPassThrough;
	calculateColorsInfoTemp.initiatedByComponent = this;
	calculateColorsInfoTemp.paintOnMeshColorSettings = paintWithinAreaStruct;
	calculateColorsInfoTemp.vertexPaintSettings = paintWithinAreaStruct;

	FillCalculateColorsInfoDebugSettings(calculateColorsInfoTemp);
	UVertexPaintFunctionLibrary::FillCalculateColorsInfoFundementals(paintWithinAreaStruct, calculateColorsInfoTemp);


	bool passedChecks_Local = true;

	if (!UVertexPaintFunctionLibrary::CheckValidFundementals(paintWithinAreaStruct.meshComponent, printLogsToScreen, printLogsToOutputLog, printLogsToScreen_Duration))
		passedChecks_Local = false;

	if (passedChecks_Local && !UVertexPaintFunctionLibrary::CheckValidVertexPaintSettings(paintWithinAreaStruct, printLogsToScreen, printLogsToOutputLog, printLogsToScreen_Duration))
		passedChecks_Local = false;

	if (passedChecks_Local && !UVertexPaintFunctionLibrary::CheckValidVertexPaintColorSettings(paintWithinAreaStruct, printLogsToScreen, printLogsToOutputLog, printLogsToScreen_Duration))
		passedChecks_Local = false;
	

	// If passed fundamental checks, then checks paint within area specific ones
	if (passedChecks_Local && paintWithinAreaStruct.componentsToCheckIfIsWithin.Num() <= 0) {

		if (printLogsToScreen || printLogsToOutputLog)
			UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Runtime Vertex Color Paint and Detection Plugin - Paint Within Area Check Valid Fail! It has no componentsToCheckIfIsWithin!")), printLogsToScreen, printLogsToOutputLog, FColor::Red, printLogsToScreen_Duration);

		passedChecks_Local = false;
	}

	if (passedChecks_Local) {

		for (auto compToCheckWithinTemp : paintWithinAreaStruct.componentsToCheckIfIsWithin) {

			if (IsValid(compToCheckWithinTemp.componentToCheckIfIsWithin)) {

				// Some component types require a specific shape to be set
				if (Cast<ALandscape>(compToCheckWithinTemp.componentToCheckIfIsWithin->GetOwner())) {

					if (compToCheckWithinTemp.paintWithinAreaShape != Enum_PaintWithinAreaShape::isComplexShape) {

						if (printLogsToScreen || printLogsToOutputLog)
							UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Runtime Vertex Color Paint and Detection Plugin - Paint Within Area Check Valid Failed since one of the componentToCheckIfWithin is a Landscape, but the PaintWithinArea Shape is not set to be Complex Shape which is required to check if you're within Landscapes!")), printLogsToScreen, printLogsToOutputLog, FColor::Red, printLogsToScreen_Duration);

						passedChecks_Local = false;
						break;
					}
				}

				else if (Cast<USphereComponent>(compToCheckWithinTemp.componentToCheckIfIsWithin)) {

					if (compToCheckWithinTemp.paintWithinAreaShape != Enum_PaintWithinAreaShape::isSphereShape) {


						if (printLogsToScreen || printLogsToOutputLog)
							UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Runtime Vertex Color Paint and Detection Plugin - Paint Within Area Check Valid Failed since one of the componentToCheckIfWithin is a Sphere Component, but the PaintWithinArea Shape is not set to be sphereShape!")), printLogsToScreen, printLogsToOutputLog, FColor::Red, printLogsToScreen_Duration);

						passedChecks_Local = false;
						break;
					}
				}

				else if (Cast<UBoxComponent>(compToCheckWithinTemp.componentToCheckIfIsWithin)) {

					if (compToCheckWithinTemp.paintWithinAreaShape != Enum_PaintWithinAreaShape::isSquareOrRectangleShape) {


						if (printLogsToScreen || printLogsToOutputLog)
							UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Runtime Vertex Color Paint and Detection Plugin - Paint Within Area Check Valid Failed since one of the componentToCheckIfWithin is a Box Component, but the PaintWithinArea Shape is not set to be Square or Rectangle Shape!")), printLogsToScreen, printLogsToOutputLog, FColor::Red, printLogsToScreen_Duration);

						passedChecks_Local = false;
						break;
					}
				}

				else if (Cast<USkeletalMeshComponent>(compToCheckWithinTemp.componentToCheckIfIsWithin)) {

					if (compToCheckWithinTemp.paintWithinAreaShape != Enum_PaintWithinAreaShape::isComplexShape) {

						if (printLogsToScreen || printLogsToOutputLog)
							UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Runtime Vertex Color Paint and Detection Plugin - Paint Within Area Check Valid Failed since one of the componentToCheckIfWithin is a Skeletal Mesh Component, but the PaintWithinArea Shape is not set to be Complex!")), printLogsToScreen, printLogsToOutputLog, FColor::Red, printLogsToScreen_Duration);

						passedChecks_Local = false;
						break;
					}
				}

				else if (Cast<USplineMeshComponent>(compToCheckWithinTemp.componentToCheckIfIsWithin)) {

					if (compToCheckWithinTemp.paintWithinAreaShape != Enum_PaintWithinAreaShape::isComplexShape) {

						if (printLogsToScreen || printLogsToOutputLog)
							UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Runtime Vertex Color Paint and Detection Plugin - Paint Within Area Check Valid Failed since one of the componentToCheckIfWithin is a Spline Mesh Component, but the PaintWithinArea Shape is not set to be Complex!")), printLogsToScreen, printLogsToOutputLog, FColor::Red, printLogsToScreen_Duration);

						passedChecks_Local = false;
						break;
					}
				}

				if (compToCheckWithinTemp.paintWithinAreaShape == Enum_PaintWithinAreaShape::isComplexShape) {

					// 
				}
			}

			else {

				if (printLogsToScreen || printLogsToOutputLog)
					UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Runtime Vertex Color Paint and Detection Plugin - Paint Within Area Check Valid Fail! One of the componentsToCheckIfIsWithin is Not Valid! ")), printLogsToScreen, printLogsToOutputLog, FColor::Red, printLogsToScreen_Duration);

				passedChecks_Local = false;
			}
		}
	}


	if (!passedChecks_Local) {

		if (printLogsToScreen || printLogsToOutputLog)
			UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Runtime Vertex Color Paint and Detection Plugin - Check Valid Failed at Paint on Mesh Within Area!")), printLogsToScreen, printLogsToOutputLog, FColor::Red, printLogsToScreen_Duration);

		UVertexPaintFunctionLibrary::RunPaintWithinAreaCallbacks(calculateColorsInfoTemp);
		return;
	}


	if (paintWithinAreaStruct.applyVertexColorSettings.applyColorsUsingPhysicsSurface.applyVertexColorUsingPhysicsSurface) {

		if (printLogsToScreen || printLogsToOutputLog)
			UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Runtime Vertex Color Paint and Detection Plugin - Paint on Mesh Within Area CheckValid Successfull. Trying to Apply Color with EPhysicalSurface: %i  with Paint Strength: %f  -  Fall Back to RGBA Colors if Failed to Apply Colors Using Physics Surface: %i  -  Paint Strength on Surfaces Without the Physical Surface: %f"), paintWithinAreaStruct.applyVertexColorSettings.applyColorsUsingPhysicsSurface.physicsSurfaceToApply, paintWithinAreaStruct.applyVertexColorSettings.applyColorsUsingPhysicsSurface.paintStrengthToApply, paintWithinAreaStruct.applyVertexColorSettings.applyColorsUsingPhysicsSurface.fallbackToRGBAColorsIfFailedToApplyColorsUsingPhysicsSurface, paintWithinAreaStruct.applyVertexColorSettings.applyColorsUsingPhysicsSurface.paintStrengtOnChannelsWithoutThePhysicsSurface), printLogsToScreen, printLogsToOutputLog, FColor::Green, printLogsToScreen_Duration);
	}

	else {

		if (printLogsToScreen || printLogsToOutputLog)
			UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Runtime Vertex Color Paint and Detection Plugin - Paint on Mesh Within Area CheckValid Successfull. Trying to Apply Color: %s"), *FLinearColor(paintWithinAreaStruct.applyVertexColorSettings.redColor, paintWithinAreaStruct.applyVertexColorSettings.greenColor, paintWithinAreaStruct.applyVertexColorSettings.blueColor, paintWithinAreaStruct.applyVertexColorSettings.alphaColor).ToString()), printLogsToScreen, printLogsToOutputLog, FColor::Green, printLogsToScreen_Duration);
	}



	if (calculateColorsInfoTemp.paintOnMeshWithinAreaSettings.actor) {

		for (auto compsToCheckIfWithinTemp : calculateColorsInfoTemp.paintOnMeshWithinAreaSettings.componentsToCheckIfIsWithin) {


			// We can cache all the things we need such as transform, forward vector etc. so we know where the component was and it's necessary values When the task was created, so in case there is a high task queue and it won't be run until a few seconds later, it will still be calculated from the same place as created. 
			compsToCheckIfWithinTemp.componentToCheckIfIsWithin_ForwardVector = compsToCheckIfWithinTemp.componentToCheckIfIsWithin->GetForwardVector();
			compsToCheckIfWithinTemp.componentToCheckIfIsWithin_RightVector = compsToCheckIfWithinTemp.componentToCheckIfIsWithin->GetRightVector();
			compsToCheckIfWithinTemp.componentToCheckIfIsWithin_UpVector = compsToCheckIfWithinTemp.componentToCheckIfIsWithin->GetUpVector();

			compsToCheckIfWithinTemp.componentToCheckIfIsWithin_Bounds = compsToCheckIfWithinTemp.componentToCheckIfIsWithin->Bounds;
			compsToCheckIfWithinTemp.componentToCheckIfIsWithin->GetOwner()->GetActorBounds(false, compsToCheckIfWithinTemp.componentToCheckIfIsWithin_ActorBounds_Origin, compsToCheckIfWithinTemp.componentToCheckIfIsWithin_ActorBounds_Extent, true);

			compsToCheckIfWithinTemp.componentToCheckIfIsWithin_RelativeScale = compsToCheckIfWithinTemp.componentToCheckIfIsWithin->GetRelativeTransform().GetScale3D();

			// Caches World Transform as well and adjusts it down below so we can save Location, Rotation, Scale in mesh compoent to paint component space. Then when we actually do the Task, we transform those back into World Space, and then re-sets this to be them. So even if the task starts much later then when it was created, and things may have moved since then, we can check things relative to how it was when task was created. 
			compsToCheckIfWithinTemp.componentToCheckIfIsWithin_WorldTransform = compsToCheckIfWithinTemp.componentToCheckIfIsWithin->GetComponentTransform();


			if (auto boxComp = Cast<UBoxComponent>(compsToCheckIfWithinTemp.componentToCheckIfIsWithin)) {

				compsToCheckIfWithinTemp.componentToCheckIfIsWithin_IsBoxComponent = true;
				compsToCheckIfWithinTemp.componentToCheckIfIsWithin_ScaledBoxExtent = boxComp->GetScaledBoxExtent();

				compsToCheckIfWithinTemp.componentToCheckIfIsWithin_IsVertexWithinArea_CubeOrRectangle_SquareXHalfSize = boxComp->GetScaledBoxExtent().X;
				compsToCheckIfWithinTemp.componentToCheckIfIsWithin_IsVertexWithinArea_CubeOrRectangle_SquareYHalfSize = boxComp->GetScaledBoxExtent().Y;
				compsToCheckIfWithinTemp.componentToCheckIfIsWithin_IsVertexWithinArea_CubeOrRectangle_SquareZHalfSize = boxComp->GetScaledBoxExtent().Z;


				if (calculateColorsInfoTemp.drawPaintWithinAreaDebugSymbols)
					DrawDebugBox(calculateColorsInfoTemp.paintOnMeshWithinAreaSettings.meshComponent->GetWorld(), compsToCheckIfWithinTemp.componentToCheckIfIsWithin_Bounds.GetBox().GetCenter(), compsToCheckIfWithinTemp.componentToCheckIfIsWithin_Bounds.GetBox().GetExtent(), FColor::Red, false, calculateColorsInfoTemp.drawPaintWithinAreaDebugSymbolsDuration, 0, 5);
			}

			else if (auto sphereComp = Cast<USphereComponent>(compsToCheckIfWithinTemp.componentToCheckIfIsWithin)) {

				compsToCheckIfWithinTemp.componentToCheckIfIsWithin_IsSphereComponent = true;
				compsToCheckIfWithinTemp.componentToCheckIfIsWithin_IsVertexWithinArea_SphereShape = sphereComp->GetScaledSphereRadius();

				if (calculateColorsInfoTemp.drawPaintWithinAreaDebugSymbols)
					DrawDebugBox(calculateColorsInfoTemp.paintOnMeshWithinAreaSettings.meshComponent->GetWorld(), compsToCheckIfWithinTemp.componentToCheckIfIsWithin_Bounds.GetBox().GetCenter(), compsToCheckIfWithinTemp.componentToCheckIfIsWithin_Bounds.GetBox().GetExtent(), FColor::Red, false, calculateColorsInfoTemp.drawPaintWithinAreaDebugSymbolsDuration, 0, 5);
			}

			else if (auto staticMeshComp = Cast<UStaticMeshComponent>(compsToCheckIfWithinTemp.componentToCheckIfIsWithin)) {

				// Since we can get Get Location on Spline Mesh Components we have to use their Box Get Center so we could still use cached World_Transform for somethings
				if (Cast<USplineMeshComponent>(compsToCheckIfWithinTemp.componentToCheckIfIsWithin)) {


					compsToCheckIfWithinTemp.componentToCheckIfIsWithin_IsSplineMeshComponent = true;
					compsToCheckIfWithinTemp.componentToCheckIfIsWithin_WorldTransform.SetLocation(compsToCheckIfWithinTemp.componentToCheckIfIsWithin_Bounds.GetBox().GetCenter());


					// Gets the AggGeom Bounds so we can get the thickness with any added box collisions the mesh may have. 
					FBoxSphereBounds boundsTemp;
					compsToCheckIfWithinTemp.componentToCheckIfIsWithin->GetBodySetup()->AggGeom.CalcBoxSphereBounds(boundsTemp, compsToCheckIfWithinTemp.componentToCheckIfIsWithin_WorldTransform);

					// Updated the extent thickness so we have the proper one with any added collisions
					compsToCheckIfWithinTemp.componentToCheckIfIsWithin_Bounds.BoxExtent.Z = boundsTemp.BoxExtent.Z;


					FVector adjustedLocationWithMeshBoxOffsetTakenIntoAccount = compsToCheckIfWithinTemp.componentToCheckIfIsWithin->Bounds.GetBox().GetCenter();

					// To get the extent from the correct center location we take added collisions into account as well so the bounds match
					if (staticMeshComp->GetStaticMesh()->GetBodySetup()) {

						if (staticMeshComp->GetStaticMesh()->GetBodySetup()->AggGeom.GetElementCount() > 0) {

							float differenceFromMeshLocationAndBoxCollisionZ = 0;


							for (int i = 0; i < staticMeshComp->GetStaticMesh()->GetBodySetup()->AggGeom.BoxElems.Num(); i++)
								differenceFromMeshLocationAndBoxCollisionZ += staticMeshComp->GetStaticMesh()->GetBodySetup()->AggGeom.BoxElems[i].Center.Z;

							for (int i = 0; i < compsToCheckIfWithinTemp.componentToCheckIfIsWithin->GetBodySetup()->AggGeom.ConvexElems.Num(); i++)
								differenceFromMeshLocationAndBoxCollisionZ += compsToCheckIfWithinTemp.componentToCheckIfIsWithin->GetBodySetup()->AggGeom.ConvexElems[i].GetTransform().GetLocation().Z;

							for (int i = 0; i < compsToCheckIfWithinTemp.componentToCheckIfIsWithin->GetBodySetup()->AggGeom.SphereElems.Num(); i++)
								differenceFromMeshLocationAndBoxCollisionZ += compsToCheckIfWithinTemp.componentToCheckIfIsWithin->GetBodySetup()->AggGeom.SphereElems[i].Center.Z;

							for (int i = 0; i < compsToCheckIfWithinTemp.componentToCheckIfIsWithin->GetBodySetup()->AggGeom.SphylElems.Num(); i++)
								differenceFromMeshLocationAndBoxCollisionZ += compsToCheckIfWithinTemp.componentToCheckIfIsWithin->GetBodySetup()->AggGeom.SphylElems[i].Center.Z;

							for (int i = 0; i < compsToCheckIfWithinTemp.componentToCheckIfIsWithin->GetBodySetup()->AggGeom.TaperedCapsuleElems.Num(); i++)
								differenceFromMeshLocationAndBoxCollisionZ += compsToCheckIfWithinTemp.componentToCheckIfIsWithin->GetBodySetup()->AggGeom.TaperedCapsuleElems[i].Center.Z;


							differenceFromMeshLocationAndBoxCollisionZ /= staticMeshComp->GetStaticMesh()->GetBodySetup()->AggGeom.GetElementCount();
							adjustedLocationWithMeshBoxOffsetTakenIntoAccount.Z += differenceFromMeshLocationAndBoxCollisionZ;

							// Sets World Transform Location to be the adjusted location so it's at the Real Center
							compsToCheckIfWithinTemp.componentToCheckIfIsWithin_WorldTransform.SetLocation(adjustedLocationWithMeshBoxOffsetTakenIntoAccount);
						}
					}


					if (calculateColorsInfoTemp.drawPaintWithinAreaDebugSymbols) {

						DrawDebugBox(calculateColorsInfoTemp.paintOnMeshWithinAreaSettings.meshComponent->GetWorld(), compsToCheckIfWithinTemp.componentToCheckIfIsWithin_WorldTransform.GetLocation(), compsToCheckIfWithinTemp.componentToCheckIfIsWithin_Bounds.BoxExtent, FColor::Red, false, calculateColorsInfoTemp.drawPaintWithinAreaDebugSymbolsDuration, 0, 5);
					}
				}

				else {

					compsToCheckIfWithinTemp.componentToCheckIfIsWithin_IsStaticMeshComponent = true;

					// Calc Box Sphere Bounds Without taking the Rotation into account, this is because if for instance a rectangle is rotated, then that affects the bounds and the result we would get here would be the correct extents of the mesh and it's collision, and it is the Extent that is what we're after
					FTransform transformWithoutRotation_Local;
					transformWithoutRotation_Local.SetLocation(compsToCheckIfWithinTemp.componentToCheckIfIsWithin_WorldTransform.GetLocation());
					transformWithoutRotation_Local.SetScale3D(FVector(1, 1, 1));

					// Always runs this so we have proper Bounds no matter what paint within area type, so even if complex shape on static mesh component etc. 
					compsToCheckIfWithinTemp.componentToCheckIfIsWithin->GetBodySetup()->AggGeom.CalcBoxSphereBounds(compsToCheckIfWithinTemp.componentToCheckIfIsWithin_CleanAggGeomBounds, transformWithoutRotation_Local);

					// Also gets the Bounds but with Actual Rotation and Scale taken into account
					transformWithoutRotation_Local.SetScale3D(compsToCheckIfWithinTemp.componentToCheckIfIsWithin_WorldTransform.GetScale3D());
					transformWithoutRotation_Local.SetRotation(compsToCheckIfWithinTemp.componentToCheckIfIsWithin_WorldTransform.GetRotation());
					compsToCheckIfWithinTemp.componentToCheckIfIsWithin->GetBodySetup()->AggGeom.CalcBoxSphereBounds(compsToCheckIfWithinTemp.componentToCheckIfIsWithin_Bounds, transformWithoutRotation_Local);


					// If a Cube / Rectangle then checks if it has box collisions in it's static mesh, if so then we have to adjust the world transform so that it's location is in the center between the actor and the collision. For instance if you have a floor, with added box collision underneath it that stretches 1M, without this we would get the bounds offset since the location of the actor is at the floor height. But with this we can get the actual center location of the Bounds since it takes into account the box collision
					if (compsToCheckIfWithinTemp.paintWithinAreaShape == Enum_PaintWithinAreaShape::isSquareOrRectangleShape) {

						// To get the extent from the correct center location that takes added box collision into account we had to do this. 
						if (compsToCheckIfWithinTemp.componentToCheckIfIsWithin->GetBodySetup()) {


							FVector adjustedLocationWithMeshBoxOffsetTakenIntoAccount = compsToCheckIfWithinTemp.componentToCheckIfIsWithin->Bounds.GetBox().GetCenter();
							float differenceFromMeshLocationAndBoxCollisionZ = 0;


							if (compsToCheckIfWithinTemp.componentToCheckIfIsWithin->GetBodySetup()->AggGeom.BoxElems.Num() > 0) {

								// Sets them to a very high values so we will always set the min and max values down below. If they where 0 we would've had an issue where if for instance there where two boxes at -50 and -100, then none would be higher then maxValue 0 and -50 wouldn't get set to be the maxValue as it should. 
								float maxZValue_Local = -10000000;
								float minZValue_Local = 10000000;

								for (int i = 0; i < compsToCheckIfWithinTemp.componentToCheckIfIsWithin->GetBodySetup()->AggGeom.BoxElems.Num(); i++) {

									// Gets the current min and max values based on their position and their extent
									float currentBoxMaxZ = compsToCheckIfWithinTemp.componentToCheckIfIsWithin->GetBodySetup()->AggGeom.BoxElems[i].Center.Z + (compsToCheckIfWithinTemp.componentToCheckIfIsWithin->GetBodySetup()->AggGeom.BoxElems[i].Z / 2);

									float currentBoxMinZ = compsToCheckIfWithinTemp.componentToCheckIfIsWithin->GetBodySetup()->AggGeom.BoxElems[i].Center.Z - (compsToCheckIfWithinTemp.componentToCheckIfIsWithin->GetBodySetup()->AggGeom.BoxElems[i].Z / 2);


									// Caches the the Highest and Lowest Z, so we can get the correct center point of them for the bounds
									if (currentBoxMaxZ > maxZValue_Local)
										maxZValue_Local = currentBoxMaxZ;

									if (currentBoxMinZ < minZValue_Local)
										minZValue_Local = currentBoxMinZ;
								}

								// The Middle point between the max and min values
								differenceFromMeshLocationAndBoxCollisionZ = (maxZValue_Local + minZValue_Local) / 2;
							}

							adjustedLocationWithMeshBoxOffsetTakenIntoAccount.Z += differenceFromMeshLocationAndBoxCollisionZ;


							// Sets World Transform Location to be the adjusted location so it's at the Real Center
							compsToCheckIfWithinTemp.componentToCheckIfIsWithin_WorldTransform.SetLocation(adjustedLocationWithMeshBoxOffsetTakenIntoAccount);

							// If there's more than 1 Box, and they're not centered on top of eachother then it seizes to be a Square or Rectangle, so throws a Error warning about that if there's more than 1. Should be a pretty rare case though. 
							if (compsToCheckIfWithinTemp.componentToCheckIfIsWithin->GetBodySetup()->AggGeom.BoxElems.Num() > 1) {

								if (calculateColorsInfoTemp.printLogsToScreen || calculateColorsInfoTemp.printLogsToOutputLog)
									UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Runtime Vertex Color Paint and Detection Plugin - Paint Within Area with Square or Rectangle for Static Mesh Component. NOTE That the Static Mesh has more than 1 Box Collision under it. If they're not centered then then it's not a Square or Rectangle anymore and the Result may not be the expected one. ")), calculateColorsInfoTemp.printLogsToScreen, calculateColorsInfoTemp.printLogsToOutputLog, FColor::Orange, calculateColorsInfoTemp.printLogsToScreen_Duration);
							}
						}

						compsToCheckIfWithinTemp.componentToCheckIfIsWithin_IsVertexWithinArea_CubeOrRectangle_SquareXHalfSize = compsToCheckIfWithinTemp.componentToCheckIfIsWithin_CleanAggGeomBounds.GetBox().GetExtent().X * compsToCheckIfWithinTemp.componentToCheckIfIsWithin_WorldTransform.GetScale3D().X;
						compsToCheckIfWithinTemp.componentToCheckIfIsWithin_IsVertexWithinArea_CubeOrRectangle_SquareYHalfSize = compsToCheckIfWithinTemp.componentToCheckIfIsWithin_CleanAggGeomBounds.GetBox().GetExtent().Y * compsToCheckIfWithinTemp.componentToCheckIfIsWithin_WorldTransform.GetScale3D().Y;
						compsToCheckIfWithinTemp.componentToCheckIfIsWithin_IsVertexWithinArea_CubeOrRectangle_SquareZHalfSize = compsToCheckIfWithinTemp.componentToCheckIfIsWithin_CleanAggGeomBounds.GetBox().GetExtent().Z * compsToCheckIfWithinTemp.componentToCheckIfIsWithin_WorldTransform.GetScale3D().Z;
					}

					// Uses AggGeom Bounds so we get the correct bounds even if the mesh is 1, 1, 1 in scale but much larger than 50cm radius
					else if (compsToCheckIfWithinTemp.paintWithinAreaShape == Enum_PaintWithinAreaShape::isSphereShape) {

						compsToCheckIfWithinTemp.componentToCheckIfIsWithin_IsVertexWithinArea_SphereShape = (compsToCheckIfWithinTemp.componentToCheckIfIsWithin_CleanAggGeomBounds.SphereRadius * compsToCheckIfWithinTemp.componentToCheckIfIsWithin->GetComponentTransform().GetScale3D().X);;
					}

					if (calculateColorsInfoTemp.drawPaintWithinAreaDebugSymbols)
						DrawDebugBox(calculateColorsInfoTemp.paintOnMeshWithinAreaSettings.meshComponent->GetWorld(), compsToCheckIfWithinTemp.componentToCheckIfIsWithin_WorldTransform.GetLocation(), compsToCheckIfWithinTemp.componentToCheckIfIsWithin_CleanAggGeomBounds.GetBox().GetExtent() * compsToCheckIfWithinTemp.componentToCheckIfIsWithin_WorldTransform.GetScale3D(), FColor::Red, false, calculateColorsInfoTemp.drawPaintWithinAreaDebugSymbolsDuration, 0, 5);
				}
			}

			else if (auto skeletalMeshComp = Cast<USkeletalMeshComponent>(compsToCheckIfWithinTemp.componentToCheckIfIsWithin)) {


				USkeletalMesh* paintWithinAreaSkelMeshTemp = nullptr;

#if ENGINE_MAJOR_VERSION == 4

				paintWithinAreaSkelMeshTemp = skeletalMeshComp->SkeletalMesh;

#elif ENGINE_MAJOR_VERSION == 5

#if ENGINE_MINOR_VERSION == 0

				paintWithinAreaSkelMeshTemp = skeletalMeshComp->SkeletalMesh.Get();

#else

				paintWithinAreaSkelMeshTemp = skeletalMeshComp->GetSkeletalMeshAsset();

#endif

#endif


				compsToCheckIfWithinTemp.componentToCheckIfIsWithin_Bounds = paintWithinAreaSkelMeshTemp->GetPhysicsAsset()->CalcAABB(skeletalMeshComp, skeletalMeshComp->GetComponentTransform());

				FVector locationTemp = compsToCheckIfWithinTemp.componentToCheckIfIsWithin_WorldTransform.GetLocation();
				locationTemp.Z += compsToCheckIfWithinTemp.componentToCheckIfIsWithin_Bounds.GetBox().GetCenter().Z - compsToCheckIfWithinTemp.componentToCheckIfIsWithin_WorldTransform.GetLocation().Z;
				compsToCheckIfWithinTemp.componentToCheckIfIsWithin_WorldTransform.SetLocation(locationTemp);

				compsToCheckIfWithinTemp.componentToCheckIfIsWithin_IsSkeletalMeshComponent = true;

				if (calculateColorsInfoTemp.drawPaintWithinAreaDebugSymbols)
					DrawDebugBox(calculateColorsInfoTemp.paintOnMeshWithinAreaSettings.meshComponent->GetWorld(), compsToCheckIfWithinTemp.componentToCheckIfIsWithin_WorldTransform.GetLocation(), compsToCheckIfWithinTemp.componentToCheckIfIsWithin_Bounds.GetBox().GetExtent(), FColor::Red, false, calculateColorsInfoTemp.drawPaintWithinAreaDebugSymbolsDuration, 0, 5);
			}

			else if (Cast<ALandscape>(compsToCheckIfWithinTemp.componentToCheckIfIsWithin->GetOwner())) {

				compsToCheckIfWithinTemp.componentToCheckIfIsWithin_IsLandscape = true;

				if (calculateColorsInfoTemp.drawPaintWithinAreaDebugSymbols)
					DrawDebugBox(calculateColorsInfoTemp.paintOnMeshWithinAreaSettings.meshComponent->GetWorld(), compsToCheckIfWithinTemp.componentToCheckIfIsWithin_Bounds.GetBox().GetCenter(), compsToCheckIfWithinTemp.componentToCheckIfIsWithin_Bounds.GetBox().GetExtent(), FColor::Red, false, calculateColorsInfoTemp.drawPaintWithinAreaDebugSymbolsDuration, 0, 5);
			}


			// After any adjustments to world transform, saves the location in meshComponent that we're gonna paint space, so we know relative to it, the componentToCheckIfIsWithin is. So when the task is finally run, we can convert it from that to world again, and check, where was it and what vertices are within it. Fixes any issue where paint within area is run and the task queue is high etc. 
			compsToCheckIfWithinTemp.componentToCheckIfIsWithin_LocationInMeshComponentToPaintSpace = UKismetMathLibrary::InverseTransformLocation(calculateColorsInfoTemp.paintOnMeshWithinAreaSettings.meshComponent->GetComponentTransform(), compsToCheckIfWithinTemp.componentToCheckIfIsWithin_WorldTransform.GetLocation());

			compsToCheckIfWithinTemp.componentToCheckIfIsWithin_RotationInMeshComponentToPaintSpace = UKismetMathLibrary::InverseTransformRotation(calculateColorsInfoTemp.paintOnMeshWithinAreaSettings.meshComponent->GetComponentTransform(), compsToCheckIfWithinTemp.componentToCheckIfIsWithin_WorldTransform.GetRotation().Rotator());

			compsToCheckIfWithinTemp.componentToCheckIfIsWithin_ScaleInMeshComponentToPaintSpace = UKismetMathLibrary::InverseTransformLocation(calculateColorsInfoTemp.paintOnMeshWithinAreaSettings.meshComponent->GetComponentTransform(), compsToCheckIfWithinTemp.componentToCheckIfIsWithin_WorldTransform.GetScale3D());


			calculateColorsInfoTemp.paintOnMeshWithinArea_ComponentsToCheckIfIsWithinInfo.Add(compsToCheckIfWithinTemp);
		}
	}
	

	vertexPaintGameInstanceSubsystem_Global->AddCalculateColorsTaskToQueue(calculateColorsInfoTemp);
}

void UVertexPaintDetectionComponent::PaintOnMeshWithinAreaTaskFinished() {

	if (amountOfPaintWithinAreaTasksCache > 0)
		amountOfPaintWithinAreaTasksCache--;
}


//-------------------------------------------------------

// Paint On Entire Mesh

void UVertexPaintDetectionComponent::PaintOnEntireMesh(FVertexPaintOnEntireMeshStruct paintOnEntireMeshStruct, FVertexDetectAdditionalDataToPassThrough additionalDataToPassThrough) {


	amountOfPaintEntireMeshTasksCache++;

	if (IsValid(paintOnEntireMeshStruct.meshComponent)) {

		if (IsValid(paintOnEntireMeshStruct.meshComponent->GetOwner())) {

			paintOnEntireMeshStruct.actor = paintOnEntireMeshStruct.meshComponent->GetOwner();

			if (printLogsToScreen || printLogsToOutputLog)
				UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Runtime Vertex Color Paint and Detection Plugin - Trying to Paint Entire Mesh on Actor: %s and Component: %s"), *paintOnEntireMeshStruct.meshComponent->GetOwner()->GetName(), *paintOnEntireMeshStruct.meshComponent->GetName()), printLogsToScreen, printLogsToOutputLog, FColor::Cyan, printLogsToScreen_Duration);
		}
	}


	// Clamps these between 0-1 in case the user set a different value
	paintOnEntireMeshStruct.callbackSettings.includeAmountOfPaintedColorsOfEachChannel = UVertexPaintFunctionLibrary::ClampAmountOfPaintedColorsOfEachChannel(paintOnEntireMeshStruct.callbackSettings.includeAmountOfPaintedColorsOfEachChannel);
	paintOnEntireMeshStruct.limitPaintColor = UVertexPaintFunctionLibrary::ClampPaintLimits(paintOnEntireMeshStruct.limitPaintColor);


	FCalculateColorsInfo calculateColorsInfoTemp;
	calculateColorsInfoTemp.paintDetectionType = EVertexPaintDetectionType::PaintEntireMesh;
	calculateColorsInfoTemp.paintOnEntireMeshSettings = paintOnEntireMeshStruct;
	calculateColorsInfoTemp.initiatedByComponent = this;
	calculateColorsInfoTemp.additionalDataToPassThrough = additionalDataToPassThrough;
	calculateColorsInfoTemp.paintOnMeshColorSettings = paintOnEntireMeshStruct;
	calculateColorsInfoTemp.vertexPaintSettings = paintOnEntireMeshStruct;

	FillCalculateColorsInfoDebugSettings(calculateColorsInfoTemp);
	UVertexPaintFunctionLibrary::FillCalculateColorsInfoFundementals(paintOnEntireMeshStruct, calculateColorsInfoTemp);


	bool passedChecks_Local = true;


	if (!UVertexPaintFunctionLibrary::CheckValidFundementals(paintOnEntireMeshStruct.meshComponent, printLogsToScreen, printLogsToOutputLog, printLogsToScreen_Duration))
		passedChecks_Local = false;

	if (!UVertexPaintFunctionLibrary::CheckValidVertexPaintSettings(paintOnEntireMeshStruct, printLogsToScreen, printLogsToOutputLog, printLogsToScreen_Duration))
		passedChecks_Local = false;

	if (!UVertexPaintFunctionLibrary::CheckValidVertexPaintColorSettings(paintOnEntireMeshStruct, printLogsToScreen, printLogsToOutputLog, printLogsToScreen_Duration))
		passedChecks_Local = false;


	// If passed fundamental checks, then checks paint entire mesh specific ones
	if (passedChecks_Local && IsValid(paintOnEntireMeshStruct.meshComponent)) {

		if (paintOnEntireMeshStruct.paintOnRandomVerticesSettings.paintAtRandomVerticesSpreadOutOverTheEntireMesh) {

			if (paintOnEntireMeshStruct.paintOnRandomVerticesSettings.paintAtRandomVerticesSpreadOutOverTheEntireMesh_PercentToPaint <= 0) {

				if (printLogsToScreen || printLogsToOutputLog)
					UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Runtime Vertex Color Paint and Detection Plugin - Check Valid, Paint at Entire Mesh Fail: Set to Paint Random Vertices Spread Out Over the Entire Mesh but the Percent to Spread out is 0 when it should be between 0-100. ")), printLogsToScreen, printLogsToOutputLog, FColor::Red, printLogsToScreen_Duration);

				passedChecks_Local = false;
			}
			else if (paintOnEntireMeshStruct.paintOnRandomVerticesSettings.paintAtRandomVerticesSpreadOutOverTheEntireMesh_PercentToPaint >= 100) {

				if (printLogsToScreen || printLogsToOutputLog)
					UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Runtime Vertex Color Paint and Detection Plugin - Check Valid, Paint at Entire Mesh Fail: Set to Paint Random Vertices Spread Out Over the Entire Mesh to 100%, meaning the Entire Mesh should be painted, which makes it unnecessary and un-optimized to use Paint at Random Vertices when you can just turn that off and just use Paint Entire Mesh the regular way. ")), printLogsToScreen, printLogsToOutputLog, FColor::Red, printLogsToScreen_Duration);

				passedChecks_Local = false;
			}
		}

		if (passedChecks_Local) {

			int numberOfVertices_Local = 0;

			if (auto skelMeshComp = Cast<USkeletalMeshComponent>(paintOnEntireMeshStruct.meshComponent)) {

				if (skelMeshComp->GetSkeletalMeshRenderData()) {

					if (skelMeshComp->GetSkeletalMeshRenderData()->LODRenderData.IsValidIndex(0))
						numberOfVertices_Local = skelMeshComp->GetSkeletalMeshRenderData()->LODRenderData[0].GetNumVertices();
				}
			}

			else if (auto staticMeshComp = Cast<UStaticMeshComponent>(paintOnEntireMeshStruct.meshComponent)) {

				if (staticMeshComp->GetStaticMesh()) {

					if (staticMeshComp->GetStaticMesh()->GetRenderData()) {

						if (staticMeshComp->GetStaticMesh()->GetRenderData()->LODResources.IsValidIndex(0))
							numberOfVertices_Local = staticMeshComp->GetStaticMesh()->GetRenderData()->LODResources[0].GetNumVertices();
					}
				}
			}


			if (numberOfVertices_Local > 0 && paintOnEntireMeshStruct.paintOnRandomVerticesSettings.paintAtRandomVerticesSpreadOutOverTheEntireMesh) {

				if (UKismetMathLibrary::MapRangeClamped(paintOnEntireMeshStruct.paintOnRandomVerticesSettings.paintAtRandomVerticesSpreadOutOverTheEntireMesh_PercentToPaint, 0, 100, 0, numberOfVertices_Local) < 1) {

					if (printLogsToScreen || printLogsToOutputLog)
						UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Runtime Vertex Color Paint and Detection Plugin - Check Valid, Paint at Entire Mesh Fail: Set to Paint Random Vertices Spread Out Over the Entire Mesh but the Percent set to Spread out is so low that not even a single vertex at LOD0 would get painted.")), printLogsToScreen, printLogsToOutputLog, FColor::Red, printLogsToScreen_Duration);

					passedChecks_Local = false;
				}
			}
		}
	}


	if (!passedChecks_Local) {

		if (printLogsToScreen || printLogsToOutputLog)
			UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Runtime Vertex Color Paint and Detection Plugin - Check Valid Failed at Paint on Entire Mesh!")), printLogsToScreen, printLogsToOutputLog, FColor::Red, printLogsToScreen_Duration);

		UVertexPaintFunctionLibrary::RunPaintEntireMeshCallbacks(calculateColorsInfoTemp);
		return;
	}


	if (paintOnEntireMeshStruct.applyVertexColorSettings.applyColorsUsingPhysicsSurface.applyVertexColorUsingPhysicsSurface) {

		if (printLogsToScreen || printLogsToOutputLog)
			UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Runtime Vertex Color Paint and Detection Plugin - Paint on Entire Mesh CheckValid Successfull. Trying to Apply Color with EPhysicalSurface: %i  with Paint Strength: %f  -  Fall Back to RGBA Colors if Failed to Apply Colors Using Physics Surface: %i  -  Paint Strength on Surfaces Without the Physical Surface: %f"), paintOnEntireMeshStruct.applyVertexColorSettings.applyColorsUsingPhysicsSurface.physicsSurfaceToApply, paintOnEntireMeshStruct.applyVertexColorSettings.applyColorsUsingPhysicsSurface.paintStrengthToApply, paintOnEntireMeshStruct.applyVertexColorSettings.applyColorsUsingPhysicsSurface.fallbackToRGBAColorsIfFailedToApplyColorsUsingPhysicsSurface, paintOnEntireMeshStruct.applyVertexColorSettings.applyColorsUsingPhysicsSurface.paintStrengtOnChannelsWithoutThePhysicsSurface), printLogsToScreen, printLogsToOutputLog, FColor::Green, printLogsToScreen_Duration);
	}

	else {

		if (printLogsToScreen || printLogsToOutputLog)
			UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Runtime Vertex Color Paint and Detection Plugin - Paint on Entire Mesh CheckValid Successfull. Trying to Apply Color: %s"), *FLinearColor(paintOnEntireMeshStruct.applyVertexColorSettings.redColor, paintOnEntireMeshStruct.applyVertexColorSettings.greenColor, paintOnEntireMeshStruct.applyVertexColorSettings.blueColor, paintOnEntireMeshStruct.applyVertexColorSettings.alphaColor).ToString()), printLogsToScreen, printLogsToOutputLog, FColor::Green, printLogsToScreen_Duration);
	}


	vertexPaintGameInstanceSubsystem_Global->AddCalculateColorsTaskToQueue(calculateColorsInfoTemp);
}

void UVertexPaintDetectionComponent::PaintOnEntireMeshTaskFinished() {

	if (amountOfPaintEntireMeshTasksCache > 0)
		amountOfPaintEntireMeshTasksCache--;
}


//--------------------------------------------------------

// Paint Color Snippet on Mesh

void UVertexPaintDetectionComponent::PaintColorSnippetOnMesh(FVertexPaintColorSnippetStruct paintColorSnippetStruct, FVertexDetectAdditionalDataToPassThrough additionalDataToPassThrough) {


	amountOfPaintColorSnippetTasksCache++;

	if (IsValid(paintColorSnippetStruct.meshComponent)) {

		if (IsValid(paintColorSnippetStruct.meshComponent->GetOwner())) {

			paintColorSnippetStruct.actor = paintColorSnippetStruct.meshComponent->GetOwner();

			if (printLogsToScreen || printLogsToOutputLog)
				UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Runtime Vertex Color Paint and Detection Plugin - Trying to Paint Color Snippet on Actor: %s and Component: %s"), *paintColorSnippetStruct.meshComponent->GetOwner()->GetName(), *paintColorSnippetStruct.meshComponent->GetName()), printLogsToScreen, printLogsToOutputLog, FColor::Cyan, printLogsToScreen_Duration);
		}
	}


	// Clamps these between 0-1 in case the user set a different value
	paintColorSnippetStruct.callbackSettings.includeAmountOfPaintedColorsOfEachChannel = UVertexPaintFunctionLibrary::ClampAmountOfPaintedColorsOfEachChannel(paintColorSnippetStruct.callbackSettings.includeAmountOfPaintedColorsOfEachChannel);


	// Fills this up here as well eventhough we're gonna have to wait for a load callback and do it again later, so even if check valid fails we can return correct things using this
	FCalculateColorsInfo calculateColorsInfoTemp;
	calculateColorsInfoTemp.paintDetectionType = EVertexPaintDetectionType::PaintColorSnippet;
	calculateColorsInfoTemp.paintOnColorSnippetSettings = paintColorSnippetStruct;
	calculateColorsInfoTemp.initiatedByComponent = this;
	calculateColorsInfoTemp.additionalDataToPassThrough = additionalDataToPassThrough;
	calculateColorsInfoTemp.vertexPaintSettings = paintColorSnippetStruct;

	FillCalculateColorsInfoDebugSettings(calculateColorsInfoTemp);
	UVertexPaintFunctionLibrary::FillCalculateColorsInfoFundementals(paintColorSnippetStruct, calculateColorsInfoTemp);


	bool passedChecks_Local = true;

	if (!UVertexPaintFunctionLibrary::CheckValidFundementals(paintColorSnippetStruct.meshComponent, printLogsToScreen, printLogsToOutputLog, printLogsToScreen_Duration))
		passedChecks_Local = false;

	if (!UVertexPaintFunctionLibrary::CheckValidVertexPaintSettings(paintColorSnippetStruct, printLogsToScreen, printLogsToOutputLog, printLogsToScreen_Duration))
		passedChecks_Local = false;


	// If passed fundamental checks, then checks paint color snippet specific ones
	if (passedChecks_Local && !paintColorSnippetStruct.colorSnippetTag.IsValid() && !paintColorSnippetStruct.randomizeAnyColorSnippetTagUnderThisCategory.IsValid()) {

		if (printLogsToScreen || printLogsToOutputLog)
			UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Runtime Vertex Color Paint and Detection Plugin - PaintColorSnippet Failed because no Snippet Tag was provided and it's not set to Randomize a Snippet Tag either!")), printLogsToScreen, printLogsToOutputLog, FColor::Red, printLogsToScreen_Duration);

		passedChecks_Local = false;
	}


	if (!passedChecks_Local) {

		if (printLogsToScreen || printLogsToOutputLog)
			UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Runtime Vertex Color Paint and Detection Plugin - Check Valid Failed at Paint Color Snippet!")), printLogsToScreen, printLogsToOutputLog, FColor::Red, printLogsToScreen_Duration);

		UVertexPaintFunctionLibrary::RunPaintColorSnippetCallbacks(calculateColorsInfoTemp);
		return;
	}


	if (printLogsToScreen || printLogsToOutputLog)
		UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Runtime Vertex Color Paint and Detection Plugin - Paint Color Snippet CheckValid Successfull. ")), printLogsToScreen, printLogsToOutputLog, FColor::Green, printLogsToScreen_Duration);



	TMap<FGameplayTag, TSoftObjectPtr<UVertexPaintColorSnippetDataAsset>> availableColorSnippetTags_Local;
	UVertexPaintFunctionLibrary::GetAllMeshColorSnippetsAsTags_Wrapper(paintColorSnippetStruct.meshComponent, availableColorSnippetTags_Local);


	// If colorSnippetTag isn't set, or it's set but the Mesh doesn't have the Color Snippet Tag, and the randomizeAnyColorSnippetTagUnderThisCategory is set, then we can Randomize any available color snippet tag under the category, if there are any. 
	if (paintColorSnippetStruct.colorSnippetTag.IsValid() && availableColorSnippetTags_Local.Contains(paintColorSnippetStruct.colorSnippetTag)) {

		paintColorSnippetStruct.colorSnippetID = paintColorSnippetStruct.colorSnippetTag.GetTagName().ToString();
	}

	else if (paintColorSnippetStruct.randomizeAnyColorSnippetTagUnderThisCategory.IsValid()) {

		TMap<FGameplayTag, TSoftObjectPtr<UVertexPaintColorSnippetDataAsset>> availableColorSnippetTagsAndDataAssets;
		UVertexPaintFunctionLibrary::GetAllMeshColorSnippetsTagsUnderTagCategory_Wrapper(paintColorSnippetStruct.meshComponent, paintColorSnippetStruct.randomizeAnyColorSnippetTagUnderThisCategory, availableColorSnippetTagsAndDataAssets);

		TArray<FGameplayTag> meshTagsUnderCategory_Local;
		availableColorSnippetTagsAndDataAssets.GetKeys(meshTagsUnderCategory_Local);


		if (availableColorSnippetTagsAndDataAssets.Num() > 0) {

			paintColorSnippetStruct.colorSnippetID = meshTagsUnderCategory_Local[UKismetMathLibrary::RandomIntegerInRange(0, availableColorSnippetTagsAndDataAssets.Num() - 1)].GetTagName().ToString();

			// So even if randomizing, the colorSnippetToPaint Tag will match the randomized snippet ID. 
			paintColorSnippetStruct.colorSnippetTag = FGameplayTag::RequestGameplayTag(FName(*paintColorSnippetStruct.colorSnippetID));
		}

		else {

			if (printLogsToScreen || printLogsToOutputLog)
				UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Runtime Vertex Color Paint and Detection Plugin - Paint Color Snippet Task Failed as it was set to Randomize a Snippet under Category, but we couldn't find any Snippet Tag under the Category for the Mesh: %s"), *paintColorSnippetStruct.meshComponent->GetName()), printLogsToScreen, printLogsToOutputLog, FColor::Red, printLogsToScreen_Duration);

			UVertexPaintFunctionLibrary::RunPaintColorSnippetCallbacks(calculateColorsInfoTemp);

			return;
		}
	}


	bool successfullyStartedAsyncLoadRequest = false;

	// If Color Snippet ID has been set and matches the tag
	if (paintColorSnippetStruct.colorSnippetID == paintColorSnippetStruct.colorSnippetTag.GetTagName().ToString()) {

		// If valid soft ptr
		if (availableColorSnippetTags_Local.FindRef(paintColorSnippetStruct.colorSnippetTag).ToSoftObjectPath().ToString().Len() > 0) {


			TArray<int> currentColorSnippetRequestAsyncTaskIDs_Local;
			colorSnippetAsyncLoadHandleMap.GenerateKeyArray(currentColorSnippetRequestAsyncTaskIDs_Local);

			int randomColorSnippetRequestAsyncTaskID_Local = 0;

			do {
				randomColorSnippetRequestAsyncTaskID_Local = UKismetMathLibrary::RandomIntegerInRange(0, 214748364);
			} while (currentColorSnippetRequestAsyncTaskIDs_Local.Contains(randomColorSnippetRequestAsyncTaskID_Local)); // Randomized to get a unique task ID. 


			TAsyncLoadPriority asyncLoadPriority = FStreamableManager::AsyncLoadHighPriority;
			colorSnippetAsyncLoadHandleMap.Add(randomColorSnippetRequestAsyncTaskID_Local, UAssetManager::GetStreamableManager().RequestAsyncLoad(availableColorSnippetTags_Local.FindRef(paintColorSnippetStruct.colorSnippetTag).ToSoftObjectPath(), FStreamableDelegate::CreateUObject(this, &UVertexPaintDetectionComponent::PaintColorSnippetOnMesh_FinishedLoadingColorSnippetDataAsset, randomColorSnippetRequestAsyncTaskID_Local, paintColorSnippetStruct, additionalDataToPassThrough), asyncLoadPriority, false, false, "Async Loading Color Snippet Data Asset"));

			successfullyStartedAsyncLoadRequest = true;
		}

		else {

			if (printLogsToScreen || printLogsToOutputLog)
				UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Runtime Vertex Color Paint and Detection Plugin - Paint Color Snippet Fail as we the Snippet Found didn't point to a valid color snippet data asset soft pointer. ")), printLogsToScreen, printLogsToOutputLog, FColor::Red, printLogsToScreen_Duration);
		}
	}

	else {

		if (printLogsToScreen || printLogsToOutputLog)
			UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Runtime Vertex Color Paint and Detection Plugin - Paint Color Snippet Fail as the Color Snippet ID didn't get set correctly. The Tag you tried to Paint is probably not Registered to this Mesh. If you know it is, try saving and restarting the editor. ")), printLogsToScreen, printLogsToOutputLog, FColor::Red, printLogsToScreen_Duration);
	}


	if (!successfullyStartedAsyncLoadRequest) {

		if (printLogsToScreen || printLogsToOutputLog)
			UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Runtime Vertex Color Paint and Detection Plugin - Failed to Start Color Snippet Async Load Request and thus the Task!")), printLogsToScreen, printLogsToOutputLog, FColor::Red, printLogsToScreen_Duration);

		calculateColorsInfoTemp.paintOnColorSnippetSettings = paintColorSnippetStruct; // Since we set ID etc. we update this before returning

		UVertexPaintFunctionLibrary::RunPaintColorSnippetCallbacks(calculateColorsInfoTemp);
		return;
	}
}


//--------------------------------------------------------

// Paint Color Snippet on Mesh - Finished Loading Color Snippet Data Asset

void UVertexPaintDetectionComponent::PaintColorSnippetOnMesh_FinishedLoadingColorSnippetDataAsset(int finishedID, FVertexPaintColorSnippetStruct paintColorSnippetStruct, FVertexDetectAdditionalDataToPassThrough additionalDataToPassThrough) {


	FCalculateColorsInfo calculateColorsInfoTemp;
	calculateColorsInfoTemp.paintDetectionType = EVertexPaintDetectionType::PaintColorSnippet;
	calculateColorsInfoTemp.paintOnColorSnippetSettings = paintColorSnippetStruct;
	calculateColorsInfoTemp.initiatedByComponent = this;
	calculateColorsInfoTemp.additionalDataToPassThrough = additionalDataToPassThrough;
	calculateColorsInfoTemp.vertexPaintSettings = paintColorSnippetStruct;

	FillCalculateColorsInfoDebugSettings(calculateColorsInfoTemp);
	UVertexPaintFunctionLibrary::FillCalculateColorsInfoFundementals(paintColorSnippetStruct, calculateColorsInfoTemp);


	bool passedChecks_Local = true;

	if (!UVertexPaintFunctionLibrary::CheckValidFundementals(paintColorSnippetStruct.meshComponent, printLogsToScreen, printLogsToOutputLog, printLogsToScreen_Duration))
		passedChecks_Local = false;

	if (!UVertexPaintFunctionLibrary::CheckValidVertexPaintSettings(paintColorSnippetStruct, printLogsToScreen, printLogsToOutputLog, printLogsToScreen_Duration))
		passedChecks_Local = false;


	// If passed fundamental checks, then checks paint color snippet specific ones
	if (passedChecks_Local && !paintColorSnippetStruct.colorSnippetTag.IsValid() && !paintColorSnippetStruct.randomizeAnyColorSnippetTagUnderThisCategory.IsValid()) {

		if (printLogsToScreen || printLogsToOutputLog)
			UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Runtime Vertex Color Paint and Detection Plugin - PaintColorSnippet Failed after Finished Loading because no Snippet Tag was provided and it's not set to Randomize a Snippet Tag either!")), printLogsToScreen, printLogsToOutputLog, FColor::Red, printLogsToScreen_Duration);

		passedChecks_Local = false;
	}


	if (!passedChecks_Local) {

		if (printLogsToScreen || printLogsToOutputLog)
			UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Runtime Vertex Color Paint and Detection Plugin - Check Valid Failed at Paint Color Snippet after Finishing Loading Color Snippet Data Asset!")), printLogsToScreen, printLogsToOutputLog, FColor::Red, printLogsToScreen_Duration);

		UVertexPaintFunctionLibrary::RunPaintColorSnippetCallbacks(calculateColorsInfoTemp);
		return;
	}


	TSharedPtr<FStreamableHandle> finishedStreamableHandle_Local;

	TArray<int> currentColorSnippetRequestAsyncTaskIDs_Local;
	colorSnippetAsyncLoadHandleMap.GenerateKeyArray(currentColorSnippetRequestAsyncTaskIDs_Local);

	TArray<TSharedPtr<FStreamableHandle>> currentColorSnippetRequestAsyncTaskStreamableHandles_Local;
	colorSnippetAsyncLoadHandleMap.GenerateValueArray(currentColorSnippetRequestAsyncTaskStreamableHandles_Local);

	// Finds the streamable handle assocaited with the finished handle ID and then removes the ID from the TMap
	for (int i = 0; i < currentColorSnippetRequestAsyncTaskIDs_Local.Num(); i++) {

		if (currentColorSnippetRequestAsyncTaskIDs_Local[i] == finishedID) {

			finishedStreamableHandle_Local = currentColorSnippetRequestAsyncTaskStreamableHandles_Local[i];
			colorSnippetAsyncLoadHandleMap.Remove(currentColorSnippetRequestAsyncTaskIDs_Local[i]);

			break;
		}
	}

	if (finishedStreamableHandle_Local.Get()) {

		if (finishedStreamableHandle_Local.Get()->GetLoadedAsset()) {

			if (auto colorSnippetDataAsset_Local = Cast<UVertexPaintColorSnippetDataAsset>(finishedStreamableHandle_Local.Get()->GetLoadedAsset())) {

				if (colorSnippetDataAsset_Local->snippetColorData.Contains(paintColorSnippetStruct.colorSnippetID)) {

					if (colorSnippetDataAsset_Local->snippetColorData.FindRef(paintColorSnippetStruct.colorSnippetID).colorSnippetDataPerLOD.IsValidIndex(0)) {

						paintColorSnippetStruct.colorSnippetDataAssetSnippetIsStoredIn = colorSnippetDataAsset_Local;
					}

					else {

						if (printLogsToScreen || printLogsToOutputLog)
							UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Runtime Vertex Color Paint and Detection Plugin - PaintColorSnippet Fail! - The Color Snippet JSon Per LOD doesn't have a valid LOD0 index. ")), printLogsToScreen, printLogsToOutputLog, FColor::Red, printLogsToScreen_Duration);
					}
				}

				else {

					if (printLogsToScreen || printLogsToOutputLog)
						UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Runtime Vertex Color Paint and Detection Plugin - PaintColorSnippet Fail! - The Color Snippet Data Asset the Mesh have referenced to store their color snippet doesn't have the Snippet ID Registered! Try removing the snippet from the Editor Widget and adding it again.")), printLogsToScreen, printLogsToOutputLog, FColor::Red, printLogsToScreen_Duration);
				}
			}

			else {

				if (printLogsToScreen || printLogsToOutputLog)
					UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Runtime Vertex Color Paint and Detection Plugin - PaintColorSnippet Fail! - The Color Snippet Data Asset could not be loaded. If you have deleted it after storing snippets on it then recommend opening up the Editor Utility widget which will refresh and clear up if any mesh has references to old data assets ")), printLogsToScreen, printLogsToOutputLog, FColor::Red, printLogsToScreen_Duration);
			}
		}

		else {

			if (printLogsToScreen || printLogsToOutputLog)
				UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Runtime Vertex Color Paint and Detection Plugin - PaintColorSnippet Fail! - The Color Snippet Data Asset could not be loaded. If you have deleted it after storing snippets on it then recommend opening up the Editor Utility widget which will refresh and clear up if any mesh has references to old data assets ")), printLogsToScreen, printLogsToOutputLog, FColor::Red, printLogsToScreen_Duration);
		}
	}

	else {

		if (printLogsToScreen || printLogsToOutputLog)
			UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Runtime Vertex Color Paint and Detection Plugin - PaintColorSnippet Fail! - After Attempting to Load the Color Snippet Data Asset the FStreamableHandle wasn't Valid! ")), printLogsToScreen, printLogsToOutputLog, FColor::Red, printLogsToScreen_Duration);
	}


	// Updates this as we've updates the struct with results
	calculateColorsInfoTemp.paintOnColorSnippetSettings = paintColorSnippetStruct;


	if (!paintColorSnippetStruct.colorSnippetDataAssetSnippetIsStoredIn) {

		UVertexPaintFunctionLibrary::RunPaintColorSnippetCallbacks(calculateColorsInfoTemp);

		return;
	}


	if (printLogsToScreen || printLogsToOutputLog)
		UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Runtime Vertex Color Paint and Detection Plugin - Paint Color Snippet Load Color Snippet Data Asset Successfull. Trying to Apply Snippet: %s"), *paintColorSnippetStruct.colorSnippetID), printLogsToScreen, printLogsToOutputLog, FColor::Green, printLogsToScreen_Duration);


	vertexPaintGameInstanceSubsystem_Global->AddCalculateColorsTaskToQueue(calculateColorsInfoTemp);
}

void UVertexPaintDetectionComponent::PaintColorSnippetOnMeshTaskFinished() {

	if (amountOfPaintColorSnippetTasksCache > 0)
		amountOfPaintColorSnippetTasksCache--;
}


//-------------------------------------------------------

// Get Current Tasks Initiated By Component

void UVertexPaintDetectionComponent::GetCurrentTasksInitiatedByComponent(int& totalAmountOfTasks, int& amountOfGetClosestVertexDataTasks, int& amountOfGetAllVertexColorsOnlyTasks, int& amountOfPaintAtLocationTasks, int& amountOfPaintWithinAreaTasks, int& amountOfPaintEntireMeshTasks, int& amountOfPaintColorSnippetTasks) {

	totalAmountOfTasks = amountOfGetClosestVertexDataTasksCache + amountOfGetAllVertexColorsOnlyTasksCache + amountOfPaintAtLocationTasksCache + amountOfPaintWithinAreaTasksCache + amountOfPaintEntireMeshTasksCache + amountOfPaintColorSnippetTasksCache;

	amountOfGetClosestVertexDataTasks = amountOfGetClosestVertexDataTasksCache;
	amountOfGetAllVertexColorsOnlyTasks = amountOfGetAllVertexColorsOnlyTasksCache;
	amountOfPaintAtLocationTasks = amountOfPaintAtLocationTasksCache;
	amountOfPaintWithinAreaTasks = amountOfPaintWithinAreaTasksCache;
	amountOfPaintEntireMeshTasks = amountOfPaintEntireMeshTasksCache;
	amountOfPaintColorSnippetTasks = amountOfPaintColorSnippetTasksCache;
}


//-------------------------------------------------------

// Fill Debug Settings To Calculate Colors Info

void UVertexPaintDetectionComponent::FillCalculateColorsInfoDebugSettings(FCalculateColorsInfo& calculateColorsInfo) {

	calculateColorsInfo.printLogsToScreen = printLogsToScreen;
	calculateColorsInfo.printLogsToScreen_Duration = printLogsToScreen_Duration;
	calculateColorsInfo.printLogsToOutputLog = printLogsToOutputLog;
	calculateColorsInfo.drawGetClosestVertexDataDebugSymbols = drawGetClosestVertexDataDebugSymbols;
	calculateColorsInfo.drawGetClosestVertexDataDebugSymbolsDuration = drawGetClosestVertexDataDebugSymbolsDuration;
	calculateColorsInfo.drawGetEstimatedColorAtHitLocationDebugSymbols = drawGetEstimatedColorAtHitLocationDebugSymbols;
	calculateColorsInfo.drawGetEstimatedColorAtHitLocationDebugSymbolsDuration = drawGetEstimatedColorAtHitLocationDebugSymbolsDuration;
	calculateColorsInfo.drawVertexPositionDebugPoint = atGameThread_drawVertexPositionDebugPoint;
	calculateColorsInfo.drawVertexPositionDebugPointOnVertexThatGotPaintApplied = atGameThread_drawVertexPositionDebugPointOnVertexThatGotPaintApplied;
	calculateColorsInfo.drawClothVertexPositionDebugPoint = atGameThread_drawClothVertexPositionDebugPoint;
	calculateColorsInfo.drawVertexesDebugPointsDuration = atGameThread_DrawVertexPointsDuration;
	calculateColorsInfo.drawVertexNormalDebugArrow = atGameThread_drawVertexNormalDebugArrow;
	calculateColorsInfo.drawPaintConditionsDebugSymbols = atGameThread_DrawPaintConditionsDebugSymbols;
	calculateColorsInfo.drawPaintConditionsDebugSymbolsDuration = atGameThread_DrawPaintConditionsDebugSymbolsDuration;
	calculateColorsInfo.drawPaintWithinAreaDebugSymbols = drawPaintWithinAreaDebugSymbols;
	calculateColorsInfo.drawPaintWithinAreaDebugSymbolsDuration = drawPaintWithinAreaDebugSymbolsDuration;
}