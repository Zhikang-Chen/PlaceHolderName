// Copyright 2022 Alexander Floden, Alias Alex River. All Rights Reserved. 


#include "VertexPaintFunctionLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Engine/EngineTypes.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "PhysicsEngine/PhysicsSettings.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Materials/MaterialInstance.h"
#include "PhysicsEngine/BodySetup.h"
#include "Engine/GameInstance.h"

#include "VertexPaintDetectionInterface.h"
#include "VertexPaintDetectionComponent.h"
#include "VertexPaintDetectionGISubSystem.h"
#include "VertexPaintMaterialDataAsset.h"
#include "VertexPaintDetectionSettings.h"
#include "VertexPaintColorSnippetRefs.h"
#include "VertexPaintOptimizationDataAsset.h"

#include "Engine/StaticMesh.h"
#include "StaticMeshResources.h"
#include "Engine/SkeletalMesh.h"
#include "Components/SkeletalMeshComponent.h"
#include "Rendering/SkeletalMeshRenderData.h"
#include "Components/SplineMeshComponent.h"
#include "Components/BoxComponent.h"


#if ENGINE_MAJOR_VERSION == 5

#include "ChaosWheeledVehicleMovementComponent.h"
#include "ChaosCloth/ChaosClothingSimulation.h"
#include "ChaosCloth/ChaosClothingSimulationInteractor.h"
#include "ClothingAsset.h"

#if ENGINE_MINOR_VERSION >= 2
#include "StaticMeshComponentLODInfo.h"
#include "Engine/Level.h"
#endif

#endif


//--------------------------------------------------------

// Get Closest Vertex Data On Mesh Wrapper

void UVertexPaintFunctionLibrary::GetClosestVertexDataOnMesh_Wrapper(UVertexPaintDetectionComponent* runtimeVertexPaintAndDetectionComponent, UPrimitiveComponent* meshComponent, FVertexDetectInfoStruct getClosestVertexDataStruct, FVertexDetectAdditionalDataToPassThrough additionalDataToPassThrough) {

	if (IsValid(runtimeVertexPaintAndDetectionComponent)) {

		getClosestVertexDataStruct.meshComponent = meshComponent;

		// Note nothing more should be set here, because if a C++ Class calls the Paint/Detection Function Right Away it shouldn't lose out on anything being set

		runtimeVertexPaintAndDetectionComponent->GetClosestVertexDataOnMesh(getClosestVertexDataStruct, additionalDataToPassThrough);
	}

	else {

		UE_LOG(LogTemp, Warning, TEXT("Runtime Vertex Color Paint and Detection Plugin - Get Closest Vertex Data on Mesh Failed because the Paint Component sent in isn't valid! Perhaps the Actor or Component is being destroyed. "));
	}
}


//--------------------------------------------------------

// Get All Vertex Colors Only Wrapper

void UVertexPaintFunctionLibrary::GetAllVertexColorsOnly_Wrapper(UVertexPaintDetectionComponent* runtimeVertexPaintAndDetectionComponent, UPrimitiveComponent* meshComponent, FVertexDetectGetColorsOnlyStruct getAllVertexColorsStruct, FVertexDetectAdditionalDataToPassThrough additionalDataToPassThrough) {

	if (IsValid(runtimeVertexPaintAndDetectionComponent)) {

		getAllVertexColorsStruct.meshComponent = meshComponent;

		// Note nothing more should be set here, because if a C++ Class calls the Paint/Detection Function Right Away it shouldn't lose out on anything being set

		runtimeVertexPaintAndDetectionComponent->GetAllVertexColorsOnly(getAllVertexColorsStruct, additionalDataToPassThrough);
	}

	else {

		UE_LOG(LogTemp, Warning, TEXT("Runtime Vertex Color Paint and Detection Plugin - Get All Vertex Colors Failed because the Paint Component sent in isn't valid! Perhaps the Actor or Component is being destroyed. "));
	}
}


//--------------------------------------------------------

// Paint on Mesh at Location Wrapper

void UVertexPaintFunctionLibrary::PaintOnMeshAtLocation_Wrapper(UVertexPaintDetectionComponent* runtimeVertexPaintAndDetectionComponent, UPrimitiveComponent* meshComponent, FVertexPaintAtLocationStruct paintAtLocationStruct, FVertexDetectAdditionalDataToPassThrough additionalDataToPassThrough) {

	if (IsValid(runtimeVertexPaintAndDetectionComponent)) {

		paintAtLocationStruct.meshComponent = meshComponent;

		// Note nothing more should be set here, because if a C++ Class calls the Paint/Detection Function Right Away it shouldn't lose out on anything being set

		runtimeVertexPaintAndDetectionComponent->PaintOnMeshAtLocation(paintAtLocationStruct, additionalDataToPassThrough);
	}

	else {

		UE_LOG(LogTemp, Warning, TEXT("Runtime Vertex Color Paint and Detection Plugin - Paint at Location Failed because the Paint Component sent in isn't valid! Perhaps the Actor or Component is being destroyed. "));
	}
}


//--------------------------------------------------------

// Paint Within Area Wrapper

void UVertexPaintFunctionLibrary::PaintOnMeshWithinArea_Wrapper(UVertexPaintDetectionComponent* runtimeVertexPaintAndDetectionComponent, UPrimitiveComponent* meshComponent, TArray<FVertexPainthWithinArea_ComponentToCheckIfIsWithinInfo> componentsToCheckIfIsWithin, FVertexPainthWithinAreaStruct paintWithinAreaStruct, FVertexDetectAdditionalDataToPassThrough additionalDataToPassThrough) {

	if (IsValid(runtimeVertexPaintAndDetectionComponent)) {

		paintWithinAreaStruct.meshComponent = meshComponent;
		paintWithinAreaStruct.componentsToCheckIfIsWithin = componentsToCheckIfIsWithin;

		// Note nothing more should be set here, because if a C++ Class calls the Paint/Detection Function Right Away it shouldn't lose out on anything being set

		runtimeVertexPaintAndDetectionComponent->PaintOnMeshWithinArea(paintWithinAreaStruct, additionalDataToPassThrough);
	}

	else {

		UE_LOG(LogTemp, Warning, TEXT("Runtime Vertex Color Paint and Detection Plugin - Paint on Mesh Within Area Failed because the Paint Component sent in isn't valid! Perhaps the Actor or Component is being destroyed. "));
	}
}


//--------------------------------------------------------

// Paint on Entire Mesh Wrapper

void UVertexPaintFunctionLibrary::PaintOnEntireMesh_Wrapper(UVertexPaintDetectionComponent* runtimeVertexPaintAndDetectionComponent, UPrimitiveComponent* meshComponent, FVertexPaintOnEntireMeshStruct paintOnEntireMeshStruct, FVertexDetectAdditionalDataToPassThrough additionalDataToPassThrough) {

	if (IsValid(runtimeVertexPaintAndDetectionComponent)) {

		paintOnEntireMeshStruct.meshComponent = meshComponent;

		// Note nothing more should be set here, because if a C++ Class calls the Paint/Detection Function Right Away it shouldn't lose out on anything being set

		runtimeVertexPaintAndDetectionComponent->PaintOnEntireMesh(paintOnEntireMeshStruct, additionalDataToPassThrough);
	}

	else {

		UE_LOG(LogTemp, Warning, TEXT("Runtime Vertex Color Paint and Detection Plugin - Paint on Entire Mesh Failed because the Paint Component sent in isn't valid! Perhaps the Actor or Component is being destroyed. "));
	}
}


//--------------------------------------------------------

// Paint Color Snippet On Mesh

void UVertexPaintFunctionLibrary::PaintColorSnippetOnMesh_Wrappers(UVertexPaintDetectionComponent* runtimeVertexPaintAndDetectionComponent, UPrimitiveComponent* meshComponent, FVertexPaintColorSnippetStruct paintColorSnippetStruct, FVertexDetectAdditionalDataToPassThrough additionalDataToPassThrough) {

	if (IsValid(runtimeVertexPaintAndDetectionComponent)) {

		paintColorSnippetStruct.meshComponent = meshComponent;

		// Note nothing more should be set here, because if a C++ Class calls the Paint/Detection Function Right Away it shouldn't lose out on anything being set

		runtimeVertexPaintAndDetectionComponent->PaintColorSnippetOnMesh(paintColorSnippetStruct, additionalDataToPassThrough);
	}

	else {

		UE_LOG(LogTemp, Warning, TEXT("Runtime Vertex Color Paint and Detection Plugin - Paint Color Snippet on Mesh Failed because the Vertex Paint Component sent in isn't valid! Perhaps the Actor or Component is being destroyed, or it hasn't been properly initialized. "));
	}
}


//--------------------------------------------------------

// Set Mesh Component Vertex Colors

void UVertexPaintFunctionLibrary::SetMeshComponentVertexColors_Wrapper(UPrimitiveComponent* meshComponent, FVertexPaintSetMeshComponentVertexColors setMeshComponentVertexColorsSettings, FVertexDetectAdditionalDataToPassThrough additionalDataToPassThrough) {


	setMeshComponentVertexColorsSettings.meshComponent = meshComponent;

	if (IsValid(setMeshComponentVertexColorsSettings.meshComponent)) {

		setMeshComponentVertexColorsSettings.actor = setMeshComponentVertexColorsSettings.meshComponent->GetOwner();

		if (setMeshComponentVertexColorsSettings.printLogsToScreen || setMeshComponentVertexColorsSettings.printLogsToOutputLog)
			UKismetSystemLibrary::PrintString(setMeshComponentVertexColorsSettings.meshComponent->GetWorld(), FString::Printf(TEXT("Runtime Vertex Color Paint and Detection Plugin - Trying to Set Mesh Component Vertex Colors on Actor: %s and Component: %s"), *setMeshComponentVertexColorsSettings.meshComponent->GetOwner()->GetName(), *setMeshComponentVertexColorsSettings.meshComponent->GetName()), setMeshComponentVertexColorsSettings.printLogsToScreen, setMeshComponentVertexColorsSettings.printLogsToOutputLog, FColor::Cyan, setMeshComponentVertexColorsSettings.printLogsToScreen_Duration);
	}


	FCalculateColorsInfo calculateColorsInfoTemp;
	calculateColorsInfoTemp.paintDetectionType = EVertexPaintDetectionType::ApplyVertexDataDirectly;
	calculateColorsInfoTemp.setMeshComponentVertexColorsSettings = setMeshComponentVertexColorsSettings;
	calculateColorsInfoTemp.initiatedByComponent = setMeshComponentVertexColorsSettings.optionalCallbackComponent;
	calculateColorsInfoTemp.additionalDataToPassThrough = additionalDataToPassThrough;
	calculateColorsInfoTemp.vertexPaintSettings = setMeshComponentVertexColorsSettings;

	calculateColorsInfoTemp.printLogsToScreen = setMeshComponentVertexColorsSettings.printLogsToScreen;
	calculateColorsInfoTemp.printLogsToScreen_Duration = setMeshComponentVertexColorsSettings.printLogsToScreen_Duration;
	calculateColorsInfoTemp.printLogsToOutputLog = setMeshComponentVertexColorsSettings.printLogsToOutputLog;
	calculateColorsInfoTemp.meshVertexData.meshDataPerLOD.SetNum(1);
	calculateColorsInfoTemp.meshVertexData.meshDataPerLOD[0].meshVertexColorsPerLODArray = setMeshComponentVertexColorsSettings.vertexColorsAtLOD0ToSet;

	FillCalculateColorsInfoFundementals(setMeshComponentVertexColorsSettings, calculateColorsInfoTemp);


	bool passedChecks_Local = true;

	if (!UVertexPaintFunctionLibrary::CheckValidFundementals(setMeshComponentVertexColorsSettings.meshComponent, setMeshComponentVertexColorsSettings.printLogsToScreen, setMeshComponentVertexColorsSettings.printLogsToOutputLog, setMeshComponentVertexColorsSettings.printLogsToScreen_Duration))
		passedChecks_Local = false;

	if (passedChecks_Local && !UVertexPaintFunctionLibrary::CheckValidVertexPaintSettings(setMeshComponentVertexColorsSettings, setMeshComponentVertexColorsSettings.printLogsToScreen, setMeshComponentVertexColorsSettings.printLogsToOutputLog, setMeshComponentVertexColorsSettings.printLogsToScreen_Duration))
		passedChecks_Local = false;

	if (setMeshComponentVertexColorsSettings.meshComponent)
		if (!UGameplayStatics::GetGameInstance(setMeshComponentVertexColorsSettings.meshComponent->GetWorld())->GetSubsystem<UVertexPaintDetectionGISubSystem>())
			passedChecks_Local = false;


	// If passed fundamental checks, then checks set mesh component vertex colors specific ones
	if (setMeshComponentVertexColorsSettings.vertexColorsAtLOD0ToSet.Num() <= 0) {

		if (setMeshComponentVertexColorsSettings.printLogsToScreen || setMeshComponentVertexColorsSettings.printLogsToOutputLog) {

			if (setMeshComponentVertexColorsSettings.meshComponent)
				UKismetSystemLibrary::PrintString(setMeshComponentVertexColorsSettings.meshComponent->GetWorld(), FString::Printf(TEXT("Runtime Vertex Color Paint and Detection Plugin - Trying to Set Mesh Component Vertex Colors but the color array passed in are 0 in length!")), setMeshComponentVertexColorsSettings.printLogsToScreen, setMeshComponentVertexColorsSettings.printLogsToOutputLog, FColor::Red, setMeshComponentVertexColorsSettings.printLogsToScreen_Duration);
		}

		passedChecks_Local = false;
	}

	if (!passedChecks_Local) {

		if (setMeshComponentVertexColorsSettings.printLogsToScreen || setMeshComponentVertexColorsSettings.printLogsToOutputLog) {

			if (setMeshComponentVertexColorsSettings.meshComponent)
				UKismetSystemLibrary::PrintString(setMeshComponentVertexColorsSettings.meshComponent->GetWorld(), FString::Printf(TEXT("Runtime Vertex Color Paint and Detection Plugin - Check Valid Failed for Set Mesh Component Vertex Colors")), setMeshComponentVertexColorsSettings.printLogsToScreen, setMeshComponentVertexColorsSettings.printLogsToOutputLog, FColor::Red, setMeshComponentVertexColorsSettings.printLogsToScreen_Duration);
		}

		UVertexPaintFunctionLibrary::RunPaintSetMeshColorsCallbacks(calculateColorsInfoTemp);
		return;
	}


	// Removes any tasks here since we're gonna Set the Colors to be something so any queue before this is irrelevant
	UGameplayStatics::GetGameInstance(setMeshComponentVertexColorsSettings.meshComponent->GetWorld())->GetSubsystem<UVertexPaintDetectionGISubSystem>()->RemoveCalculateColorsMeshFromPaintTaskQueue(setMeshComponentVertexColorsSettings.meshComponent);

	UGameplayStatics::GetGameInstance(setMeshComponentVertexColorsSettings.meshComponent->GetWorld())->GetSubsystem<UVertexPaintDetectionGISubSystem>()->AddCalculateColorsTaskToQueue(calculateColorsInfoTemp);
}


//--------------------------------------------------------

// Set Mesh Component Vertex Colors Using Serialized String

void UVertexPaintFunctionLibrary::SetMeshComponentVertexColorsUsingSerializedString_Wrapper(UPrimitiveComponent* meshComponent, FVertexPaintSetMeshComponentVertexColorsUsingSerializedString setMeshComponentVertexColorsUsingSerializedStringSettings, FVertexDetectAdditionalDataToPassThrough additionalDataToPassThrough) {


	setMeshComponentVertexColorsUsingSerializedStringSettings.meshComponent = meshComponent;

	if (IsValid(setMeshComponentVertexColorsUsingSerializedStringSettings.meshComponent)) {

		setMeshComponentVertexColorsUsingSerializedStringSettings.actor = setMeshComponentVertexColorsUsingSerializedStringSettings.meshComponent->GetOwner();

		if (setMeshComponentVertexColorsUsingSerializedStringSettings.printLogsToScreen || setMeshComponentVertexColorsUsingSerializedStringSettings.printLogsToOutputLog)
			UKismetSystemLibrary::PrintString(setMeshComponentVertexColorsUsingSerializedStringSettings.meshComponent->GetWorld(), FString::Printf(TEXT("Runtime Vertex Color Paint and Detection Plugin - Trying to Set Mesh Component Vertex Colors Using Serialized String on Actor: %s and Component: %s"), *setMeshComponentVertexColorsUsingSerializedStringSettings.meshComponent->GetOwner()->GetName(), *setMeshComponentVertexColorsUsingSerializedStringSettings.meshComponent->GetName()), setMeshComponentVertexColorsUsingSerializedStringSettings.printLogsToScreen, setMeshComponentVertexColorsUsingSerializedStringSettings.printLogsToOutputLog, FColor::Cyan, setMeshComponentVertexColorsUsingSerializedStringSettings.printLogsToScreen_Duration);
	}


	FCalculateColorsInfo calculateColorsInfoTemp;
	calculateColorsInfoTemp.paintDetectionType = EVertexPaintDetectionType::ApplyVertexDataDirectly;
	calculateColorsInfoTemp.setMeshComponentVertexColorsUsingSerializedStringSettings = setMeshComponentVertexColorsUsingSerializedStringSettings;
	calculateColorsInfoTemp.initiatedByComponent = setMeshComponentVertexColorsUsingSerializedStringSettings.optionalCallbackComponent;
	calculateColorsInfoTemp.additionalDataToPassThrough = additionalDataToPassThrough;
	calculateColorsInfoTemp.vertexPaintSettings = setMeshComponentVertexColorsUsingSerializedStringSettings;

	calculateColorsInfoTemp.printLogsToScreen = setMeshComponentVertexColorsUsingSerializedStringSettings.printLogsToScreen;
	calculateColorsInfoTemp.printLogsToScreen_Duration = setMeshComponentVertexColorsUsingSerializedStringSettings.printLogsToScreen_Duration;
	calculateColorsInfoTemp.printLogsToOutputLog = setMeshComponentVertexColorsUsingSerializedStringSettings.printLogsToOutputLog;

	FillCalculateColorsInfoFundementals(setMeshComponentVertexColorsUsingSerializedStringSettings, calculateColorsInfoTemp);


	bool passedChecks_Local = true;

	if (!UVertexPaintFunctionLibrary::CheckValidFundementals(setMeshComponentVertexColorsUsingSerializedStringSettings.meshComponent, setMeshComponentVertexColorsUsingSerializedStringSettings.printLogsToScreen, setMeshComponentVertexColorsUsingSerializedStringSettings.printLogsToOutputLog, setMeshComponentVertexColorsUsingSerializedStringSettings.printLogsToScreen_Duration))
		passedChecks_Local = false;

	if (passedChecks_Local && !UVertexPaintFunctionLibrary::CheckValidVertexPaintSettings(setMeshComponentVertexColorsUsingSerializedStringSettings, setMeshComponentVertexColorsUsingSerializedStringSettings.printLogsToScreen, setMeshComponentVertexColorsUsingSerializedStringSettings.printLogsToOutputLog, setMeshComponentVertexColorsUsingSerializedStringSettings.printLogsToScreen_Duration))
		passedChecks_Local = false;

	if (setMeshComponentVertexColorsUsingSerializedStringSettings.meshComponent)
		if (!UGameplayStatics::GetGameInstance(setMeshComponentVertexColorsUsingSerializedStringSettings.meshComponent->GetWorld())->GetSubsystem<UVertexPaintDetectionGISubSystem>())
			passedChecks_Local = false;


	if (setMeshComponentVertexColorsUsingSerializedStringSettings.serializedColorDataAtLOD0.Len() <= 0) {

		if (setMeshComponentVertexColorsUsingSerializedStringSettings.printLogsToScreen || setMeshComponentVertexColorsUsingSerializedStringSettings.printLogsToOutputLog) {

			if (setMeshComponentVertexColorsUsingSerializedStringSettings.meshComponent)
				UKismetSystemLibrary::PrintString(setMeshComponentVertexColorsUsingSerializedStringSettings.meshComponent->GetWorld(), FString::Printf(TEXT("Runtime Vertex Color Paint and Detection Plugin - Trying to Set Mesh Component Vertex Colors Using Serialized String but the serializedColorData String passed in are 0 in length!")), setMeshComponentVertexColorsUsingSerializedStringSettings.printLogsToScreen, setMeshComponentVertexColorsUsingSerializedStringSettings.printLogsToOutputLog, FColor::Red, setMeshComponentVertexColorsUsingSerializedStringSettings.printLogsToScreen_Duration);
		}

		passedChecks_Local = false;
	}


	if (!passedChecks_Local) {

		if (setMeshComponentVertexColorsUsingSerializedStringSettings.printLogsToScreen || setMeshComponentVertexColorsUsingSerializedStringSettings.printLogsToOutputLog) {

			if (setMeshComponentVertexColorsUsingSerializedStringSettings.meshComponent)
				UKismetSystemLibrary::PrintString(setMeshComponentVertexColorsUsingSerializedStringSettings.meshComponent->GetWorld(), FString::Printf(TEXT("Runtime Vertex Color Paint and Detection Plugin - Check Valid Failed for Set Mesh Component Vertex Colors Using Serialized String")), setMeshComponentVertexColorsUsingSerializedStringSettings.printLogsToScreen, setMeshComponentVertexColorsUsingSerializedStringSettings.printLogsToOutputLog, FColor::Red, setMeshComponentVertexColorsUsingSerializedStringSettings.printLogsToScreen_Duration);
		}

		UVertexPaintFunctionLibrary::RunPaintSetMeshColorsCallbacks(calculateColorsInfoTemp);
		return;
	}


	// Removes any tasks here since we're gonna Set the Colors to be something so any queue before this is irrelevant
	UGameplayStatics::GetGameInstance(setMeshComponentVertexColorsUsingSerializedStringSettings.meshComponent->GetWorld())->GetSubsystem<UVertexPaintDetectionGISubSystem>()->RemoveCalculateColorsMeshFromPaintTaskQueue(setMeshComponentVertexColorsUsingSerializedStringSettings.meshComponent);

	UGameplayStatics::GetGameInstance(setMeshComponentVertexColorsUsingSerializedStringSettings.meshComponent->GetWorld())->GetSubsystem<UVertexPaintDetectionGISubSystem>()->AddCalculateColorsTaskToQueue(calculateColorsInfoTemp);
}


//--------------------------------------------------------

// Adjust Box Collision To Fit Area Between Paint At Locations

void UVertexPaintFunctionLibrary::AdjustBoxCollisionToFitAreaBetweenPaintAtLocations(UBoxComponent* boxCollision, FVector fromLocation, FVector toLocation, float paintAtLocationAreaOfAffect) {

	if (!IsValid(boxCollision)) return;

	float distanceBetweenLocations = (fromLocation - toLocation).Size();

	if (distanceBetweenLocations > paintAtLocationAreaOfAffect) {

		FVector middlePointBetweenLocations = (fromLocation + toLocation) / 2;
		FRotator rotationFromLocationToLocation = UKismetMathLibrary::GetDirectionUnitVector(fromLocation, toLocation).ToOrientationRotator();

		boxCollision->SetBoxExtent(FVector(distanceBetweenLocations / 2, paintAtLocationAreaOfAffect, paintAtLocationAreaOfAffect), true);

		FHitResult hitResultTemp;
		boxCollision->SetWorldLocationAndRotation(middlePointBetweenLocations, rotationFromLocationToLocation, false, &hitResultTemp, ETeleportType::None);
	}
}


//--------------------------------------------------------

// Is World Valid

bool UVertexPaintFunctionLibrary::IsWorldValid(UWorld* world) {

	if (!world) return false;
	if (!IsValid(world)) return false;
	if (!world->IsGameWorld()) return false;
	if (world->bIsTearingDown) return false;

	return true;
}


//-------------------------------------------------------

// Clamp Amount Of Painted Colors Of Each Channel

FVertexDetectIncludeAmountOfPaintedColorsOfEachChannel UVertexPaintFunctionLibrary::ClampAmountOfPaintedColorsOfEachChannel(FVertexDetectIncludeAmountOfPaintedColorsOfEachChannel amountOfColorsOfEachChannel) {

	amountOfColorsOfEachChannel.includeAmountOfPaintedColorsOfEachChannel_IfMinColorAmount = UKismetMathLibrary::FClamp(amountOfColorsOfEachChannel.includeAmountOfPaintedColorsOfEachChannel_IfMinColorAmount, 0, 1);

	return amountOfColorsOfEachChannel;
}


//-------------------------------------------------------

// Clamp Paint Limits

FVertexPaintingLimitStruct UVertexPaintFunctionLibrary::ClampPaintLimits(FVertexPaintingLimitStruct paintLimits) {

	paintLimits.paintLimitOnRedChannel = UKismetMathLibrary::FClamp(paintLimits.paintLimitOnRedChannel, 0, 1);
	paintLimits.paintLimitOnGreenChannel = UKismetMathLibrary::FClamp(paintLimits.paintLimitOnGreenChannel, 0, 1);
	paintLimits.paintLimitOnBlueChannel = UKismetMathLibrary::FClamp(paintLimits.paintLimitOnBlueChannel, 0, 1);
	paintLimits.paintLimitOnAlphaChannel = UKismetMathLibrary::FClamp(paintLimits.paintLimitOnAlphaChannel, 0, 1);

	return paintLimits;
}


//--------------------------------------------------------

// Get Mesh Painted Since Session Started

TMap<UPrimitiveComponent*, int> UVertexPaintFunctionLibrary::GetMeshPaintedSinceSessionStarted_Wrapper(const UObject* WorldContextObject) {

	if (IsValid(WorldContextObject)) {

		if (UGameplayStatics::GetGameInstance(WorldContextObject)->GetSubsystem<UVertexPaintDetectionGISubSystem>())
			return UGameplayStatics::GetGameInstance(WorldContextObject)->GetSubsystem<UVertexPaintDetectionGISubSystem>()->GetMeshPaintedSinceSessionStarted();
	}

	return TMap<UPrimitiveComponent*, int>();
}


//--------------------------------------------------------

// Clear Mesh Painted Since Session Started

void UVertexPaintFunctionLibrary::ClearMeshPaintedSinceSessionStarted_Wrapper(const UObject* WorldContextObject) {

	if (IsValid(WorldContextObject)) {

		if (UGameplayStatics::GetGameInstance(WorldContextObject)->GetSubsystem<UVertexPaintDetectionGISubSystem>())
			UGameplayStatics::GetGameInstance(WorldContextObject)->GetSubsystem<UVertexPaintDetectionGISubSystem>()->ClearMeshPaintedSinceSessionStarted();
	}
}


//-------------------------------------------------------

// Get Calculate Colors Paint Task Amount

TMap<UPrimitiveComponent*, int> UVertexPaintFunctionLibrary::GetCalculateColorsPaintTasksAmount_Wrapper(const UObject* WorldContextObject) {

	if (IsValid(WorldContextObject)) {

		if (UGameplayStatics::GetGameInstance(WorldContextObject)->GetSubsystem<UVertexPaintDetectionGISubSystem>())
			return UGameplayStatics::GetGameInstance(WorldContextObject)->GetSubsystem<UVertexPaintDetectionGISubSystem>()->GetCalculateColorsPaintTasksAmount();
	}

	return TMap<UPrimitiveComponent*, int>();
}


//-------------------------------------------------------

// Get Calculate Colors Detection Tasks

TMap<UPrimitiveComponent*, FVertexPaintCalculateColorsQueueArrayStruct> UVertexPaintFunctionLibrary::GetCalculateColorsPaintTasks_Wrapper(const UObject* WorldContextObject) {

	if (IsValid(WorldContextObject)) {

		if (UGameplayStatics::GetGameInstance(WorldContextObject)->GetSubsystem<UVertexPaintDetectionGISubSystem>())
			return UGameplayStatics::GetGameInstance(WorldContextObject)->GetSubsystem<UVertexPaintDetectionGISubSystem>()->GetCalculateColorsPaintTasks();
	}

	return TMap<UPrimitiveComponent*, FVertexPaintCalculateColorsQueueArrayStruct>();
}


//-------------------------------------------------------

// Get Calculate Colors Detection Tasks

TMap<UPrimitiveComponent*, FVertexPaintCalculateColorsQueueArrayStruct> UVertexPaintFunctionLibrary::GetCalculateColorsDetectionTasks_Wrapper(const UObject* WorldContextObject) {

	if (IsValid(WorldContextObject)) {

		if (UGameplayStatics::GetGameInstance(WorldContextObject)->GetSubsystem<UVertexPaintDetectionGISubSystem>())
			return UGameplayStatics::GetGameInstance(WorldContextObject)->GetSubsystem<UVertexPaintDetectionGISubSystem>()->GetCalculateColorsDetectionTasks();
	}

	return TMap<UPrimitiveComponent*, FVertexPaintCalculateColorsQueueArrayStruct>();
}


//-------------------------------------------------------

// Get Calculate Colors Detection Task Amount

TMap<UPrimitiveComponent*, int> UVertexPaintFunctionLibrary::GetCalculateColorsDetectionTasksAmount_Wrapper(const UObject* WorldContextObject) {

	if (IsValid(WorldContextObject)) {

		if (UGameplayStatics::GetGameInstance(WorldContextObject)->GetSubsystem<UVertexPaintDetectionGISubSystem>())
			return UGameplayStatics::GetGameInstance(WorldContextObject)->GetSubsystem<UVertexPaintDetectionGISubSystem>()->GetCalculateColorsDetectionTasksAmount();
	}

	return TMap<UPrimitiveComponent*, int>();
}


//--------------------------------------------------------

// Get All Available Color Snippets on Mesh

void UVertexPaintFunctionLibrary::GetAllMeshColorSnippetsAsString_Wrapper(UPrimitiveComponent* meshComponent, TMap<FString, TSoftObjectPtr<UVertexPaintColorSnippetDataAsset>>& availableColorSnippetsAndDataAssets) {

	availableColorSnippetsAndDataAssets.Empty();

	if (!IsValid(meshComponent)) return;

	if (!GetColorSnippetReferenceDataAsset()) {

		UE_LOG(LogTemp, Warning, TEXT("Runtime Vertex Color Paint and Detection Plugin - GetAllAvailableColorSnippetsOnMesh Fail because there's no Color Snippet Reference Data Asset Set in Settings so we can't get color snippet data assets a mesh has!"));

		return;
	}


	TMap<FString, TSoftObjectPtr<UVertexPaintColorSnippetDataAsset>> colorSnippets_Local;

	if (auto staticMeshComp_Local = Cast<UStaticMeshComponent>(meshComponent)) {

		colorSnippets_Local = GetColorSnippetReferenceDataAsset()->GetAllColorSnippetsAndDataAssetForObject(staticMeshComp_Local->GetStaticMesh());
	}

	else if (auto skeletalMeshComponent_Local = Cast<USkeletalMeshComponent>(meshComponent)) {

		UObject* meshAsset_Local = nullptr;

#if ENGINE_MAJOR_VERSION == 4

		meshAsset_Local = skeletalMeshComponent_Local->SkeletalMesh;

#elif ENGINE_MAJOR_VERSION == 5

#if ENGINE_MINOR_VERSION == 0

		meshAsset_Local = skeletalMeshComponent_Local->SkeletalMesh.Get();

#else

		meshAsset_Local = skeletalMeshComponent_Local->GetSkeletalMeshAsset();

#endif
#endif

		colorSnippets_Local = GetColorSnippetReferenceDataAsset()->GetAllColorSnippetsAndDataAssetForObject(meshAsset_Local);
	}

	// If is stored on a data asset
	if (colorSnippets_Local.Num() > 0)
		availableColorSnippetsAndDataAssets = colorSnippets_Local;
}


//--------------------------------------------------------

// Get All Available Color Snippets Tags on Mesh

void UVertexPaintFunctionLibrary::GetAllMeshColorSnippetsAsTags_Wrapper(UPrimitiveComponent* meshComponent, TMap<FGameplayTag, TSoftObjectPtr<UVertexPaintColorSnippetDataAsset>>& availableColorSnippetTagsAndDataAssets) {

	availableColorSnippetTagsAndDataAssets.Empty();

	if (!IsValid(meshComponent) || !GetColorSnippetReferenceDataAsset()) return;


	TMap<FString, TSoftObjectPtr<UVertexPaintColorSnippetDataAsset>> availableColorSnippetsAndDataAssets;
	GetAllMeshColorSnippetsAsString_Wrapper(meshComponent, availableColorSnippetsAndDataAssets);


	if (availableColorSnippetsAndDataAssets.Num() > 0) {

		TArray<FString> availableColorSnippets_Local;
		availableColorSnippetsAndDataAssets.GetKeys(availableColorSnippets_Local);

		TArray<TSoftObjectPtr<UVertexPaintColorSnippetDataAsset>> colorSnippetDataAssets_Local;
		availableColorSnippetsAndDataAssets.GenerateValueArray(colorSnippetDataAssets_Local);


		FGameplayTag topLevelTag = FGameplayTag::RequestGameplayTag(FName("ColorSnippets"));
		FGameplayTagContainer tagContainerTemp;
		tagContainerTemp.AddTag(FGameplayTag::RequestGameplayTag(topLevelTag.GetTagName(), false));
		FGameplayTagContainer tagContainer_Local = GetColorSnippetReferenceDataAsset()->allAvailableColorSnippets.Filter(tagContainerTemp);


		TArray<FGameplayTag> allAvailableTagsUnderCategory_Local;
		tagContainer_Local.GetGameplayTagArray(allAvailableTagsUnderCategory_Local);


		for (auto tag : allAvailableTagsUnderCategory_Local) {

			// If Mesh has Snippet ID that matches Tag under category 
			if (availableColorSnippets_Local.Contains(tag.GetTagName().ToString())) {

				int arrayIndex = -1;

				availableColorSnippets_Local.Find(tag.GetTagName().ToString(), arrayIndex);

				if (colorSnippetDataAssets_Local.IsValidIndex(arrayIndex))
					availableColorSnippetTagsAndDataAssets.Add(tag, colorSnippetDataAssets_Local[arrayIndex]);
			}
		}
	}
}


//--------------------------------------------------------

// Get All Mesh Color Snippets Tags Under Tag Category

void UVertexPaintFunctionLibrary::GetAllMeshColorSnippetsTagsUnderTagCategory_Wrapper(UPrimitiveComponent* meshComponent, FGameplayTag tagCategory, TMap<FGameplayTag, TSoftObjectPtr<UVertexPaintColorSnippetDataAsset>>& availableColorSnippetTagsAndDataAssetsUnderTagCategory) {

	availableColorSnippetTagsAndDataAssetsUnderTagCategory.Empty();

	if (!IsValid(meshComponent) || !GetColorSnippetReferenceDataAsset()) return;


	TMap<FGameplayTag, TSoftObjectPtr<UVertexPaintColorSnippetDataAsset>> availableColorSnippetTagsAndDataAssets;
	GetAllMeshColorSnippetsAsTags_Wrapper(meshComponent, availableColorSnippetTagsAndDataAssets);

	TArray<FGameplayTag> availableColorSnippetTags_Local;
	availableColorSnippetTagsAndDataAssets.GetKeys(availableColorSnippetTags_Local);

	TArray<TSoftObjectPtr<UVertexPaintColorSnippetDataAsset>> colorSnippetDataAssets_Local;
	availableColorSnippetTagsAndDataAssets.GenerateValueArray(colorSnippetDataAssets_Local);


	FGameplayTagContainer tagContainerTemp;
	tagContainerTemp.AddTag(FGameplayTag::RequestGameplayTag(tagCategory.GetTagName(), false));
	FGameplayTagContainer tagContainer_Local = GetColorSnippetReferenceDataAsset()->allAvailableColorSnippets.Filter(tagContainerTemp);

	TArray<FGameplayTag> allAvailableTagsUnderCategory_Local;
	tagContainer_Local.GetGameplayTagArray(allAvailableTagsUnderCategory_Local);


	for (auto tag : allAvailableTagsUnderCategory_Local) {

		// If Mesh has Snippet Tag that matches Tag under category 
		if (availableColorSnippetTags_Local.Contains(tag)) {

			int arrayIndex = -1;
			availableColorSnippetTags_Local.Find(tag, arrayIndex);

			if (colorSnippetDataAssets_Local.IsValidIndex(arrayIndex))
				availableColorSnippetTagsAndDataAssetsUnderTagCategory.Add(tag, colorSnippetDataAssets_Local[arrayIndex]);
		}
	}
}


//--------------------------------------------------------

// Get Mesh Color Snippets Tags In Tag Category

void UVertexPaintFunctionLibrary::GetMeshColorSnippetsTagsInTagCategory_Wrapper(UPrimitiveComponent* meshComponent, FGameplayTag tagCategory, TMap<FGameplayTag, TSoftObjectPtr<UVertexPaintColorSnippetDataAsset>>& availableColorSnippetTagsAndDataAssetsUnderTagCategory) {

	availableColorSnippetTagsAndDataAssetsUnderTagCategory.Empty();

	if (!IsValid(meshComponent) || !GetColorSnippetReferenceDataAsset()) return;


	TMap<FGameplayTag, TSoftObjectPtr<UVertexPaintColorSnippetDataAsset>> availableColorSnippetTagsAndDataAssets;
	GetAllMeshColorSnippetsAsTags_Wrapper(meshComponent, availableColorSnippetTagsAndDataAssets);

	TArray<FGameplayTag> availableColorSnippetTags_Local;
	availableColorSnippetTagsAndDataAssets.GetKeys(availableColorSnippetTags_Local);

	TArray<TSoftObjectPtr<UVertexPaintColorSnippetDataAsset>> colorSnippetDataAssets_Local;
	availableColorSnippetTagsAndDataAssets.GenerateValueArray(colorSnippetDataAssets_Local);


	FGameplayTagContainer tagContainerTemp;
	tagContainerTemp.AddTag(FGameplayTag::RequestGameplayTag(tagCategory.GetTagName(), false));

	FGameplayTagContainer tagContainer_Local = GetColorSnippetReferenceDataAsset()->allAvailableColorSnippets.Filter(tagContainerTemp);
	TArray<FGameplayTag> allAvailableTagsUnderCategory_Local;
	tagContainer_Local.GetGameplayTagArray(allAvailableTagsUnderCategory_Local);


	for (auto tag : allAvailableTagsUnderCategory_Local) {

		FString tagAsString = tag.GetTagName().ToString();

		// Plus 1 so we don't have the . after the parent category
		FString tagStringWithoutParentCategory = tagAsString.RightChop(tagCategory.GetTagName().ToString().Len() + 1);

		int charIndexWithPeriod = -1;

		// If has a ., i.e. this is not tag that is directly in the tag category but another under that. 
		if (tagStringWithoutParentCategory.FindChar('.', charIndexWithPeriod))
			continue;


		// If Mesh has Snippet Tag that matches Tag under category 
		if (availableColorSnippetTags_Local.Contains(tag)) {

			int arrayIndex = -1;
			availableColorSnippetTags_Local.Find(tag, arrayIndex);

			if (colorSnippetDataAssets_Local.IsValidIndex(arrayIndex))
				availableColorSnippetTagsAndDataAssetsUnderTagCategory.Add(tag, colorSnippetDataAssets_Local[arrayIndex]);
		}
	}
}


//--------------------------------------------------------

// Get Color Snippet Reference Data Asset

UVertexPaintColorSnippetRefs* UVertexPaintFunctionLibrary::GetColorSnippetReferenceDataAsset() {

	if (auto vertexPaintDetectionSettings_Local = GetDefault<UVertexPaintDetectionSettings>()) {

		if (vertexPaintDetectionSettings_Local->vertexPaintAndDetectionPlugin_ColorSnippetReferencesDataAssetToUse.IsValid())
			return vertexPaintDetectionSettings_Local->vertexPaintAndDetectionPlugin_ColorSnippetReferencesDataAssetToUse.Get();

		if (IsInGameThread())
			return vertexPaintDetectionSettings_Local->vertexPaintAndDetectionPlugin_ColorSnippetReferencesDataAssetToUse.LoadSynchronous();
	}

	return nullptr;
}


//--------------------------------------------------------

// Get Optimization Data Asset

UVertexPaintOptimizationDataAsset* UVertexPaintFunctionLibrary::GetOptimizationDataAsset() {

	if (auto vertexPaintDetectionSettings_Local = GetDefault<UVertexPaintDetectionSettings>()) {

		if (vertexPaintDetectionSettings_Local->vertexPaintAndDetectionPlugin_OptimizationDataAssetToUse.IsValid())
			return vertexPaintDetectionSettings_Local->vertexPaintAndDetectionPlugin_OptimizationDataAssetToUse.Get();

		if (IsInGameThread())
			return vertexPaintDetectionSettings_Local->vertexPaintAndDetectionPlugin_OptimizationDataAssetToUse.LoadSynchronous();
	}

	return nullptr;
}


//--------------------------------------------------------

// Get Vertex Paint Material Data Asset

UVertexPaintMaterialDataAsset* UVertexPaintFunctionLibrary::GetVertexPaintMaterialDataAsset() {

	if (auto vertexPaintDetectionSettings_Local = GetDefault<UVertexPaintDetectionSettings>()) {

		if (vertexPaintDetectionSettings_Local->vertexPaintAndDetectionPlugin_MaterialsDataAssetToUse.IsValid())
			return vertexPaintDetectionSettings_Local->vertexPaintAndDetectionPlugin_MaterialsDataAssetToUse.Get();

		if (IsInGameThread())
			return vertexPaintDetectionSettings_Local->vertexPaintAndDetectionPlugin_MaterialsDataAssetToUse.LoadSynchronous();
	}

	return nullptr;
}


//-------------------------------------------------------

// Get Max LOD To Paint On

int UVertexPaintFunctionLibrary::GetAmountOfLODsToPaintOn(UPrimitiveComponent* meshComp, bool overrideLODToPaintUpOn, int overrideUpToLOD) {

	if (!IsValid(meshComp)) return 1;


	int lodsToPaint_Local = 1;
	int maxLODsAvailable_Local = 1;


	if (auto staticMesh_Local = Cast<UStaticMeshComponent>(meshComp)) {

		if (!staticMesh_Local->GetStaticMesh()) return 1;
		if (!staticMesh_Local->GetStaticMesh()->GetRenderData()) return 1;

		maxLODsAvailable_Local = staticMesh_Local->GetStaticMesh()->GetRenderData()->LODResources.Num();

		lodsToPaint_Local = maxLODsAvailable_Local;


		if (GetOptimizationDataAsset()) {

			// If the mesh is added to only be painted a certain number of LODs in the Optimization Data Asset 
			if (GetOptimizationDataAsset()->staticMeshNumOfLODsToPaint.Contains(staticMesh_Local->GetStaticMesh())) {

				int LODsLimitToPaint_Local = GetOptimizationDataAsset()->staticMeshNumOfLODsToPaint.FindRef(staticMesh_Local->GetStaticMesh()).maxAmountOfLODsToPaint;


				if (LODsLimitToPaint_Local <= maxLODsAvailable_Local)
					lodsToPaint_Local = LODsLimitToPaint_Local;
			}
		}


		if (overrideLODToPaintUpOn) {

			if (maxLODsAvailable_Local >= overrideUpToLOD) {

				return overrideUpToLOD;
			}

			else {

				// If lodsToPaint_Local was less than override LOD, then just returns the maximum LODs we have so we get as close to the Override LOD 
				return maxLODsAvailable_Local;
			}
		}

		return lodsToPaint_Local;
	}

	else if (auto skeletalMeshComp_Local = Cast<USkeletalMeshComponent>(meshComp)) {


		USkeletalMesh* skelMesh_Local = nullptr;

#if ENGINE_MAJOR_VERSION == 4

		skelMesh_Local = skeletalMeshComp_Local->SkeletalMesh;

#elif ENGINE_MAJOR_VERSION == 5

#if ENGINE_MINOR_VERSION == 0

		skelMesh_Local = skeletalMeshComp_Local->SkeletalMesh.Get();

#else

		skelMesh_Local = skeletalMeshComp_Local->GetSkeletalMeshAsset();

#endif
#endif

		if (!skelMesh_Local) return 1;
		if (!skeletalMeshComp_Local->GetSkeletalMeshRenderData()) return 1;


		maxLODsAvailable_Local = skeletalMeshComp_Local->GetSkeletalMeshRenderData()->LODRenderData.Num();

		lodsToPaint_Local = maxLODsAvailable_Local;


		if (GetOptimizationDataAsset()) {

			if (GetOptimizationDataAsset()->skeletalMeshNumOfLODsToPaint.Contains(skelMesh_Local)) {

				int LODsLimitToPaint_Local = GetOptimizationDataAsset()->skeletalMeshNumOfLODsToPaint.FindRef(skelMesh_Local).maxAmountOfLODsToPaint;

				if (LODsLimitToPaint_Local <= maxLODsAvailable_Local)
					lodsToPaint_Local = LODsLimitToPaint_Local;
			}
		}


		if (overrideLODToPaintUpOn) {

			if (maxLODsAvailable_Local >= overrideUpToLOD) {

				return overrideUpToLOD;
			}

			else {

				return maxLODsAvailable_Local;
			}
		}

		return lodsToPaint_Local;
	}

	return 1;
}


//--------------------------------------------------------

// Multisphere Trace For Paint At Location

void UVertexPaintFunctionLibrary::MultiCapsuleTraceForClosestUniqueMeshes_Wrapper(const UObject* WorldContextObject, FVector location, float radius, ETraceTypeQuery traceChannel, const TArray<TEnumAsByte<EObjectTypeQuery>>& objectTypesToTraceFor, const TArray<AActor*>& actorsToIgnore, EDrawDebugTrace::Type drawDebugType, TArray<FHitResult>& closestHitsWithUniqueMeshes, float debugDrawTime, bool ignoreSelf, bool traceComplex) {

	closestHitsWithUniqueMeshes.Empty();

	if (!WorldContextObject) return;
	if (!WorldContextObject->GetWorld()) return;
	if (radius <= 0) return;


	// For UE4 it doesn't work to get the face index even if traceComplex is set to True, so turns it off here. 
#if ENGINE_MAJOR_VERSION == 4
	traceComplex = false;
#endif


	TArray<FHitResult> hitResults_Local;

	// Capsule Trace either with objects or visibility so we could get the Face Index. Wasn't posible with Sphere Trace. If we've set object types then traces with that. 
	if (objectTypesToTraceFor.Num() > 0) {

		UKismetSystemLibrary::CapsuleTraceMultiForObjects(WorldContextObject->GetWorld(), location, location * 1.00001f, radius, radius, objectTypesToTraceFor, traceComplex, actorsToIgnore, drawDebugType, hitResults_Local, ignoreSelf, FLinearColor::Red, FLinearColor::Green, debugDrawTime);
	}
	else {

		UKismetSystemLibrary::CapsuleTraceMulti(WorldContextObject->GetWorld(), location, location * 1.00001f, radius, radius, traceChannel, traceComplex, actorsToIgnore, drawDebugType, hitResults_Local, ignoreSelf, FLinearColor::Red, FLinearColor::Green, debugDrawTime);
	}


	if (hitResults_Local.Num() > 0) {

		TMap<UPrimitiveComponent*, FHitResult> hitResultWithUniqueMeshes_Local;
		TMap<UPrimitiveComponent*, float> closestDistances_Local;


		// Makes sure we only get one from each mesh and the closest hit
		for (int i = 0; i < hitResults_Local.Num(); i++) {

			if (IsValid(hitResults_Local[i].Component.Get())) {

				float distance_Local = (hitResults_Local[i].Location - location).Size();


				if (hitResultWithUniqueMeshes_Local.Contains(hitResults_Local[i].Component.Get())) {

					// If mesh already been added, but this hit is closer, then updates it
					if (distance_Local < closestDistances_Local.FindRef(hitResults_Local[i].Component.Get())) {

						closestDistances_Local.Add(hitResults_Local[i].Component.Get(), distance_Local);
						hitResultWithUniqueMeshes_Local.Add(hitResults_Local[i].Component.Get(), hitResults_Local[i]);
					}
				}

				else {

					closestDistances_Local.Add(hitResults_Local[i].Component.Get(), distance_Local);
					hitResultWithUniqueMeshes_Local.Add(hitResults_Local[i].Component.Get(), hitResults_Local[i]);
				}
			}
		}

		hitResultWithUniqueMeshes_Local.GenerateValueArray(closestHitsWithUniqueMeshes);
	}
}


//--------------------------------------------------------

// Set Static Mesh And Release Override Vertex Colors

void UVertexPaintFunctionLibrary::VertexPaintDetectionPlugin_SetStaticMeshAndReleaseOverrideVertexColors(UStaticMeshComponent* staticMeshComponent, UStaticMesh* newMesh) {

	if (!IsValid(staticMeshComponent) || !IsValid(newMesh)) return;


	// Necessary otherwise it had the same amount of LODs as the previous static mesh
	staticMeshComponent->SetLODDataCount(newMesh->GetNumLODs(), newMesh->GetNumLODs());

	if (staticMeshComponent->LODData.Num() > 0) {

		for (int i = 0; i < staticMeshComponent->LODData.Num(); i++) {


			int amountOfVertsToSwitchTo = 0;
			TArray<FColor> colorLOD_Local;

			if (newMesh->GetRenderData()->LODResources.IsValidIndex(i)) {

				amountOfVertsToSwitchTo = newMesh->GetRenderData()->LODResources[i].GetNumVertices();
				auto colVertBufferAtLOD_Local = &newMesh->GetRenderData()->LODResources[i].VertexBuffers.ColorVertexBuffer;

				// If color buffer isn't initialized it means its default colors are White and it hasn't been painted either in editor or in runtime, if this is the case we init the array with white so even unstored, unpainted cpu meshes with all default white vertex colors can be painted and look as they should. 
				if (colVertBufferAtLOD_Local) {

					if (colVertBufferAtLOD_Local->IsInitialized())
						colorLOD_Local.Init(FColor(0, 0, 0, 0), amountOfVertsToSwitchTo);
					else
						colorLOD_Local.Init(FColor::White, amountOfVertsToSwitchTo);
				}

				else {

					colorLOD_Local.Init(FColor::White, amountOfVertsToSwitchTo);
				}
			}

			// Had to make a new color vertex buffer and init it, otherwise when switching mesh, the new mesh could get really weird colors, as if the old buffer "bleed" to the next mesh but where the vertex color didn't match since it's a new mesh with different vertex amount etc. 
			staticMeshComponent->LODData[i].OverrideVertexColors = new FColorVertexBuffer();
			staticMeshComponent->LODData[i].OverrideVertexColors->InitFromColorArray(colorLOD_Local);
			BeginInitResource(staticMeshComponent->LODData[i].OverrideVertexColors);
		}
	}

	staticMeshComponent->SetStaticMesh(newMesh);

	staticMeshComponent->MarkRenderStateDirty();


}


//--------------------------------------------------------

// Release Resources And Set Skeletal Mesh

void UVertexPaintFunctionLibrary::VertexPaintDetectionPlugin_SetSkeletalMeshAndReleaseResources(USkeletalMeshComponent* skeletalMeshComponent, USkeletalMesh* newMesh) {

	if (!IsValid(skeletalMeshComponent) || !IsValid(newMesh)) return;


	USkeletalMesh* skelMeshTemp = nullptr;

#if ENGINE_MAJOR_VERSION == 4

	skelMeshTemp = skeletalMeshComponent->SkeletalMesh;

#elif ENGINE_MAJOR_VERSION == 5

#if ENGINE_MINOR_VERSION == 0

	skelMeshTemp = skeletalMeshComponent->SkeletalMesh.Get();

#else

	skelMeshTemp = skeletalMeshComponent->GetSkeletalMeshAsset();

#endif

#endif


	skeletalMeshComponent->ReleaseResources(); // Fixes so we don't get crash if we're applying colors to the skeletal mesh and switching


	// When i tested painting every single frame and switching characters like crazy i once still got a crash when running SetSkeletalMesh(), with trying to delete vertex color buffer before releasing it or something. So added all these to really make sure everything gets released before we switch


	skeletalMeshComponent->ReleaseAllClothingResources();

	for (int i = 0; i < skeletalMeshComponent->LODInfo.Num(); i++) {

		skeletalMeshComponent->ClearVertexColorOverride(i);
		skeletalMeshComponent->LODInfo[i].BeginReleaseOverrideVertexColors();
		skeletalMeshComponent->LODInfo[i].ReleaseOverrideVertexColorsAndBlock();
	}

	if (skelMeshTemp)
		skelMeshTemp->ReleaseResourcesFence.Wait();

	//----------

#if ENGINE_MAJOR_VERSION == 4

	skeletalMeshComponent->SetSkeletalMesh(newMesh, false);

#elif ENGINE_MAJOR_VERSION == 5

#if ENGINE_MINOR_VERSION == 0

	skeletalMeshComponent->SetSkeletalMesh(newMesh, false);

#else

	skeletalMeshComponent->SetSkeletalMeshAsset(newMesh);

#endif

#endif
}


//--------------------------------------------------------

// Sort String Array Alphabetically

TArray<FString> UVertexPaintFunctionLibrary::VertexPaintDetectionPlugin_SortStringArrayAlphabetically(TArray<FString> strings) {

	FString tempString;

	for (int i = 0; i < strings.Num(); ++i) {

		for (int j = i + 1; j < strings.Num(); ++j) {

			if (strings[i] > strings[j]) {

				tempString = strings[i];
				strings[i] = strings[j];
				strings[j] = tempString;
			}
		}
	}

	return strings;
}


//--------------------------------------------------------

// Sort Assets Names Alphabetically

TMap<int, FString> UVertexPaintFunctionLibrary::VertexPaintDetectionPlugin_SortAssetsNamesAlphabetically(TMap<int, FString> assetIndexAndName) {


	FString name_Local;
	TArray<FString> names_Local;
	assetIndexAndName.GenerateValueArray(names_Local);

	int index_Local;
	TArray<int> indexes_Local;
	assetIndexAndName.GetKeys(indexes_Local);


	for (int i = 0; i < names_Local.Num(); ++i) {

		for (int j = i + 1; j < names_Local.Num(); ++j) {

			if (names_Local[i] > names_Local[j]) {

				name_Local = names_Local[i];
				index_Local = indexes_Local[i];

				names_Local[i] = names_Local[j];
				indexes_Local[i] = indexes_Local[j];

				names_Local[j] = name_Local;
				indexes_Local[j] = index_Local;
			}
		}
	}

	TMap<int, FString> indexesAndTheirName_Local;

	for (int i = 0; i < indexes_Local.Num(); i++) {

		indexesAndTheirName_Local.Add(indexes_Local[i], names_Local[i]);
	}

	return indexesAndTheirName_Local;
}


//--------------------------------------------------------

// Draw Primitive Bounds Box

void UVertexPaintFunctionLibrary::DrawPrimitiveComponentBoundsBox(UPrimitiveComponent* component, float lifetime, float thickness, FLinearColor colorToDraw) {

	if (!IsInGameThread()) return;
	if (!IsValid(component)) return;
	if (!IsValid(component->GetWorld())) return;
	if (!component->GetWorld()->IsGameWorld()) return;
	if (component->GetWorld()->bIsTearingDown) return;


	if (Cast<USkeletalMeshComponent>(component)) {

		DrawDebugBox(component->GetWorld(), component->Bounds.GetBox().GetCenter(), component->Bounds.GetBox().GetExtent(), colorToDraw.ToFColor(false), false, lifetime, 0, thickness);
	}

	// In order to take the added collision of static meshes into account we had to get the AggGeom
	else if (Cast<UStaticMeshComponent>(component)) {

		FBoxSphereBounds AggGeomBounds;
		component->GetBodySetup()->AggGeom.CalcBoxSphereBounds(AggGeomBounds, component->GetComponentToWorld());

		DrawDebugBox(component->GetWorld(), component->Bounds.GetBox().GetCenter(), AggGeomBounds.GetBox().GetExtent(), colorToDraw.ToFColor(false), false, lifetime, 0, thickness);
	}
}


//--------------------------------------------------------

// Get Amount Of Fully Painted Colors For Each Channel - Gets % of each color from FColor Array

FVertexDetectAmountOfPaintedColorsOfEachChannel UVertexPaintFunctionLibrary::GetAmountOfPaintedColorsForEachChannel(const TArray<FColor>& vertexColors, float minColorAmountToBeConsidered) {

	if (vertexColors.Num() <= 0) return FVertexDetectAmountOfPaintedColorsOfEachChannel();


	FVertexDetectAmountOfPaintedColorsOfEachChannel amountOfPaintedColorsOfEachChannel_Local;
	FLinearColor linearColor_Local;


	for (auto colorTemp : vertexColors) {

		linearColor_Local = colorTemp.ReinterpretAsLinear(); // So we only have to run Reinterpret once


		if (linearColor_Local.R >= minColorAmountToBeConsidered) {

			amountOfPaintedColorsOfEachChannel_Local.amountOfVerticesWithAnyRedPaintedAtTheMinAmount++;
			amountOfPaintedColorsOfEachChannel_Local.averageAmountOfRedColorAtTheMinAmount += linearColor_Local.R;
		}
		if (linearColor_Local.G >= minColorAmountToBeConsidered) {

			amountOfPaintedColorsOfEachChannel_Local.amountOfVerticesWithAnyGreenPaintedAtTheMinAmount++;
			amountOfPaintedColorsOfEachChannel_Local.averageAmountOfGreenColorAtTheMinAmount += linearColor_Local.G;
		}
		if (linearColor_Local.B >= minColorAmountToBeConsidered) {

			amountOfPaintedColorsOfEachChannel_Local.amountOfVerticesWithAnyBluePaintedAtTheMinAmount++;
			amountOfPaintedColorsOfEachChannel_Local.averageAmountOfBlueColorAtTheMinAmount += linearColor_Local.B;
		}
		if (linearColor_Local.A >= minColorAmountToBeConsidered) {

			amountOfPaintedColorsOfEachChannel_Local.amountOfVerticesWithAnyAlphaPaintedAtTheMinAmount++;
			amountOfPaintedColorsOfEachChannel_Local.averageAmountOfAlphaColorAtTheMinAmount += linearColor_Local.A;
		}
	}


	amountOfPaintedColorsOfEachChannel_Local.averageAmountOfRedColorAtTheMinAmount = amountOfPaintedColorsOfEachChannel_Local.averageAmountOfRedColorAtTheMinAmount / vertexColors.Num();
	amountOfPaintedColorsOfEachChannel_Local.averageAmountOfGreenColorAtTheMinAmount = amountOfPaintedColorsOfEachChannel_Local.averageAmountOfGreenColorAtTheMinAmount / vertexColors.Num();
	amountOfPaintedColorsOfEachChannel_Local.averageAmountOfBlueColorAtTheMinAmount = amountOfPaintedColorsOfEachChannel_Local.averageAmountOfBlueColorAtTheMinAmount / vertexColors.Num();
	amountOfPaintedColorsOfEachChannel_Local.averageAmountOfAlphaColorAtTheMinAmount = amountOfPaintedColorsOfEachChannel_Local.averageAmountOfAlphaColorAtTheMinAmount / vertexColors.Num();


	amountOfPaintedColorsOfEachChannel_Local.amountOfRedChannelPercentPaintedAtTheMinAmount = amountOfPaintedColorsOfEachChannel_Local.amountOfVerticesWithAnyRedPaintedAtTheMinAmount / vertexColors.Num();
	amountOfPaintedColorsOfEachChannel_Local.amountOfGreenChannelPercentPaintedAtTheMinAmount = amountOfPaintedColorsOfEachChannel_Local.amountOfVerticesWithAnyGreenPaintedAtTheMinAmount / vertexColors.Num();
	amountOfPaintedColorsOfEachChannel_Local.amountOfBlueChannelPercentPaintedAtTheMinAmount = amountOfPaintedColorsOfEachChannel_Local.amountOfVerticesWithAnyBluePaintedAtTheMinAmount / vertexColors.Num();
	amountOfPaintedColorsOfEachChannel_Local.amountOfAlphaChannelPercentPaintedAtTheMinAmount = amountOfPaintedColorsOfEachChannel_Local.amountOfVerticesWithAnyAlphaPaintedAtTheMinAmount / vertexColors.Num();

	amountOfPaintedColorsOfEachChannel_Local.amountOfRedChannelPercentPaintedAtTheMinAmount *= 100;
	amountOfPaintedColorsOfEachChannel_Local.amountOfGreenChannelPercentPaintedAtTheMinAmount *= 100;
	amountOfPaintedColorsOfEachChannel_Local.amountOfBlueChannelPercentPaintedAtTheMinAmount *= 100;
	amountOfPaintedColorsOfEachChannel_Local.amountOfAlphaChannelPercentPaintedAtTheMinAmount *= 100;

	amountOfPaintedColorsOfEachChannel_Local.successfullyGotVerticesOfEachChannelAtTheMinAmount = true;

	return amountOfPaintedColorsOfEachChannel_Local;
}


//--------------------------------------------------------

// Get Skeletal Mesh

USkeletalMesh* UVertexPaintFunctionLibrary::VertexPaintDetectionPlugin_GetSkeletalMesh(USkeletalMeshComponent* skeletalMeshComp) {

	if (!IsValid(skeletalMeshComp)) return nullptr;


#if ENGINE_MAJOR_VERSION == 4

	return skeletalMeshComp->SkeletalMesh;

#elif ENGINE_MAJOR_VERSION == 5

#if ENGINE_MINOR_VERSION == 0

	return skeletalMeshComp->SkeletalMesh.Get();

#else

	return skeletalMeshComp->GetSkeletalMeshAsset();

#endif
#endif

	return nullptr;
}



//--------------------------------------------------------

// Get Mesh Component Vertex Colors

FVertexDetectMeshDataStruct UVertexPaintFunctionLibrary::GetMeshComponentVertexColors_Wrapper(UPrimitiveComponent* meshComponent, bool& success, bool getColorsForAllLODs, int getColorsUpToLOD) {

	success = false;
	if (!IsValid(meshComponent)) return FVertexDetectMeshDataStruct();
	if (!getColorsForAllLODs && getColorsUpToLOD <= 0) return FVertexDetectMeshDataStruct();


	FVertexDetectMeshDataStruct vertexMeshData_Local;
	TArray<FVertexDetectMeshDataPerLODStruct> meshDataPerLod_Local;
	int amountOfLODsToGet = getColorsUpToLOD;


	if (auto staticMeshComponent = Cast<UStaticMeshComponent>(meshComponent)) {

		if (!IsValid(staticMeshComponent->GetStaticMesh())) return FVertexDetectMeshDataStruct();


		vertexMeshData_Local.meshComp = staticMeshComponent;
		vertexMeshData_Local.meshSource = staticMeshComponent->GetStaticMesh();

		if (getColorsForAllLODs)
			amountOfLODsToGet = staticMeshComponent->GetStaticMesh()->GetRenderData()->LODResources.Num();


		for (int i = 0; i < amountOfLODsToGet; i++) {

			FVertexDetectMeshDataPerLODStruct meshDataPerLodTemp;
			meshDataPerLodTemp.lod = i;
			meshDataPerLodTemp.meshVertexColorsPerLODArray = VertexPaintDetectionPlugin_GetStaticMeshVertexColorsAtLOD(staticMeshComponent, i);

			// If actually got any colors, i.e. the lod was valid
			if (meshDataPerLodTemp.meshVertexColorsPerLODArray.Num() > 0)
				meshDataPerLod_Local.Add(meshDataPerLodTemp);
		}

		vertexMeshData_Local.meshDataPerLOD = meshDataPerLod_Local;
		success = true;

		return vertexMeshData_Local;
	}

	else if (auto skeletalMeshComponent = Cast<USkeletalMeshComponent>(meshComponent)) {

		USkeletalMesh* skelMesh = nullptr;

#if ENGINE_MAJOR_VERSION == 4

		skelMesh = skeletalMeshComponent->SkeletalMesh;

#elif ENGINE_MAJOR_VERSION == 5

#if ENGINE_MINOR_VERSION == 0

		skelMesh = skeletalMeshComponent->SkeletalMesh.Get();

#else

		skelMesh = skeletalMeshComponent->GetSkeletalMeshAsset();
#endif
#endif

		if (!IsValid(skelMesh)) return FVertexDetectMeshDataStruct();


		vertexMeshData_Local.meshComp = skeletalMeshComponent;
		vertexMeshData_Local.meshSource = skelMesh;

		if (getColorsForAllLODs)
			amountOfLODsToGet = skeletalMeshComponent->GetSkeletalMeshRenderData()->LODRenderData.Num();


		for (int i = 0; i < amountOfLODsToGet; i++) {

			FVertexDetectMeshDataPerLODStruct meshDataPerLodTemp;
			meshDataPerLodTemp.lod = i;
			meshDataPerLodTemp.meshVertexColorsPerLODArray = VertexPaintDetectionPlugin_GetSkeletalMeshVertexColorsAtLOD(skeletalMeshComponent, i);

			// If actually got any colors, i.e. the lod was valid
			if (meshDataPerLodTemp.meshVertexColorsPerLODArray.Num() > 0)
				meshDataPerLod_Local.Add(meshDataPerLodTemp);
		}

		vertexMeshData_Local.meshDataPerLOD = meshDataPerLod_Local;
		success = true;

		return vertexMeshData_Local;
	}

	return FVertexDetectMeshDataStruct();
}


//--------------------------------------------------------

// Get Skeletal Mesh Vertex Colors At LOD

TArray<FColor> UVertexPaintFunctionLibrary::VertexPaintDetectionPlugin_GetSkeletalMeshVertexColorsAtLOD(USkeletalMeshComponent* skeletalMeshComponent, int lod) {

	if (!IsValid(skeletalMeshComponent)) return TArray<FColor>();


	TArray<FColor> colorFromLOD_Local;

	if (skeletalMeshComponent->LODInfo.IsValidIndex(lod)) {

		// If been painted on before then can get the current Instance Color
		if (skeletalMeshComponent->LODInfo[lod].OverrideVertexColors) {

			skeletalMeshComponent->LODInfo[lod].OverrideVertexColors->GetVertexColors(colorFromLOD_Local);
		}
		else {

			if (skeletalMeshComponent->GetSkeletalMeshRenderData()->LODRenderData.IsValidIndex(lod))
				skeletalMeshComponent->GetSkeletalMeshRenderData()->LODRenderData[lod].StaticVertexBuffers.ColorVertexBuffer.GetVertexColors(colorFromLOD_Local);
		}
	}

	else {

		if (skeletalMeshComponent->GetSkeletalMeshRenderData()->LODRenderData.IsValidIndex(lod))
			skeletalMeshComponent->GetSkeletalMeshRenderData()->LODRenderData[lod].StaticVertexBuffers.ColorVertexBuffer.GetVertexColors(colorFromLOD_Local);
	}


	// In one instance when i tested on a side project with a purchased character, it got 0 in Num when first time painting at it, i.e. got vertex colors from SkeletalMeshRenderData. This only occured if the character hadn't gotten any paint on it and was imported with FColor(255,255,255,255) so it was an easy solution to just fill arrays to that color in this rare instance. So unlike the static mesh solution i couldn't initialize colorFromLOD_Local to be a length before running GetVertexColors because then i couldn't detect this rare case. 
	if (colorFromLOD_Local.Num() > 0) {

		// 
	}

	else {

		if (skeletalMeshComponent->GetSkeletalMeshRenderData()->LODRenderData.IsValidIndex(lod))
			colorFromLOD_Local.Init(FColor(255, 255, 255, 255), skeletalMeshComponent->GetSkeletalMeshRenderData()->LODRenderData[lod].GetNumVertices());
	}

	return colorFromLOD_Local;
}


//--------------------------------------------------------

// Get Static Mesh Vertex Colors At LOD

TArray<FColor> UVertexPaintFunctionLibrary::VertexPaintDetectionPlugin_GetStaticMeshVertexColorsAtLOD(UStaticMeshComponent* staticMeshComponent, int lod) {

	if (!IsValid(staticMeshComponent)) return TArray<FColor>();
	if (!IsValid(staticMeshComponent->GetStaticMesh())) return TArray<FColor>();
	if (!staticMeshComponent->GetStaticMesh()->bAllowCPUAccess) return TArray<FColor>();
	if (!staticMeshComponent->GetStaticMesh()->GetRenderData()->LODResources.IsValidIndex(lod)) return TArray<FColor>();


	TArray<FColor> colorFromLOD_Local;
	FColorVertexBuffer* colVertBufferAtLOD_Local = nullptr;
	int meshLODTotalAmountOfVerts_Local = staticMeshComponent->GetStaticMesh()->GetRenderData()->LODResources[lod].VertexBuffers.PositionVertexBuffer.GetNumVertices();

	colorFromLOD_Local.SetNum(meshLODTotalAmountOfVerts_Local, true);

	// Depending if override vertex colors is valid, we get the Vertex Colors from it, i.e. the instanced colors
	if (staticMeshComponent->LODData.IsValidIndex(lod)) {

		if (staticMeshComponent->LODData[lod].OverrideVertexColors) {

			staticMeshComponent->LODData[lod].OverrideVertexColors->GetVertexColors(colorFromLOD_Local);
		}

		else {

			staticMeshComponent->GetStaticMesh()->GetRenderData()->LODResources[lod].VertexBuffers.ColorVertexBuffer.GetVertexColors(colorFromLOD_Local);
		}
	}

	else {

		colVertBufferAtLOD_Local = &staticMeshComponent->GetStaticMesh()->GetRenderData()->LODResources[lod].VertexBuffers.ColorVertexBuffer;

		// If color buffer isn't initialized it means its default colors are White and it hasn't been painted either in editor or in runtime, if this is the case we init the array with white so even unstored, unpainted cpu meshes with all default white vertex colors can be painted and look as they should. 
		if (colVertBufferAtLOD_Local) {

			if (colVertBufferAtLOD_Local->IsInitialized()) {

				colVertBufferAtLOD_Local->GetVertexColors(colorFromLOD_Local);
			}
			else {

				colorFromLOD_Local.Init(FColor::White, meshLODTotalAmountOfVerts_Local);
			}
		}

		else {

			colorFromLOD_Local.Init(FColor::White, meshLODTotalAmountOfVerts_Local);
		}
	}

	return colorFromLOD_Local;
}


//-------------------------------------------------------

// Collision Channel to Object Type

EObjectTypeQuery UVertexPaintFunctionLibrary::CollisionChannelToObjectType(ECollisionChannel channel) {

	// Has this here since i don't want to include UnrealTypes in.h
	return UEngineTypes::ConvertToObjectType(channel);
}


//-------------------------------------------------------

// Object Type to Collision Channel

ECollisionChannel UVertexPaintFunctionLibrary::ObjectTypeToCollisionChannel(EObjectTypeQuery object) {

	// Has this here since i don't want to include UnrealTypes in.h
	return UEngineTypes::ConvertToCollisionChannel(object);
}


//-------------------------------------------------------

// Is In Editor

bool UVertexPaintFunctionLibrary::IsPlayInEditor(const UObject* WorldContextObject) {

	if (!IsValid(WorldContextObject)) return false;
	if (!IsValid(WorldContextObject->GetWorld())) return false;
	if (!IsWorldValid(WorldContextObject->GetWorld())) return false;

	return WorldContextObject->GetWorld()->IsPlayInEditor();
}


//-------------------------------------------------------

// Does Physics Surface Belong To Parent Surface

bool UVertexPaintFunctionLibrary::DoesPhysicsSurfaceBelongToPhysicsSurfaceFamily(TEnumAsByte<EPhysicalSurface> physicsSurface, TEnumAsByte<EPhysicalSurface> parentOfPhysicsSurfaceFamily) {

	if (physicsSurface == EPhysicalSurface::SurfaceType_Default) return false;
	if (parentOfPhysicsSurfaceFamily == EPhysicalSurface::SurfaceType_Default) return false;
	if (!UVertexPaintFunctionLibrary::GetVertexPaintMaterialDataAsset()) return false;
	if (!UVertexPaintFunctionLibrary::GetVertexPaintMaterialDataAsset()->GetPhysicsSurfaceFamilies().Contains(parentOfPhysicsSurfaceFamily)) return false;

	// If its the parent surface itself. 
	if (physicsSurface == parentOfPhysicsSurfaceFamily) return true;

	if (UVertexPaintFunctionLibrary::GetVertexPaintMaterialDataAsset()->GetPhysicsSurfaceFamilies().FindRef(parentOfPhysicsSurfaceFamily).childSurfaces.Contains(physicsSurface)) return true;

	return false;
}


//-------------------------------------------------------

// Get Colors To Apply Based Off Detected Physics Material

void UVertexPaintFunctionLibrary::GetColorsToApplyFromPhysicsSurface_Wrapper(UMaterialInterface* materialToApplyColorsTo, const TEnumAsByte<EPhysicalSurface>& physicalSurface, float& redAmountToApply, bool& redAmountSuccessfullyGot, float& greenAmountToApply, bool& greenAmountSuccessfullyGot, float& blueAmountToApply, bool& blueAmountSuccessfullyGot, float& alphaAmountToApply, bool& alphaAmountSuccessfullyGot, bool& successfullyGotColorsToApplyOnAChannel, float amountToReturnOnChannelsWithPhysicsSurface, float amountToReturnOnChannelsWithoutThePhysicsSurface, bool applyOnChannelsWithSamePhysicsParents, bool applyOnChannelsThatsChildOfPhysicsSurface, bool applyOnChannelsThatResultsInBlendableSurfaces) {

	successfullyGotColorsToApplyOnAChannel = false;

	redAmountToApply = 0;
	redAmountSuccessfullyGot = false;

	greenAmountToApply = 0;
	greenAmountSuccessfullyGot = false;

	blueAmountToApply = 0;
	blueAmountSuccessfullyGot = false;

	alphaAmountToApply = 0;
	alphaAmountSuccessfullyGot = false;


	// if (amountToReturnOnChannelsWithPhysicsSurface == 0) return; // We DON'T want to return if 0, since if we're painting with Set instead of Adding, then 0 may be what we want. 
	if (physicalSurface == EPhysicalSurface::SurfaceType_Default) return;
	if (!UVertexPaintFunctionLibrary::GetVertexPaintMaterialDataAsset()) return;
	materialToApplyColorsTo = UVertexPaintFunctionLibrary::GetVertexPaintMaterialDataAsset()->GetRegisteredMaterialInstanceOrParentMaterial(materialToApplyColorsTo);
	if (!IsValid(materialToApplyColorsTo)) return;




	TEnumAsByte<EPhysicalSurface> physicsSurfacePaintedAtRed = UVertexPaintFunctionLibrary::GetVertexPaintMaterialDataAsset()->GetVertexPaintMaterialInterface().FindRef(materialToApplyColorsTo).paintedAtRed;
	TEnumAsByte<EPhysicalSurface> physicsSurfacePaintedAtGreen = UVertexPaintFunctionLibrary::GetVertexPaintMaterialDataAsset()->GetVertexPaintMaterialInterface().FindRef(materialToApplyColorsTo).paintedAtGreen;
	TEnumAsByte<EPhysicalSurface> physicsSurfacePaintedAtBlue = UVertexPaintFunctionLibrary::GetVertexPaintMaterialDataAsset()->GetVertexPaintMaterialInterface().FindRef(materialToApplyColorsTo).paintedAtBlue;
	TEnumAsByte<EPhysicalSurface> physicsSurfacePaintedAtAlpha = UVertexPaintFunctionLibrary::GetVertexPaintMaterialDataAsset()->GetVertexPaintMaterialInterface().FindRef(materialToApplyColorsTo).paintedAtAlpha;


	// If Exactly what we're trying to paint is on the channel
	if (physicalSurface == physicsSurfacePaintedAtRed)
		redAmountSuccessfullyGot = true;

	if (physicalSurface == physicsSurfacePaintedAtGreen)
		greenAmountSuccessfullyGot = true;

	if (physicalSurface == physicsSurfacePaintedAtBlue)
		blueAmountSuccessfullyGot = true;

	if (physicalSurface == physicsSurfacePaintedAtAlpha)
		alphaAmountSuccessfullyGot = true;


	TArray<TEnumAsByte<EPhysicalSurface>> parentPhysicsSurfaceTemp = UVertexPaintFunctionLibrary::GetVertexPaintMaterialDataAsset()->GetPhysicsSurfaceFamilyParentsOfPhysicsSurface(physicalSurface);

	// If we want to Apply on the channel if it has a Parent of what's on the Channel. For instance Cobble-Sand didn't exist on any of the Channels, but Sand which is a Parent does exist, then we want to apply. Useful if for instance Characters that has fundemental Parent Surfaces like Sand, Mud etc. is walking over different types of Sandy/Muddy surfaces and we want them to rub off on the correct channel on the character. 
	if (applyOnChannelsWithSamePhysicsParents) {

		if (!redAmountSuccessfullyGot && parentPhysicsSurfaceTemp.Contains(physicsSurfacePaintedAtRed))
			redAmountSuccessfullyGot = true;

		if (!greenAmountSuccessfullyGot && parentPhysicsSurfaceTemp.Contains(physicsSurfacePaintedAtGreen))
			greenAmountSuccessfullyGot = true;

		if (!blueAmountSuccessfullyGot && parentPhysicsSurfaceTemp.Contains(physicsSurfacePaintedAtBlue))
			blueAmountSuccessfullyGot = true;

		if (!alphaAmountSuccessfullyGot && parentPhysicsSurfaceTemp.Contains(physicsSurfacePaintedAtAlpha))
			alphaAmountSuccessfullyGot = true;
	}


	FVertexPaintParentPhysicsSurfaceStruct physicsSurfaceFamilyTemp;

	// Checks if physics surface we're trying to apply is a parent, and if what's on each of the vertex color channels is a child of that parent. Then we can apply color on that channel as well. This can be useful if for instance you have a Wheel with Parent Surface Sand on it, that is driving over a Material that doesn't have Sand registered, but it has Cobble-Sand which is a child of it. Then the sandy Wheel can Rub off on the correct channel and spread Sand onto the Cobble-Sand. 
	if (applyOnChannelsThatsChildOfPhysicsSurface && UVertexPaintFunctionLibrary::GetVertexPaintMaterialDataAsset()->GetPhysicsSurfaceFamilies().Contains(physicalSurface)) {

		physicsSurfaceFamilyTemp = UVertexPaintFunctionLibrary::GetVertexPaintMaterialDataAsset()->GetPhysicsSurfaceFamilies().FindRef(physicalSurface);

		if (physicsSurfaceFamilyTemp.childSurfaces.Num() > 0) {

			if (!redAmountSuccessfullyGot) {

				if (physicsSurfaceFamilyTemp.childSurfaces.Contains(physicsSurfacePaintedAtRed))
					redAmountSuccessfullyGot = true;
			}

			if (!greenAmountSuccessfullyGot) {

				if (physicsSurfaceFamilyTemp.childSurfaces.Contains(physicsSurfacePaintedAtGreen))
					greenAmountSuccessfullyGot = true;
			}

			if (!blueAmountSuccessfullyGot) {

				if (physicsSurfaceFamilyTemp.childSurfaces.Contains(physicsSurfacePaintedAtBlue))
					blueAmountSuccessfullyGot = true;
			}

			if (!alphaAmountSuccessfullyGot) {

				if (physicsSurfaceFamilyTemp.childSurfaces.Contains(physicsSurfacePaintedAtAlpha))
					alphaAmountSuccessfullyGot = true;
			}
		}
	}



	// Checks Physics Surfaces that it can Blend into as well, if we're trying to apply a blended surface or if we should apply it on parents or childs of it. In the Example Project for instance we have a Cobble Material with Cobble-Sand on Red Channel, and Cobble-Puddle on Blue, which can blend into Cobble-Mud, which is a child of Mud. So if for instance a wheel that has Mud that is driving over the Cobble Material, we can with this get Cobble-Mud if set to applyOnChannelsThatsChildOfPhysicsSurface, and that it's a child of Mud, which means the Wheel with Mud can paint off itself on the channels that formed Cobble-Mud. Same way other way around if applyOnChannelsWithSamePhysicsParents is true, where a clean wheel can get painted with Mud by Cobble-Mud. 
	if (applyOnChannelsThatResultsInBlendableSurfaces) {

		// Gets Physics Surface Blend Settings on the Material we're Applying Colors on, for instance the Cobble Material from the Example Project has some registered 
		for (auto& blendSettingTemp : UVertexPaintFunctionLibrary::GetVertexPaintMaterialDataAsset()->GetVertexPaintMaterialInterface().FindRef(materialToApplyColorsTo).physicsSurfaceBlendingSettings) {

			// If we're trying to apply a Blendable Surface, for instance Cobble-Mud, Or we've set to affect childs of a parent, for instance Mud, and we find the Blendable Cobble-Mud as a child
			if (blendSettingTemp.Key == physicalSurface || (applyOnChannelsThatsChildOfPhysicsSurface && physicsSurfaceFamilyTemp.childSurfaces.Contains(blendSettingTemp.Key)) || (applyOnChannelsWithSamePhysicsParents && parentPhysicsSurfaceTemp.Contains(blendSettingTemp.Key))) {

				TArray<TEnumAsByte<EPhysicalSurface>> blendableParentsPhysicsSurfaceTemp = UVertexPaintFunctionLibrary::GetVertexPaintMaterialDataAsset()->GetPhysicsSurfaceFamilyParentsOfPhysicsSurface(blendSettingTemp.Key);


				// Now we know that the Blended Surface checks out, and we just need to get which vertex color channels that was the result of that blend, so we can set those channels to return the correct amount. To do this we loop through the physics surfaces that resultet in the blend, and then runs GetVertexColorChannelsPhysicsSurfaceIsRegisteredTo on that physics surface
				for (auto physicsSurfacesThatBlendedIntoEachotherTemp : blendSettingTemp.Value.physicsSurfacesThatCanBlend) {

					bool successfullyGotChannelPhysicsSurfaceIsRegisteredToLocal = false;
					auto channelPhysicsSurfaceIsRegisteredTo_Local = GetVertexColorChannelsPhysicsSurfaceIsRegisteredTo(materialToApplyColorsTo, physicsSurfacesThatBlendedIntoEachotherTemp, successfullyGotChannelPhysicsSurfaceIsRegisteredToLocal);

					if (successfullyGotChannelPhysicsSurfaceIsRegisteredToLocal) {

						for (auto vertexColorChannel : channelPhysicsSurfaceIsRegisteredTo_Local) {

							switch (vertexColorChannel) {

							case Enum_SurfaceAtChannel::Default:
								break;

							case Enum_SurfaceAtChannel::RedChannel:

								redAmountSuccessfullyGot = true;
								break;

							case Enum_SurfaceAtChannel::GreenChannel:

								greenAmountSuccessfullyGot = true;
								break;

							case Enum_SurfaceAtChannel::BlueChannel:

								blueAmountSuccessfullyGot = true;
								break;

							case Enum_SurfaceAtChannel::AlphaChannel:

								alphaAmountSuccessfullyGot = true;
								break;

							default:
								break;
							}
						}
					}
				}
			}
		}
	}


	if (redAmountSuccessfullyGot)
		redAmountToApply = amountToReturnOnChannelsWithPhysicsSurface;
	else
		redAmountToApply = amountToReturnOnChannelsWithoutThePhysicsSurface;


	if (greenAmountSuccessfullyGot)
		greenAmountToApply = amountToReturnOnChannelsWithPhysicsSurface;
	else
		greenAmountToApply = amountToReturnOnChannelsWithoutThePhysicsSurface;


	if (blueAmountSuccessfullyGot)
		blueAmountToApply = amountToReturnOnChannelsWithPhysicsSurface;
	else
		blueAmountToApply = amountToReturnOnChannelsWithoutThePhysicsSurface;


	if (alphaAmountSuccessfullyGot)
		alphaAmountToApply = amountToReturnOnChannelsWithPhysicsSurface;
	else
		alphaAmountToApply = amountToReturnOnChannelsWithoutThePhysicsSurface;


	// If managed to get color to apply on any channel at all. 
	if (redAmountSuccessfullyGot || greenAmountSuccessfullyGot || blueAmountSuccessfullyGot || alphaAmountSuccessfullyGot) {

		successfullyGotColorsToApplyOnAChannel = true;
	}

	else {

		redAmountToApply = 0;
		greenAmountToApply = 0;
		blueAmountToApply = 0;
		alphaAmountToApply = 0;
	}
}


//-------------------------------------------------------

// Get The Most Dominant Physics Surface

bool UVertexPaintFunctionLibrary::GetTheMostDominantPhysicsSurface_Wrapper(UMaterialInterface* optionalMaterialPhysicsSurfaceWasDetectedOn, TArray<TEnumAsByte<EPhysicalSurface>> physicsSurfaces, TArray<float> physicsSurfaceValues, TEnumAsByte<EPhysicalSurface>& mostDominantPhysicsSurfaceFromArray, float& mostDominantPhysicsSurfaceColorValue) {

	mostDominantPhysicsSurfaceFromArray = EPhysicalSurface::SurfaceType_Default;
	mostDominantPhysicsSurfaceColorValue = 0;

	if (physicsSurfaces.Num() <= 0) return false;
	if (physicsSurfaceValues.Num() <= 0) return false;
	if (physicsSurfaces.Num() != physicsSurfaceValues.Num()) return false;

	float strongestSurfaceValue_Local = 0;
	TEnumAsByte<EPhysicalSurface> strongestSurface_Local = EPhysicalSurface::SurfaceType_Default;
	TArray<TEnumAsByte<EPhysicalSurface>> blendedSurfaceResults_Local;
	TArray<float> blendedSurfaceValueResults_Local;


	// First just gets the surface with the strongest value
	for (int i = 0; i < physicsSurfaces.Num(); i++) {

		// Has to have something. So if you send in an array where everything is 0 then you shouldn't be able to get a dominant surface
		if (physicsSurfaceValues[i] > 0) {

			if (physicsSurfaceValues[i] >= strongestSurfaceValue_Local) {

				strongestSurface_Local = physicsSurfaces[i];
				strongestSurfaceValue_Local = physicsSurfaceValues[i];
			}
		}
	}

	// If material is registered then checks if any of the surfaces can blend
	if (IsValid(UVertexPaintFunctionLibrary::GetVertexPaintMaterialDataAsset())) {


		TArray<FVertexPaintPhysicsSurfaceBlendingSettings> surfaceBlendSettings_Local;


		if (!UVertexPaintFunctionLibrary::GetVertexPaintMaterialDataAsset()->GetVertexPaintMaterialInterface().Contains(optionalMaterialPhysicsSurfaceWasDetectedOn)) {

			// If not registered, then if it's an instance, then replaces material with the parent and checks that. Makes it possible so you don't have to register every single instance but can if you wish only register the parent
			if (auto materialInstanceTemp = Cast<UMaterialInstance>(optionalMaterialPhysicsSurfaceWasDetectedOn)) {

				optionalMaterialPhysicsSurfaceWasDetectedOn = materialInstanceTemp->Parent;
			}
		}


		if (UVertexPaintFunctionLibrary::GetVertexPaintMaterialDataAsset()->GetVertexPaintMaterialInterface().Contains(optionalMaterialPhysicsSurfaceWasDetectedOn)) {

			UVertexPaintFunctionLibrary::GetVertexPaintMaterialDataAsset()->GetVertexPaintMaterialInterface().FindRef(optionalMaterialPhysicsSurfaceWasDetectedOn).physicsSurfaceBlendingSettings.GenerateValueArray(surfaceBlendSettings_Local);


			bool blendedSurfaces_ContainsAll_Local = true;
			bool blendedSurfaces_AllHaveMinAmount_Local = true;
			float blendedSurfaces_TotalAmount_Local = 0;


			// Then checks if any of the surface we got as a parameter can blendand if their value combined allows them to blendand is then the strongest surface
			for (auto blendSettingTemp : surfaceBlendSettings_Local) {

				blendedSurfaces_ContainsAll_Local = true;

				// If the phys surface array sent in even has all necessary surfaces for this blend setting
				for (auto physSurfaceThatBlendTemp : blendSettingTemp.physicsSurfacesThatCanBlend) {

					if (!physicsSurfaces.Contains(physSurfaceThatBlendTemp)) {

						blendedSurfaces_ContainsAll_Local = false;
						break;
					}
				}

				if (!blendedSurfaces_ContainsAll_Local) continue;


				blendedSurfaces_AllHaveMinAmount_Local = true;
				blendedSurfaces_TotalAmount_Local = 0;

				// If we know we have all phys surfaces requires, then checks if each has min required color amount that they require to blend
				for (auto physSurfaceThatBlendTemp : blendSettingTemp.physicsSurfacesThatCanBlend) {

					if (physicsSurfaceValues[physicsSurfaces.Find(physSurfaceThatBlendTemp)] >= blendSettingTemp.minAmountOnEachSurfaceToBeAbleToBlend) {

						blendedSurfaces_TotalAmount_Local += physicsSurfaceValues[physicsSurfaces.Find(physSurfaceThatBlendTemp)];
					}
					else {

						blendedSurfaces_AllHaveMinAmount_Local = false;
						break;
					}
				}

				if (!blendedSurfaces_AllHaveMinAmount_Local) continue;


				blendedSurfaceResults_Local.Add(blendSettingTemp.physicsSurfaceToResultIn);
				blendedSurfaceValueResults_Local.Add(blendedSurfaces_TotalAmount_Local);
			}
		}
	}

	// If got any blendes surfaces then checks if they're stronger than the strongest surface we got earlier when we checked them invidiually and updates it. If there are several blendes surfaces, the one that combines has the strongest value will be considered the strongest surface
	for (int i = 0; i < blendedSurfaceValueResults_Local.Num(); i++) {

		if (blendedSurfaceValueResults_Local[i] > strongestSurfaceValue_Local) {


			strongestSurfaceValue_Local = blendedSurfaceValueResults_Local[i];
			strongestSurface_Local = blendedSurfaceResults_Local[i];
		}
	}

	// Only returns true if we actually got a surface, if they where all 0 so strongest surface is the default 0 then we don't want to return anything surface
	if (strongestSurface_Local != EPhysicalSurface::SurfaceType_Default && strongestSurfaceValue_Local > 0) {

		mostDominantPhysicsSurfaceFromArray = strongestSurface_Local;
		mostDominantPhysicsSurfaceColorValue = strongestSurfaceValue_Local;

		return true;
	}

	return false;
}


//-------------------------------------------------------

// Get Vertex Color Channels Physics Surface Is Registered To Wrapper

TArray<Enum_SurfaceAtChannel> UVertexPaintFunctionLibrary::GetAllVertexColorChannelsPhysicsSurfaceIsRegisteredTo_Wrapper(UMaterialInterface* material, TEnumAsByte<EPhysicalSurface> physicsSurface, bool& successfull) {

	successfull = false;
	TArray< Enum_SurfaceAtChannel> surfaceAtChannels; // Array in case it's a blendable where there are several channels that make up the physics surface

	if (!IsValid(material)) return surfaceAtChannels;
	if (!IsValid(UVertexPaintFunctionLibrary::GetVertexPaintMaterialDataAsset())) return surfaceAtChannels;


	if (!UVertexPaintFunctionLibrary::GetVertexPaintMaterialDataAsset()->GetVertexPaintMaterialInterface().Contains(material)) {

		// If not registered, then if it's an instance, then replaces material with the parent and checks that. Makes it possible so you don't have to register every single instance but can if you wish only register the parent
		if (auto materialInstanceTemp = Cast<UMaterialInstance>(material)) {

			material = materialInstanceTemp->Parent;

			if (!UVertexPaintFunctionLibrary::GetVertexPaintMaterialDataAsset()->GetVertexPaintMaterialInterface().Contains(material))
				return surfaceAtChannels;
		}
		else {

			return surfaceAtChannels;
		}
	}


	auto materialDataAssetTemp = UVertexPaintFunctionLibrary::GetVertexPaintMaterialDataAsset()->GetVertexPaintMaterialInterface().FindRef(material);

	// Gets if physics surface is registered to R, G, B or A for this Material, if so then returns
	surfaceAtChannels = GetVertexColorChannelsPhysicsSurfaceIsRegisteredTo(material, physicsSurface, successfull);

	if (successfull) {

		return surfaceAtChannels;
	}

	// If couldn't find any, then checks if there are several blendables that make up this surface, and get the surface channels for them. 
	for (auto& blendSettingTemp : materialDataAssetTemp.physicsSurfaceBlendingSettings) {

		if (blendSettingTemp.Key == physicsSurface) {

			for (auto blendedPhysSurfacesThatMakeUpThePhysicsSurface : blendSettingTemp.Value.physicsSurfacesThatCanBlend) {

				TArray< Enum_SurfaceAtChannel> blendableChannelsThatMakeUpPhysicsSurfaceTemp = GetVertexColorChannelsPhysicsSurfaceIsRegisteredTo(material, blendedPhysSurfacesThatMakeUpThePhysicsSurface, successfull);

				if (successfull)
					surfaceAtChannels.Append(blendableChannelsThatMakeUpPhysicsSurfaceTemp);
			}

			if (successfull)
				return surfaceAtChannels;
		}
	}

	return surfaceAtChannels;
}


//-------------------------------------------------------

// Get Vertex Color Channels Physics Surface Is Registered To - This just checks the RGBA channels and not Blendables like the one above

TArray<Enum_SurfaceAtChannel> UVertexPaintFunctionLibrary::GetVertexColorChannelsPhysicsSurfaceIsRegisteredTo(UMaterialInterface* material, TEnumAsByte<EPhysicalSurface> physicsSurface, bool& successfull) {

	successfull = false;

	if (!IsValid(material)) return TArray<Enum_SurfaceAtChannel>();
	if (!IsValid(UVertexPaintFunctionLibrary::GetVertexPaintMaterialDataAsset())) return TArray<Enum_SurfaceAtChannel>();

	if (!UVertexPaintFunctionLibrary::GetVertexPaintMaterialDataAsset()->GetVertexPaintMaterialInterface().Contains(material)) {

		// If not registered, then if it's an instance, then replaces material with the parent and checks that. Makes it possible so you don't have to register every single instance but can if you wish only register the parent
		if (auto materialInstanceTemp = Cast<UMaterialInstance>(material)) {

			material = materialInstanceTemp->Parent;

			if (!UVertexPaintFunctionLibrary::GetVertexPaintMaterialDataAsset()->GetVertexPaintMaterialInterface().Contains(material))
				return TArray<Enum_SurfaceAtChannel>();
		}
		else {

			return TArray<Enum_SurfaceAtChannel>();
		}
	}

	auto materialDataAssetTemp = UVertexPaintFunctionLibrary::GetVertexPaintMaterialDataAsset()->GetVertexPaintMaterialInterface().FindRef(material);

	TArray<Enum_SurfaceAtChannel> atSurfaceChannels_Local;


	if (materialDataAssetTemp.atDefault == physicsSurface) {

		successfull = true;
		atSurfaceChannels_Local.Add(Enum_SurfaceAtChannel::Default);
	}

	if (materialDataAssetTemp.paintedAtRed == physicsSurface) {

		successfull = true;
		atSurfaceChannels_Local.Add(Enum_SurfaceAtChannel::RedChannel);
	}

	if (materialDataAssetTemp.paintedAtGreen == physicsSurface) {

		successfull = true;
		atSurfaceChannels_Local.Add(Enum_SurfaceAtChannel::GreenChannel);
	}

	if (materialDataAssetTemp.paintedAtBlue == physicsSurface) {

		successfull = true;
		atSurfaceChannels_Local.Add(Enum_SurfaceAtChannel::BlueChannel);
	}

	if (materialDataAssetTemp.paintedAtAlpha == physicsSurface) {

		successfull = true;
		atSurfaceChannels_Local.Add(Enum_SurfaceAtChannel::AlphaChannel);
	}

	return atSurfaceChannels_Local;
}

//-------------------------------------------------------

// Adjust Callback Vertex Data

FCalculateColorsInfo UVertexPaintFunctionLibrary::AdjustCallbackVertexData(FCalculateColorsInfo calculateColorsInfo, bool resetColorsOfBoneIfNotIncluded, bool resetAmountOfPaintedColorsOfEachChannelIfNotIncluded, bool resetMeshVertexDataIfNotIncluded) {


	if (!calculateColorsInfo.fundementalStruct.callbackSettings.includeColorsOfEachBone && resetColorsOfBoneIfNotIncluded) {

		calculateColorsInfo.vertexColorsOnEachBone = FVertexDetectVertexColorsOnEachBone();
	}


	if (!calculateColorsInfo.fundementalStruct.callbackSettings.includeAmountOfPaintedColorsOfEachChannel.includeAmountOfPaintedColorsOfEachChannel && resetAmountOfPaintedColorsOfEachChannelIfNotIncluded) {

		calculateColorsInfo.amountOfPaintedColorsOfEachChannelAtMinAmount_AfterApplyingColor = FVertexDetectAmountOfPaintedColorsOfEachChannel();
	}


	if (calculateColorsInfo.fundementalStruct.callbackSettings.includeVertexDataOnlyForLOD0 && calculateColorsInfo.meshVertexData.meshDataPerLOD.Num() > 1) {

		// Starting at LOD1 we empty each array, so we can still return the meshDataPerLOD array with the correct length and amount of vertices at each LOD etc. even if set to only include vertex data for LOD0
		for (int i = 1; i < calculateColorsInfo.meshVertexData.meshDataPerLOD.Num(); i++) {

			calculateColorsInfo.meshVertexData.meshDataPerLOD[i].meshVertexColorsPerLODArray.Empty();
			calculateColorsInfo.meshVertexData.meshDataPerLOD[i].meshVertexPositionsInComponentSpacePerLODArray.Empty();
			calculateColorsInfo.meshVertexData.meshDataPerLOD[i].meshVertexNormalsPerLODArray.Empty();
			calculateColorsInfo.meshVertexData.meshDataPerLOD[i].serializedVertexColorsData.colorsAtLODAsJSonString = "";
		}
	}


	if (resetMeshVertexDataIfNotIncluded) {

		for (int i = 0; i < calculateColorsInfo.meshVertexData.meshDataPerLOD.Num(); i++) {

			if (!calculateColorsInfo.fundementalStruct.callbackSettings.includeVertexColorData || !calculateColorsInfo.fundementalStruct.callbackSettings.runCallbackDelegate)
				calculateColorsInfo.meshVertexData.meshDataPerLOD[i].meshVertexColorsPerLODArray.Empty();

			if (!calculateColorsInfo.fundementalStruct.callbackSettings.includeVertexPositionData || !calculateColorsInfo.fundementalStruct.callbackSettings.runCallbackDelegate)
				calculateColorsInfo.meshVertexData.meshDataPerLOD[i].meshVertexPositionsInComponentSpacePerLODArray.Empty();

			if (!calculateColorsInfo.fundementalStruct.callbackSettings.includeVertexNormalData || !calculateColorsInfo.fundementalStruct.callbackSettings.runCallbackDelegate)
				calculateColorsInfo.meshVertexData.meshDataPerLOD[i].meshVertexNormalsPerLODArray.Empty();
		}
	}


	return calculateColorsInfo;
}


//--------------------------------------------------------

// Get Clothing Assets

TArray<UClothingAssetBase*> UVertexPaintFunctionLibrary::GetClothAssets(USkeletalMesh* skeletalMesh) {

	if (!IsValid(skeletalMesh)) return TArray<UClothingAssetBase*>();

	return skeletalMesh->GetMeshClothingAssets();
}


//-------------------------------------------------------

// Set Cloth Physics

void UVertexPaintFunctionLibrary::SetChaosClothPhysics(USkeletalMeshComponent* skeletalMeshComponent, UClothingAssetBase* clothingAsset, const FVertexDetectChaosClothPhysicsSettings& clothPhysicsSettings) {

#if ENGINE_MAJOR_VERSION == 5

	if (!IsValid(skeletalMeshComponent) || !IsValid(clothingAsset)) return;


	auto clothingSimulationInteractorTemp = skeletalMeshComponent->GetClothingSimulationInteractor();

	if (auto chaosClothingSimilationInteractorTemp = Cast<UChaosClothingSimulationInteractor>(clothingSimulationInteractorTemp)) {

		if (auto clothingInteractorTemp = chaosClothingSimilationInteractorTemp->GetClothingInteractor(clothingAsset->GetName())) {

			if (auto chaosClothingInteractorTemp = Cast<UChaosClothingInteractor>(clothingInteractorTemp)) {


				if (clothPhysicsSettings.SetDamping)
					chaosClothingInteractorTemp->SetDamping(clothPhysicsSettings.clothDampingSettings.SetDamping_dampingCoefficient);

				if (clothPhysicsSettings.SetGravity)
					chaosClothingInteractorTemp->SetGravity(clothPhysicsSettings.clothGravitySettings.SetGravity_gravityScale, clothPhysicsSettings.clothGravitySettings.SetGravity_overrideGravity, clothPhysicsSettings.clothGravitySettings.SetGravity_gravityOverride);

				if (clothPhysicsSettings.SetWind)
					chaosClothingInteractorTemp->SetWind(clothPhysicsSettings.clothWindSettings.SetWind_drag, clothPhysicsSettings.clothWindSettings.SetWind_lift, clothPhysicsSettings.clothWindSettings.SetWind_airDensity, clothPhysicsSettings.clothWindSettings.SetWind_windVelocity);

				if (clothPhysicsSettings.SetAnimDrive)
					chaosClothingInteractorTemp->SetAnimDrive(clothPhysicsSettings.clothAnimDriveSettings.SetAnimDrive_Stiffness, clothPhysicsSettings.clothAnimDriveSettings.SetAnimDrive_Damping);

				if (clothPhysicsSettings.SetCollision)
					chaosClothingInteractorTemp->SetCollision(clothPhysicsSettings.clothCollisionSettings.SetCollision_CollisionThickness, clothPhysicsSettings.clothCollisionSettings.SetCollision_FrictionCoefficient, clothPhysicsSettings.clothCollisionSettings.SetCollision_UseCCD, clothPhysicsSettings.clothCollisionSettings.SetCollision_SelfCollisionThickness);

				if (clothPhysicsSettings.SetLongRangeAttachment)
					chaosClothingInteractorTemp->SetLongRangeAttachment(clothPhysicsSettings.clothLongRangeAttachmentSettings.LongRangeAttachment_TetherThickness, clothPhysicsSettings.clothLongRangeAttachmentSettings.LongRangeAttachment_TetherScale);

				if (clothPhysicsSettings.SetMaterial)
					chaosClothingInteractorTemp->SetMaterial(clothPhysicsSettings.clothMaterialSettings.Material_EdgeStiffness, clothPhysicsSettings.clothMaterialSettings.Material_BendingStiffness, clothPhysicsSettings.clothMaterialSettings.Material_AreaStiffness);

				if (clothPhysicsSettings.SetPhysicsVelocityScale)
					chaosClothingInteractorTemp->SetVelocityScale(clothPhysicsSettings.clothPhysicsVelocityScaleSettings.PhysicsVelocityScale_LinearVelocityScale, clothPhysicsSettings.clothPhysicsVelocityScaleSettings.PhysicVelocityScale_AngularVelocityScale, clothPhysicsSettings.clothPhysicsVelocityScaleSettings.PhysicsVelocityScale_FictitiousAngularVelocityScale);

#if ENGINE_MINOR_VERSION >= 1

				if (clothPhysicsSettings.SetAirPressure) {
					chaosClothingInteractorTemp->SetPressure(clothPhysicsSettings.clothPhysicssAirPressureSettings.SetPressure_Pressure);
				}
#endif
			}
		}
	}


#endif
}


//-------------------------------------------------------

// Updated Chaos Cloth Physics With Existing Colors

void UVertexPaintFunctionLibrary::UpdateChaosClothPhysicsWithExistingColors(USkeletalMeshComponent* skeletalMeshComponent) {

#if ENGINE_MAJOR_VERSION == 5

	if (!IsValid(skeletalMeshComponent)) return;

	UpdateChaosClothPhysics(skeletalMeshComponent->GetOwner(), skeletalMeshComponent, false, FLinearColor::White);

#endif
}

#if ENGINE_MAJOR_VERSION == 5

//-------------------------------------------------------

// Update Chaos Cloth Physics

void UVertexPaintFunctionLibrary::UpdateChaosClothPhysics(AActor* actor, USkeletalMeshComponent* skeletalMeshComponent, bool useGivenColorAsAvarageColor, FLinearColor avarageColor) {

	if (!IsValid(skeletalMeshComponent) || !IsValid(actor)) return;
	if (!IsWorldValid(skeletalMeshComponent->GetWorld())) return;
	if (!IsInGameThread()) return;


	const FSkeletalMeshLODRenderData& skelMeshRenderDataTemp = skeletalMeshComponent->GetSkeletalMeshRenderData()->LODRenderData[skeletalMeshComponent->GetPredictedLODLevel()];


	if (skelMeshRenderDataTemp.HasClothData()) {

		TMap<UClothingAssetBase*, FVertexDetectClothSettingsStruct> clothsAndTheirPhysicsSettings_Local;

		if (UKismetSystemLibrary::DoesImplementInterface(actor, UVertexPaintDetectionInterface::StaticClass()))
			clothsAndTheirPhysicsSettings_Local = IVertexPaintDetectionInterface::Execute_GetSkeletalMeshClothPhysicsSettings(actor, skeletalMeshComponent);


		if (clothsAndTheirPhysicsSettings_Local.Num() > 0) {

			USkeletalMesh* skelMeshTemp = nullptr;

#if ENGINE_MAJOR_VERSION == 4

			skelMeshTemp = skeletalMeshComponent->SkeletalMesh;

#elif ENGINE_MAJOR_VERSION == 5

#if ENGINE_MINOR_VERSION == 0

			skelMeshTemp = skeletalMeshComponent->SkeletalMesh.Get();

#else

			skelMeshTemp = skeletalMeshComponent->GetSkeletalMeshAsset();

#endif

#endif


			// Loops through Cloths
			for (int i = 0; i < skelMeshTemp->GetMeshClothingAssets().Num(); i++) {

				// Loops through Render Sections on current LOD
				for (int j = 0; j < skeletalMeshComponent->GetSkeletalMeshRenderData()->LODRenderData[skeletalMeshComponent->GetPredictedLODLevel()].RenderSections.Num(); j++) {

					// When on the same section as the Cloth
					if (i == skeletalMeshComponent->GetSkeletalMeshRenderData()->LODRenderData[skeletalMeshComponent->GetPredictedLODLevel()].RenderSections[j].CorrespondClothAssetIndex) {

						// Gets start and end vertex, which could be for instance 2300-2600 or something. So we get vertex colors below for just these verts
						int sectionStartVertexIndex = skeletalMeshComponent->GetSkeletalMeshRenderData()->LODRenderData[skeletalMeshComponent->GetPredictedLODLevel()].RenderSections[j].BaseVertexIndex;

						int sectionTotalAmountOfVertices = sectionStartVertexIndex + skeletalMeshComponent->GetSkeletalMeshRenderData()->LODRenderData[skeletalMeshComponent->GetPredictedLODLevel()].RenderSections[j].GetNumVertices();


						TArray<FColor> clothColorsTemp;

						// If Setting the entire mesh to be a specific color, then we already know the avarage color and doesn't have to run loops
						if (!useGivenColorAsAvarageColor) {

							FColor vertexColorTemp;
							for (int32 k = sectionStartVertexIndex; k < sectionTotalAmountOfVertices; k++) {


								// If been painted on before then can get the current Instance Color
								if (skeletalMeshComponent->LODInfo[skeletalMeshComponent->GetPredictedLODLevel()].OverrideVertexColors)
									vertexColorTemp = skeletalMeshComponent->LODInfo[skeletalMeshComponent->GetPredictedLODLevel()].OverrideVertexColors->VertexColor(k);

								// If haven't been painted on before then we get the Default Color
								else
									vertexColorTemp = skeletalMeshComponent->GetSkeletalMeshRenderData()->LODRenderData[skeletalMeshComponent->GetPredictedLODLevel()].StaticVertexBuffers.ColorVertexBuffer.VertexColor(k);

								clothColorsTemp.Add(vertexColorTemp);
							}
						}

						FVertexDetectClothSettingsStruct physicsSettingsAtColorTemp = clothsAndTheirPhysicsSettings_Local.FindRef(skelMeshTemp->GetMeshClothingAssets()[i]);

						auto chaosClothPhysicsSettingsTemp = GetChaosClothPhysicsSettingsBasedOnVertexColors(skelMeshTemp->GetMeshClothingAssets()[i], clothColorsTemp, physicsSettingsAtColorTemp, skeletalMeshComponent, useGivenColorAsAvarageColor, avarageColor);

						UVertexPaintFunctionLibrary::SetChaosClothPhysics(skeletalMeshComponent, skelMeshTemp->GetMeshClothingAssets()[i], chaosClothPhysicsSettingsTemp);
					}
				}
			}
		}
	}
}


//-------------------------------------------------------

// Get Chaos Cloth Physics Settings Based On Vertex Colors

FVertexDetectChaosClothPhysicsSettings UVertexPaintFunctionLibrary::GetChaosClothPhysicsSettingsBasedOnVertexColors(UClothingAssetBase* clothingAsset, TArray<FColor> clothColors, FVertexDetectClothSettingsStruct clothPhysicsSettings, USkeletalMeshComponent* skeletalMeshComponent, bool useGivenColorAsAvarageColor, FLinearColor avarageColor) {

	if (!IsValid(skeletalMeshComponent) || !IsValid(clothingAsset)) return FVertexDetectChaosClothPhysicsSettings();


	// https://docs.unrealengine.com/4.27/en-US/API/Plugins/ChaosCloth/ChaosCloth/UChaosClothingInteractor/


	float redAvarageColorValue;
	float greenAvarageColorValue;
	float blueAvarageColorValue;
	float alphaAvarageColorValue;

	// If Setting the entire mesh to be a specific color, then we already know the avarage color and doesn't have to run loops
	if (!useGivenColorAsAvarageColor) {

		FVertexDetectAmountOfPaintedColorsOfEachChannel amountPerChannelTemp = UVertexPaintFunctionLibrary::GetAmountOfPaintedColorsForEachChannel(clothColors, 0.01f);

		redAvarageColorValue = amountPerChannelTemp.averageAmountOfRedColorAtTheMinAmount;
		greenAvarageColorValue = amountPerChannelTemp.averageAmountOfGreenColorAtTheMinAmount;
		blueAvarageColorValue = amountPerChannelTemp.averageAmountOfBlueColorAtTheMinAmount;
		alphaAvarageColorValue = amountPerChannelTemp.averageAmountOfAlphaColorAtTheMinAmount;
	}
	else {

		redAvarageColorValue = avarageColor.R;
		greenAvarageColorValue = avarageColor.G;
		blueAvarageColorValue = avarageColor.B;
		alphaAvarageColorValue = avarageColor.A;
	}


	TArray<float> clothPhysicsSettingsAtColorAvarageValue_Local;
	TArray<FVertexDetectClothPhysicsSettingsAtVertexColorStruct> clothPhysicsSettingsAtColor_Local;


	clothPhysicsSettingsAtColorAvarageValue_Local.Add(redAvarageColorValue);
	clothPhysicsSettingsAtColor_Local.Add(clothPhysicsSettings.clothPhysicsSettingsAtRedChannel);

	clothPhysicsSettingsAtColorAvarageValue_Local.Add(greenAvarageColorValue);
	clothPhysicsSettingsAtColor_Local.Add(clothPhysicsSettings.clothPhysicsSettingsAtGreenChannel);

	clothPhysicsSettingsAtColorAvarageValue_Local.Add(blueAvarageColorValue);
	clothPhysicsSettingsAtColor_Local.Add(clothPhysicsSettings.clothPhysicsSettingsAtBlueChannel);

	clothPhysicsSettingsAtColorAvarageValue_Local.Add(alphaAvarageColorValue);
	clothPhysicsSettingsAtColor_Local.Add(clothPhysicsSettings.clothPhysicsSettingsAtAlphaChannel);



	// NOTE Removed the minRequiredPaintAmount to affect Gravity or damping etc. because it caused an issue when Removing colors and trying to reset the cloth, since it couldn't do anything if not above 0.25 or something

	FVertexDetectChaosClothPhysicsSettings chaosClothPhysicsSettingsLocal;

	// Resets all the settings so we base wind lift etc. all from 0 and not += onto the default values
	chaosClothPhysicsSettingsLocal.ResetAllClothSettings();

	for (int i = 0; i < clothPhysicsSettingsAtColor_Local.Num(); i++) {


		// Damping
		if (clothPhysicsSettingsAtColor_Local[i].SetDamping) {

			chaosClothPhysicsSettingsLocal.SetDamping = true;

			chaosClothPhysicsSettingsLocal.clothDampingSettings.SetDamping_dampingCoefficient += FMath::Lerp(clothPhysicsSettingsAtColor_Local[i].clothDampingSettingsWithNoColorPaintedAtChannel.SetDamping_dampingCoefficient, clothPhysicsSettingsAtColor_Local[i].clothDampingSettingsWithFullColorPaintedAtChannel.SetDamping_dampingCoefficient, clothPhysicsSettingsAtColorAvarageValue_Local[i]);
		}

		// Gravity
		if (clothPhysicsSettingsAtColor_Local[i].SetGravity) {

			chaosClothPhysicsSettingsLocal.SetGravity = true;

			chaosClothPhysicsSettingsLocal.clothGravitySettings.SetGravity_gravityScale += FMath::Lerp(clothPhysicsSettingsAtColor_Local[i].clothGravitySettingsWithNoColorPaintedAtChannel.SetGravity_gravityScale, clothPhysicsSettingsAtColor_Local[i].clothGravitySettingsWithFullColorPaintedAtChannel.SetGravity_gravityScale, clothPhysicsSettingsAtColorAvarageValue_Local[i]);

			chaosClothPhysicsSettingsLocal.clothGravitySettings.SetGravity_gravityOverride += FMath::Lerp(clothPhysicsSettingsAtColor_Local[i].clothGravitySettingsWithNoColorPaintedAtChannel.SetGravity_gravityOverride, clothPhysicsSettingsAtColor_Local[i].clothGravitySettingsWithFullColorPaintedAtChannel.SetGravity_gravityOverride, clothPhysicsSettingsAtColorAvarageValue_Local[i]);

			// If at least 1 color has set it to override gravity then that is what it will do
			if (clothPhysicsSettingsAtColorAvarageValue_Local[i] > 0.5) {

				// If already true then can't set it to false
				if (!chaosClothPhysicsSettingsLocal.clothGravitySettings.SetGravity_overrideGravity)
					chaosClothPhysicsSettingsLocal.clothGravitySettings.SetGravity_overrideGravity = clothPhysicsSettingsAtColor_Local[i].clothGravitySettingsWithFullColorPaintedAtChannel.SetGravity_overrideGravity;
			}

			else {

				if (!chaosClothPhysicsSettingsLocal.clothGravitySettings.SetGravity_overrideGravity)
					chaosClothPhysicsSettingsLocal.clothGravitySettings.SetGravity_overrideGravity = clothPhysicsSettingsAtColor_Local[i].clothGravitySettingsWithNoColorPaintedAtChannel.SetGravity_overrideGravity;
			}
		}

		// Wind
		if (clothPhysicsSettingsAtColor_Local[i].SetWind) {

			chaosClothPhysicsSettingsLocal.SetWind = true;

			chaosClothPhysicsSettingsLocal.clothWindSettings.SetWind_drag += FMath::Lerp(clothPhysicsSettingsAtColor_Local[i].clothWindSettingsWithNoColorPaintedAtChannel.SetWind_drag, clothPhysicsSettingsAtColor_Local[i].clothWindSettingsWithFullColorPaintedAtChannel.SetWind_drag, clothPhysicsSettingsAtColorAvarageValue_Local[i]);

			chaosClothPhysicsSettingsLocal.clothWindSettings.SetWind_lift += FMath::Lerp(clothPhysicsSettingsAtColor_Local[i].clothWindSettingsWithNoColorPaintedAtChannel.SetWind_lift, clothPhysicsSettingsAtColor_Local[i].clothWindSettingsWithFullColorPaintedAtChannel.SetWind_lift, clothPhysicsSettingsAtColorAvarageValue_Local[i]);

			chaosClothPhysicsSettingsLocal.clothWindSettings.SetWind_airDensity += FMath::Lerp(clothPhysicsSettingsAtColor_Local[i].clothWindSettingsWithNoColorPaintedAtChannel.SetWind_airDensity, clothPhysicsSettingsAtColor_Local[i].clothWindSettingsWithFullColorPaintedAtChannel.SetWind_airDensity, clothPhysicsSettingsAtColorAvarageValue_Local[i]);

			chaosClothPhysicsSettingsLocal.clothWindSettings.SetWind_windVelocity += FMath::Lerp(clothPhysicsSettingsAtColor_Local[i].clothWindSettingsWithNoColorPaintedAtChannel.SetWind_windVelocity, clothPhysicsSettingsAtColor_Local[i].clothWindSettingsWithFullColorPaintedAtChannel.SetWind_windVelocity, clothPhysicsSettingsAtColorAvarageValue_Local[i]);
		}

		// Anim Drive
		if (clothPhysicsSettingsAtColor_Local[i].SetAnimDrive) {

			chaosClothPhysicsSettingsLocal.SetAnimDrive = true;

			chaosClothPhysicsSettingsLocal.clothAnimDriveSettings.SetAnimDrive_Stiffness += FMath::Lerp(clothPhysicsSettingsAtColor_Local[i].clothAnimDriveSettingsWithNoColorPaintedAtChannel.SetAnimDrive_Stiffness, clothPhysicsSettingsAtColor_Local[i].clothAnimDriveSettingsWithFullColorPaintedAtChannel.SetAnimDrive_Stiffness, clothPhysicsSettingsAtColorAvarageValue_Local[i]);

			chaosClothPhysicsSettingsLocal.clothAnimDriveSettings.SetAnimDrive_Damping += FMath::Lerp(clothPhysicsSettingsAtColor_Local[i].clothAnimDriveSettingsWithNoColorPaintedAtChannel.SetAnimDrive_Damping, clothPhysicsSettingsAtColor_Local[i].clothAnimDriveSettingsWithFullColorPaintedAtChannel.SetAnimDrive_Damping, clothPhysicsSettingsAtColorAvarageValue_Local[i]);

		}

		// Collision
		if (clothPhysicsSettingsAtColor_Local[i].SetCollision) {

			chaosClothPhysicsSettingsLocal.SetCollision = true;

			chaosClothPhysicsSettingsLocal.clothCollisionSettings.SetCollision_CollisionThickness += FMath::Lerp(clothPhysicsSettingsAtColor_Local[i].clothCollisionSettingsWithNoColorPaintedAtChannel.SetCollision_CollisionThickness, clothPhysicsSettingsAtColor_Local[i].clothCollisionSettingsWithFullColorPaintedAtChannel.SetCollision_CollisionThickness, clothPhysicsSettingsAtColorAvarageValue_Local[i]);

			chaosClothPhysicsSettingsLocal.clothCollisionSettings.SetCollision_FrictionCoefficient += FMath::Lerp(clothPhysicsSettingsAtColor_Local[i].clothCollisionSettingsWithNoColorPaintedAtChannel.SetCollision_FrictionCoefficient, clothPhysicsSettingsAtColor_Local[i].clothCollisionSettingsWithFullColorPaintedAtChannel.SetCollision_FrictionCoefficient, clothPhysicsSettingsAtColorAvarageValue_Local[i]);

			chaosClothPhysicsSettingsLocal.clothCollisionSettings.SetCollision_SelfCollisionThickness += FMath::Lerp(clothPhysicsSettingsAtColor_Local[i].clothCollisionSettingsWithNoColorPaintedAtChannel.SetCollision_SelfCollisionThickness, clothPhysicsSettingsAtColor_Local[i].clothCollisionSettingsWithFullColorPaintedAtChannel.SetCollision_SelfCollisionThickness, clothPhysicsSettingsAtColorAvarageValue_Local[i]);


			// If at least 1 color has set it to use CCD then that is what it will do
			if (clothPhysicsSettingsAtColorAvarageValue_Local[i] > 0.5) {

				if (!chaosClothPhysicsSettingsLocal.clothCollisionSettings.SetCollision_UseCCD)
					chaosClothPhysicsSettingsLocal.clothCollisionSettings.SetCollision_UseCCD = clothPhysicsSettingsAtColor_Local[i].clothCollisionSettingsWithFullColorPaintedAtChannel.SetCollision_UseCCD;
			}

			else {

				if (!chaosClothPhysicsSettingsLocal.clothCollisionSettings.SetCollision_UseCCD)
					chaosClothPhysicsSettingsLocal.clothCollisionSettings.SetCollision_UseCCD = clothPhysicsSettingsAtColor_Local[i].clothCollisionSettingsWithNoColorPaintedAtChannel.SetCollision_UseCCD;
			}
		}

		// Long Range Attachment
		if (clothPhysicsSettingsAtColor_Local[i].SetLongRangeAttachment) {

			chaosClothPhysicsSettingsLocal.SetLongRangeAttachment = true;

			chaosClothPhysicsSettingsLocal.clothLongRangeAttachmentSettings.LongRangeAttachment_TetherThickness += FMath::Lerp(clothPhysicsSettingsAtColor_Local[i].clothLongRangeAttachmentSettingsWithNoColorPaintedAtChannel.LongRangeAttachment_TetherThickness, clothPhysicsSettingsAtColor_Local[i].clothLongRangeAttachmentSettingsWithFullColorPaintedAtChannel.LongRangeAttachment_TetherThickness, clothPhysicsSettingsAtColorAvarageValue_Local[i]);

			chaosClothPhysicsSettingsLocal.clothLongRangeAttachmentSettings.LongRangeAttachment_TetherScale += FMath::Lerp(clothPhysicsSettingsAtColor_Local[i].clothLongRangeAttachmentSettingsWithNoColorPaintedAtChannel.LongRangeAttachment_TetherScale, clothPhysicsSettingsAtColor_Local[i].clothLongRangeAttachmentSettingsWithFullColorPaintedAtChannel.LongRangeAttachment_TetherScale, clothPhysicsSettingsAtColorAvarageValue_Local[i]);
		}

		// Material
		if (clothPhysicsSettingsAtColor_Local[i].SetMaterial) {

			chaosClothPhysicsSettingsLocal.SetMaterial = true;

			chaosClothPhysicsSettingsLocal.clothMaterialSettings.Material_EdgeStiffness += FMath::Lerp(clothPhysicsSettingsAtColor_Local[i].clothMaterialSettingsWithNoColorPaintedAtChannel.Material_EdgeStiffness, clothPhysicsSettingsAtColor_Local[i].clothMaterialSettingsWithFullColorPaintedAtChannel.Material_EdgeStiffness, clothPhysicsSettingsAtColorAvarageValue_Local[i]);

			chaosClothPhysicsSettingsLocal.clothMaterialSettings.Material_BendingStiffness += FMath::Lerp(clothPhysicsSettingsAtColor_Local[i].clothMaterialSettingsWithNoColorPaintedAtChannel.Material_BendingStiffness, clothPhysicsSettingsAtColor_Local[i].clothMaterialSettingsWithFullColorPaintedAtChannel.Material_BendingStiffness, clothPhysicsSettingsAtColorAvarageValue_Local[i]);

			chaosClothPhysicsSettingsLocal.clothMaterialSettings.Material_AreaStiffness += FMath::Lerp(clothPhysicsSettingsAtColor_Local[i].clothMaterialSettingsWithNoColorPaintedAtChannel.Material_AreaStiffness, clothPhysicsSettingsAtColor_Local[i].clothMaterialSettingsWithFullColorPaintedAtChannel.Material_AreaStiffness, clothPhysicsSettingsAtColorAvarageValue_Local[i]);
		}

		// Physics Velocity Scale
		if (clothPhysicsSettingsAtColor_Local[i].SetPhysicsVelocityScale) {

			chaosClothPhysicsSettingsLocal.SetPhysicsVelocityScale = true;

			chaosClothPhysicsSettingsLocal.clothPhysicsVelocityScaleSettings.PhysicsVelocityScale_LinearVelocityScale += FMath::Lerp(clothPhysicsSettingsAtColor_Local[i].clothPhysicsVelocityScaleSettingsWithNoColorPaintedAtChannel.PhysicsVelocityScale_LinearVelocityScale, clothPhysicsSettingsAtColor_Local[i].clothPhysicsVelocityScaleSettingsWithFullColorPaintedAtChannel.PhysicsVelocityScale_LinearVelocityScale, clothPhysicsSettingsAtColorAvarageValue_Local[i]);

			chaosClothPhysicsSettingsLocal.clothPhysicsVelocityScaleSettings.PhysicVelocityScale_AngularVelocityScale += FMath::Lerp(clothPhysicsSettingsAtColor_Local[i].clothPhysicsVelocityScaleSettingsWithNoColorPaintedAtChannel.PhysicVelocityScale_AngularVelocityScale, clothPhysicsSettingsAtColor_Local[i].clothPhysicsVelocityScaleSettingsWithFullColorPaintedAtChannel.PhysicVelocityScale_AngularVelocityScale, clothPhysicsSettingsAtColorAvarageValue_Local[i]);

			chaosClothPhysicsSettingsLocal.clothPhysicsVelocityScaleSettings.PhysicsVelocityScale_FictitiousAngularVelocityScale += FMath::Lerp(clothPhysicsSettingsAtColor_Local[i].clothPhysicsVelocityScaleSettingsWithNoColorPaintedAtChannel.PhysicsVelocityScale_FictitiousAngularVelocityScale, clothPhysicsSettingsAtColor_Local[i].clothPhysicsVelocityScaleSettingsWithFullColorPaintedAtChannel.PhysicsVelocityScale_FictitiousAngularVelocityScale, clothPhysicsSettingsAtColorAvarageValue_Local[i]);
		}

		// Air Pressure
		if (clothPhysicsSettingsAtColor_Local[i].SetAirPressure) {

			chaosClothPhysicsSettingsLocal.SetAirPressure = true;

			chaosClothPhysicsSettingsLocal.clothPhysicssAirPressureSettings.SetPressure_Pressure += FMath::Lerp(clothPhysicsSettingsAtColor_Local[i].clothPhysicssAirPressureWithNoColorPaintedAtChannel.SetPressure_Pressure, clothPhysicsSettingsAtColor_Local[i].clothPhysicssAirPressureWithFullColorPaintedAtChannel.SetPressure_Pressure, clothPhysicsSettingsAtColorAvarageValue_Local[i]);
		}
	}

	return chaosClothPhysicsSettingsLocal;
}



#endif


//-------------------------------------------------------

// Set Chaos Wheeled Vehicle Friction Multiplier - Made this into a wrapper since in UE4 the function doesn't exist so couldn't have it in BP

void UVertexPaintFunctionLibrary::SetChaosWheeledVehicleFrictionMultiplier_Wrapper(UChaosWheeledVehicleMovementComponent* chaosWheeledVehicleMovementComponent, int wheelIndex, float friction) {

#if ENGINE_MAJOR_VERSION == 5

	if (!IsValid(chaosWheeledVehicleMovementComponent) || wheelIndex < 0) return;

	chaosWheeledVehicleMovementComponent->SetWheelFrictionMultiplier(wheelIndex, friction);

#endif

}


//-------------------------------------------------------

// Get Physics Surface as String

FString UVertexPaintFunctionLibrary::GetPhysicsSurfaceAsString(TEnumAsByte<EPhysicalSurface> physicsSurface) {

	if (physicsSurface == EPhysicalSurface::SurfaceType_Default)
		return "Default";

	for (auto physSurfaceNameTemp : GetDefault<UPhysicsSettings>()->PhysicalSurfaces) {

		if (physSurfaceNameTemp.Type == physicsSurface)
			return physSurfaceNameTemp.Name.ToString();
	}

	return FString();
}


//-------------------------------------------------------

// Get Physical Material Using Physics Surface

UPhysicalMaterial* UVertexPaintFunctionLibrary::GetPhysicalMaterialUsingPhysicsSurface_Wrapper(const UObject* WorldContextObject, TSubclassOf<UPhysicalMaterial> physicalMaterialClass, TEnumAsByte<EPhysicalSurface> physicsSurface) {

	if (physicsSurface == EPhysicalSurface::SurfaceType_Default) return nullptr;
	if (!IsValid(WorldContextObject)) return nullptr;
	if (!physicalMaterialClass) return nullptr;


	if (UGameplayStatics::GetGameInstance(WorldContextObject)->GetSubsystem<UVertexPaintDetectionGISubSystem>()) {

		for (auto physicalMaterial : UGameplayStatics::GetGameInstance(WorldContextObject)->GetSubsystem<UVertexPaintDetectionGISubSystem>()->GetAllCachedPhysicsMaterialAssets()) {


			// If set to only check in specific class then makes sure that the physical material is that class
			if (physicalMaterialClass->GetClass() != UPhysicalMaterial::StaticClass()->GetClass()) {

				// If you have custom phys material classes then you can check only through your specific class instead of all of them. 
				if (physicalMaterial->GetClass() == physicalMaterialClass) {

					if (physicalMaterial->SurfaceType == physicsSurface)
						return physicalMaterial;
				}
			}

			else if (physicalMaterial->SurfaceType == physicsSurface) {

				return physicalMaterial;
			}
		}
	}

	return nullptr;
}


//--------------------------------------------------------

// Get All Physics Surfaces

TArray<TEnumAsByte<EPhysicalSurface>> UVertexPaintFunctionLibrary::GetAllPhysicsSurfaces() {

	TArray<TEnumAsByte<EPhysicalSurface>> physicalSurfaces_Local;

	for (auto physSurfaceNameTemp : GetDefault<UPhysicsSettings>()->PhysicalSurfaces)
		physicalSurfaces_Local.Add(physSurfaceNameTemp.Type);

	return physicalSurfaces_Local;
}


#if WITH_EDITOR


//--------------------------------------------------------

// Get Objects Of Class

TArray<UObject*> UVertexPaintFunctionLibrary::VertexPaintDetectionPlugin_GetObjectsOfClass(TSubclassOf<UObject> objectsToGet, bool onlyGetObjectsFromContentFolder) {

	if (!objectsToGet.Get()) return TArray<UObject*>();


	FAssetRegistryModule& AssetRegistryModule_Local = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	TArray<FAssetData> AssetData_Local;
	TArray<UObject*> objects_Local;


#if ENGINE_MAJOR_VERSION == 4

	AssetRegistryModule_Local.Get().GetAssetsByClass(objectsToGet.Get()->GetFName(), AssetData_Local);

#elif ENGINE_MAJOR_VERSION == 5

#if ENGINE_MINOR_VERSION == 0

	AssetRegistryModule_Local.Get().GetAssetsByClass(objectsToGet.Get()->GetFName(), AssetData_Local);


#else

	AssetRegistryModule_Local.Get().GetAssetsByClass(objectsToGet.Get()->GetClassPathName(), AssetData_Local);

#endif
#endif


	for (auto asset_Local : AssetData_Local) {


		// If asset it not in the content folder
		if (!asset_Local.ToSoftObjectPath().ToString().StartsWith("/Game", ESearchCase::IgnoreCase)) {

			if (onlyGetObjectsFromContentFolder)
				continue;
		}

		if (asset_Local.GetAsset())
			objects_Local.Add(asset_Local.GetAsset());
	}

	return objects_Local;
}

//--------------------------------------------------------

// Get Soft Object Path From Soft Object Ptr

FSoftObjectPath UVertexPaintFunctionLibrary::VertexPaintDetectionPlugin_GetSoftObjectPathFromSoftObjectPtr(TSoftObjectPtr<UObject> object) {

	if (!object) return FSoftObjectPath();

	return object.ToSoftObjectPath();
}


//--------------------------------------------------------

// Get Soft Object Name From Soft Object Ptr

FString UVertexPaintFunctionLibrary::VertexPaintDetectionPlugin_GetObjectNameFromSoftObjectPtr(TSoftObjectPtr<UObject> object) {

	if (!object) return FString();

	return object.GetAssetName();
}


//--------------------------------------------------------

// Get Object Of Class as Soft Ptrs

TArray<TSoftObjectPtr<UObject>> UVertexPaintFunctionLibrary::VertexPaintDetectionPlugin_GetObjectsOfClassAsSoftPtrs(TSubclassOf<UObject> objectsToGet, bool loadObjects) {

	if (!objectsToGet.Get()) return TArray<TSoftObjectPtr<UObject>>();


	FAssetRegistryModule& AssetRegistryModule_Local = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	TArray<FAssetData> AssetData_Local;


#if ENGINE_MAJOR_VERSION == 4

	AssetRegistryModule_Local.Get().GetAssetsByClass(objectsToGet.Get()->GetFName(), AssetData_Local);

#elif ENGINE_MAJOR_VERSION == 5

#if ENGINE_MINOR_VERSION == 0

	AssetRegistryModule_Local.Get().GetAssetsByClass(objectsToGet.Get()->GetFName(), AssetData_Local);


#else

	AssetRegistryModule_Local.Get().GetAssetsByClass(objectsToGet.Get()->GetClassPathName(), AssetData_Local);

#endif
#endif


	TArray<TSoftObjectPtr<UObject>> softObjectPtrs_Local;
	TSoftObjectPtr<UObject> tempSoftObject;

	for (auto asset_Local : AssetData_Local) {


		// If asset it not in the content folder
		if (!asset_Local.ToSoftObjectPath().ToString().StartsWith("/Game", ESearchCase::IgnoreCase)) {

			continue;
		}

		tempSoftObject = asset_Local.ToSoftObjectPath();


		if (loadObjects)
			tempSoftObject.LoadSynchronous();

		if (!tempSoftObject) continue;

		softObjectPtrs_Local.Add(tempSoftObject);
	}

	return softObjectPtrs_Local;
}


#endif


//-------------------------------------------------------

// Fill Calculate Colors Info Fundementals

void UVertexPaintFunctionLibrary::FillCalculateColorsInfoFundementals(const FVertexDetectFundementalStruct& fundementalStruct, FCalculateColorsInfo& calculateColorsInfo) {

	if (!IsValid(fundementalStruct.meshComponent)) return;


	UObject* sourceMeshTemp = nullptr;
	UStaticMeshComponent* staticMeshComponentTemp = nullptr;
	USplineMeshComponent* splineMeshComponentTemp = nullptr;
	USkeletalMeshComponent* skeletalMeshComponentTemp = nullptr;

	staticMeshComponentTemp = Cast<UStaticMeshComponent>(fundementalStruct.meshComponent);

	if (staticMeshComponentTemp) {

		sourceMeshTemp = staticMeshComponentTemp->GetStaticMesh();
		splineMeshComponentTemp = Cast<USplineMeshComponent>(fundementalStruct.meshComponent);
	}

	else {

		skeletalMeshComponentTemp = Cast<USkeletalMeshComponent>(fundementalStruct.meshComponent);

		if (skeletalMeshComponentTemp) {

#if ENGINE_MAJOR_VERSION == 4

			sourceMeshTemp = skeletalMeshComponentTemp->SkeletalMesh;

#elif ENGINE_MAJOR_VERSION == 5

#if ENGINE_MINOR_VERSION == 0

			sourceMeshTemp = skeletalMeshComponentTemp->SkeletalMesh.Get();

#else

			sourceMeshTemp = skeletalMeshComponentTemp->GetSkeletalMeshAsset();

#endif
#endif
		}
	}



	// If paint job
	if (calculateColorsInfo.vertexPaintSettings.actor) {

		calculateColorsInfo.lodsToLoopThrough = UVertexPaintFunctionLibrary::GetAmountOfLODsToPaintOn(fundementalStruct.meshComponent, calculateColorsInfo.vertexPaintSettings.overrideLOD.overrideLODToPaintUpTo, calculateColorsInfo.vertexPaintSettings.overrideLOD.amountOfLODsToPaint);
	}

	// If just Detecting and set to only include vertex data for lod 0 in the callback, there's no reason to loop through more LODs, unlike painting where even you may still need to paint on all LODs eventhough you only want LOD0 in the callback. 
	else if (calculateColorsInfo.detectInfoSettings.actor) {

		if (calculateColorsInfo.detectInfoSettings.callbackSettings.includeVertexDataOnlyForLOD0)
			calculateColorsInfo.lodsToLoopThrough = 1;
	}


	calculateColorsInfo.meshVertexData.meshComp = fundementalStruct.meshComponent;
	calculateColorsInfo.meshVertexData.meshSource = sourceMeshTemp;
	calculateColorsInfo.meshVertexData.meshDataPerLOD.SetNum(calculateColorsInfo.lodsToLoopThrough, true);

	for (int i = 0; i < calculateColorsInfo.meshVertexData.meshDataPerLOD.Num(); i++)
		calculateColorsInfo.meshVertexData.meshDataPerLOD[i].lod = i;

	calculateColorsInfo.vertexPaintDetectionHitActor = fundementalStruct.meshComponent->GetOwner();
	calculateColorsInfo.vertexPaintComponent = fundementalStruct.meshComponent;
	calculateColorsInfo.vertexPaintStaticMeshComponent = staticMeshComponentTemp;
	calculateColorsInfo.vertexPaintSplineMeshComponent = splineMeshComponentTemp;
	calculateColorsInfo.vertexPaintSkelComponent = skeletalMeshComponentTemp;
	calculateColorsInfo.vertexPaintSourceMesh = sourceMeshTemp;
	calculateColorsInfo.fundementalStruct = fundementalStruct;
	calculateColorsInfo.worldTaskWasCreatedIn = calculateColorsInfo.vertexPaintDetectionHitActor->GetWorld(); // Had to cache the world as well in case component or actor wasn't valid when the task was finished, but we needed to check if the world was valid and start reset timers etc. 


	// If set to compare mesh colors to a color array, either serialized och TArray, one of them has to be set, otherwise we set it to false so we don't loop through vertices for no reason if for instance a color snippet and this was true. 
	if (calculateColorsInfo.fundementalStruct.callbackSettings.compareMeshColorsToColorArray.compareMeshColorsToColorArrayAtLOD0 && calculateColorsInfo.fundementalStruct.callbackSettings.compareMeshColorsToColorArray.colorArrayToCompareWith.Num() <= 0) {

		calculateColorsInfo.fundementalStruct.callbackSettings.compareMeshColorsToColorArray.compareMeshColorsToColorArrayAtLOD0 = false;
	}


	// Paint Color Snippets, Set Mesh Component Verte Colors regularly or with Serialized String only use LOD0 data, so for them we always propogate to other LODs. Paint Randomly over the Entire Mesh however has the option to do this since you might want to randomize on each LOD to save performance
	if (calculateColorsInfo.paintOnColorSnippetSettings.actor || calculateColorsInfo.setMeshComponentVertexColorsSettings.actor || calculateColorsInfo.setMeshComponentVertexColorsUsingSerializedStringSettings.actor || (calculateColorsInfo.paintOnEntireMeshSettings.actor && calculateColorsInfo.paintOnEntireMeshSettings.paintOnRandomVerticesSettings.paintAtRandomVerticesSpreadOutOverTheEntireMesh && calculateColorsInfo.paintOnEntireMeshSettings.paintOnRandomVerticesSettings.paintAtRandomVerticesSpreadOutOverTheEntireMesh_PropogateLOD0ToAllLODsMethod == Enum_PaintEntireMeshAtRandomVertices_PropogateToLODsMethod::ModifiedEngineMethod)) {

		if (calculateColorsInfo.lodsToLoopThrough > 1)
			calculateColorsInfo.propogateLOD0ToAllLODs = true;
	}


	if (fundementalStruct.multiThreadSettings.useMultithreadingForCalculations) {

		if (calculateColorsInfo.drawVertexPositionDebugPoint || calculateColorsInfo.drawVertexPositionDebugPointOnVertexThatGotPaintApplied || calculateColorsInfo.drawClothVertexPositionDebugPoint || calculateColorsInfo.drawVertexNormalDebugArrow) {

			if (calculateColorsInfo.printLogsToScreen || calculateColorsInfo.printLogsToOutputLog)
				UKismetSystemLibrary::PrintString(fundementalStruct.meshComponent->GetWorld(), FString::Printf(TEXT("Runtime Vertex Color Paint and Detection Plugin - Paint/Detect Task is set to Draw Debug Symbols at Vertices but it only works if not Multithreading, i.e. on the Game Thread.")), calculateColorsInfo.printLogsToScreen, calculateColorsInfo.printLogsToOutputLog, FColor::Orange, calculateColorsInfo.printLogsToScreen_Duration);
		}
	}


	// If paint job where you set strength of colors to Apply and can have paint conditions for them
	if (calculateColorsInfo.paintOnMeshColorSettings.actor) {

		// If not painting using physics surface but the regular way, then if Add, and not 0 then that channel we will apply colors to (eventhough there may not be a difference but we will try), if Set then all channels will get applied to. 
		if (!calculateColorsInfo.paintOnMeshColorSettings.applyVertexColorSettings.applyColorsUsingPhysicsSurface.applyVertexColorUsingPhysicsSurface) {


			if (calculateColorsInfo.paintOnMeshColorSettings.applyVertexColorSettings.applyVertexColorSetting == EApplyVertexColorSetting::ESetVertexColor || calculateColorsInfo.paintOnColorSnippetSettings.actor || calculateColorsInfo.paintDetectionType == EVertexPaintDetectionType::ApplyVertexDataDirectly) {

				calculateColorsInfo.appliedToVertexColorChannels.Add(EVertexColorChannel::RedChannel);
				calculateColorsInfo.appliedToVertexColorChannels.Add(EVertexColorChannel::GreenChannel);
				calculateColorsInfo.appliedToVertexColorChannels.Add(EVertexColorChannel::BlueChannel);
				calculateColorsInfo.appliedToVertexColorChannels.Add(EVertexColorChannel::AlphaChannel);
			}

			else if (calculateColorsInfo.paintOnMeshColorSettings.applyVertexColorSettings.applyVertexColorSetting == EApplyVertexColorSetting::EAddVertexColor) {

				if (calculateColorsInfo.paintOnMeshColorSettings.applyVertexColorSettings.redColor != 0)
					calculateColorsInfo.appliedToVertexColorChannels.Add(EVertexColorChannel::RedChannel);

				if (calculateColorsInfo.paintOnMeshColorSettings.applyVertexColorSettings.greenColor != 0)
					calculateColorsInfo.appliedToVertexColorChannels.Add(EVertexColorChannel::GreenChannel);

				if (calculateColorsInfo.paintOnMeshColorSettings.applyVertexColorSettings.blueColor != 0)
					calculateColorsInfo.appliedToVertexColorChannels.Add(EVertexColorChannel::BlueChannel);

				if (calculateColorsInfo.paintOnMeshColorSettings.applyVertexColorSettings.alphaColor != 0)
					calculateColorsInfo.appliedToVertexColorChannels.Add(EVertexColorChannel::AlphaChannel);
			}
		}


		// Gets the Actor Location for only affect vertices with direction if it has been set. 
		if (calculateColorsInfo.paintOnMeshColorSettings.onlyAffectVerticesWithDirectionToLocation.onlyAffectVerticesWithDirectionToActorOrLocation) {

			if (IsValid(calculateColorsInfo.paintOnMeshColorSettings.onlyAffectVerticesWithDirectionToLocation.actor)) {

				calculateColorsInfo.paintOnMeshColorSettings.onlyAffectVerticesWithDirectionToLocation.location = calculateColorsInfo.paintOnMeshColorSettings.onlyAffectVerticesWithDirectionToLocation.actor->GetActorLocation();
			}
		}


		// Gets Actor Location for Line of Sight Paint Condition if the actor has been set
		for (int i = 0; i < calculateColorsInfo.paintOnMeshColorSettings.applyVertexColorSettings.redColorCondition.ifVertexHasLineOfSightTo.Num(); i++) {

			if (IsValid(calculateColorsInfo.paintOnMeshColorSettings.applyVertexColorSettings.redColorCondition.ifVertexHasLineOfSightTo[i].ifVertexHasLineOfSightToActor)) {

				calculateColorsInfo.paintOnMeshColorSettings.applyVertexColorSettings.redColorCondition.ifVertexHasLineOfSightTo[i].ifVertexHasLineOfSightToPosition = calculateColorsInfo.paintOnMeshColorSettings.applyVertexColorSettings.redColorCondition.ifVertexHasLineOfSightTo[i].ifVertexHasLineOfSightToActor->GetActorLocation();
			}
		}

		for (int i = 0; i < calculateColorsInfo.paintOnMeshColorSettings.applyVertexColorSettings.greenColorCondition.ifVertexHasLineOfSightTo.Num(); i++) {

			if (IsValid(calculateColorsInfo.paintOnMeshColorSettings.applyVertexColorSettings.greenColorCondition.ifVertexHasLineOfSightTo[i].ifVertexHasLineOfSightToActor)) {

				calculateColorsInfo.paintOnMeshColorSettings.applyVertexColorSettings.greenColorCondition.ifVertexHasLineOfSightTo[i].ifVertexHasLineOfSightToPosition = calculateColorsInfo.paintOnMeshColorSettings.applyVertexColorSettings.greenColorCondition.ifVertexHasLineOfSightTo[i].ifVertexHasLineOfSightToActor->GetActorLocation();
			}
		}

		for (int i = 0; i < calculateColorsInfo.paintOnMeshColorSettings.applyVertexColorSettings.blueColorCondition.ifVertexHasLineOfSightTo.Num(); i++) {

			if (IsValid(calculateColorsInfo.paintOnMeshColorSettings.applyVertexColorSettings.blueColorCondition.ifVertexHasLineOfSightTo[i].ifVertexHasLineOfSightToActor)) {

				calculateColorsInfo.paintOnMeshColorSettings.applyVertexColorSettings.blueColorCondition.ifVertexHasLineOfSightTo[i].ifVertexHasLineOfSightToPosition = calculateColorsInfo.paintOnMeshColorSettings.applyVertexColorSettings.blueColorCondition.ifVertexHasLineOfSightTo[i].ifVertexHasLineOfSightToActor->GetActorLocation();
			}
		}

		for (int i = 0; i < calculateColorsInfo.paintOnMeshColorSettings.applyVertexColorSettings.alphaColorCondition.ifVertexHasLineOfSightTo.Num(); i++) {

			if (IsValid(calculateColorsInfo.paintOnMeshColorSettings.applyVertexColorSettings.alphaColorCondition.ifVertexHasLineOfSightTo[i].ifVertexHasLineOfSightToActor)) {

				calculateColorsInfo.paintOnMeshColorSettings.applyVertexColorSettings.alphaColorCondition.ifVertexHasLineOfSightTo[i].ifVertexHasLineOfSightToPosition = calculateColorsInfo.paintOnMeshColorSettings.applyVertexColorSettings.alphaColorCondition.ifVertexHasLineOfSightTo[i].ifVertexHasLineOfSightToActor->GetActorLocation();
			}
		}

		for (int i = 0; i < calculateColorsInfo.paintOnMeshColorSettings.applyVertexColorSettings.applyColorsUsingPhysicsSurface.paintUsingPhysicsSurfaceConditions.ifVertexHasLineOfSightTo.Num(); i++) {

			if (IsValid(calculateColorsInfo.paintOnMeshColorSettings.applyVertexColorSettings.applyColorsUsingPhysicsSurface.paintUsingPhysicsSurfaceConditions.ifVertexHasLineOfSightTo[i].ifVertexHasLineOfSightToActor)) {

				calculateColorsInfo.paintOnMeshColorSettings.applyVertexColorSettings.applyColorsUsingPhysicsSurface.paintUsingPhysicsSurfaceConditions.ifVertexHasLineOfSightTo[i].ifVertexHasLineOfSightToPosition = calculateColorsInfo.paintOnMeshColorSettings.applyVertexColorSettings.applyColorsUsingPhysicsSurface.paintUsingPhysicsSurfaceConditions.ifVertexHasLineOfSightTo[i].ifVertexHasLineOfSightToActor->GetActorLocation();
			}
		}

		for (int i = 0; i < calculateColorsInfo.paintOnMeshColorSettings.applyVertexColorSettings.applyColorsUsingPhysicsSurface.paintConditionsOnChannelsWithoutThePhysicsSurface.ifVertexHasLineOfSightTo.Num(); i++) {

			if (IsValid(calculateColorsInfo.paintOnMeshColorSettings.applyVertexColorSettings.applyColorsUsingPhysicsSurface.paintConditionsOnChannelsWithoutThePhysicsSurface.ifVertexHasLineOfSightTo[i].ifVertexHasLineOfSightToActor)) {

				calculateColorsInfo.paintOnMeshColorSettings.applyVertexColorSettings.applyColorsUsingPhysicsSurface.paintConditionsOnChannelsWithoutThePhysicsSurface.ifVertexHasLineOfSightTo[i].ifVertexHasLineOfSightToPosition = calculateColorsInfo.paintOnMeshColorSettings.applyVertexColorSettings.applyColorsUsingPhysicsSurface.paintConditionsOnChannelsWithoutThePhysicsSurface.ifVertexHasLineOfSightTo[i].ifVertexHasLineOfSightToActor->GetActorLocation();
			}
		}


		// Updates the invidual struct settings as well so they match the paintOnMeshColorSettings, in case we for some reason try to access the condition things we've set above through them. Shouldn't be necessary i think but just in case 
		switch (calculateColorsInfo.paintDetectionType) {

		case EVertexPaintDetectionType::ApplyVertexDataDirectly:
			break;

		case EVertexPaintDetectionType::Detection:
			break;

		case EVertexPaintDetectionType::PaintAtLocation:
			calculateColorsInfo.paintOnMeshAtLocationSettings.applyVertexColorSettings = calculateColorsInfo.paintOnMeshColorSettings.applyVertexColorSettings;
			calculateColorsInfo.paintOnMeshAtLocationSettings.onlyAffectVerticesWithDirectionToLocation = calculateColorsInfo.paintOnMeshColorSettings.onlyAffectVerticesWithDirectionToLocation;
			break;

		case EVertexPaintDetectionType::PaintWithinArea:
			calculateColorsInfo.paintOnMeshWithinAreaSettings.applyVertexColorSettings = calculateColorsInfo.paintOnMeshColorSettings.applyVertexColorSettings;
			calculateColorsInfo.paintOnMeshWithinAreaSettings.onlyAffectVerticesWithDirectionToLocation = calculateColorsInfo.paintOnMeshColorSettings.onlyAffectVerticesWithDirectionToLocation;
			break;

		case EVertexPaintDetectionType::PaintEntireMesh:
			calculateColorsInfo.paintOnEntireMeshSettings.applyVertexColorSettings = calculateColorsInfo.paintOnMeshColorSettings.applyVertexColorSettings;
			calculateColorsInfo.paintOnEntireMeshSettings.onlyAffectVerticesWithDirectionToLocation = calculateColorsInfo.paintOnMeshColorSettings.onlyAffectVerticesWithDirectionToLocation;
			break;

		case EVertexPaintDetectionType::PaintColorSnippet:
			break;

		default:
			break;
		}

	}
}


//-------------------------------------------------------

// Callbacks

void UVertexPaintFunctionLibrary::RunGetClosestVertexDataCallbacks(const FCalculateColorsInfo& calculateColorsInfo, const FVertexDetectClosestVertexDataResultStruct& closestVertexColorResult, const FVertexDetectEstimatedColorAtHitLocationResultStruct& estimatedColorAtHitLocationResult, const FVertexDetectAvarageColorInAreaInfo& avarageColor) {

	if (IsValid(calculateColorsInfo.initiatedByComponent)) {

		calculateColorsInfo.initiatedByComponent->GetClosestVertexDataOnMeshTaskFinished();

		if (calculateColorsInfo.detectInfoSettings.callbackSettings.runCallbackDelegate)
			calculateColorsInfo.initiatedByComponent->getClosestVertexDataDelegate.Broadcast(calculateColorsInfo.taskResult, calculateColorsInfo.meshVertexData, calculateColorsInfo.detectInfoSettings, closestVertexColorResult, estimatedColorAtHitLocationResult, avarageColor, calculateColorsInfo.amountOfPaintedColorsOfEachChannelAtMinAmount_BeforeApplyingColor, calculateColorsInfo.vertexColorsOnEachBone, calculateColorsInfo.additionalDataToPassThrough);
	}

	if (IsValid(calculateColorsInfo.vertexPaintDetectionHitActor)) {

		if (calculateColorsInfo.detectInfoSettings.callbackSettings.runCallbackInterfaces && calculateColorsInfo.vertexPaintDetectionHitActor->GetClass()->ImplementsInterface(UVertexPaintDetectionInterface::StaticClass()))
			IVertexPaintDetectionInterface::Execute_GetClosestVertexDataOnActor(calculateColorsInfo.detectInfoSettings.actor, calculateColorsInfo.taskResult, calculateColorsInfo.meshVertexData, calculateColorsInfo.detectInfoSettings, closestVertexColorResult, estimatedColorAtHitLocationResult, avarageColor, calculateColorsInfo.amountOfPaintedColorsOfEachChannelAtMinAmount_BeforeApplyingColor, calculateColorsInfo.vertexColorsOnEachBone, calculateColorsInfo.additionalDataToPassThrough);
	}
}

void UVertexPaintFunctionLibrary::RunGetAllColorsOnlyCallbacks(const FCalculateColorsInfo& calculateColorsInfo) {


	if (IsValid(calculateColorsInfo.initiatedByComponent)) {

		calculateColorsInfo.initiatedByComponent->GetAllVertexColorsOnlyTaskFinished();

		if (calculateColorsInfo.detectInfoSettings.callbackSettings.runCallbackDelegate)
			calculateColorsInfo.initiatedByComponent->getAllVertexColorsOnlyDelegate.Broadcast(calculateColorsInfo.taskResult, calculateColorsInfo.meshVertexData, calculateColorsInfo.getAllVertexColorsSettings, calculateColorsInfo.amountOfPaintedColorsOfEachChannelAtMinAmount_BeforeApplyingColor, calculateColorsInfo.vertexColorsOnEachBone, calculateColorsInfo.additionalDataToPassThrough);
	}


	if (IsValid(calculateColorsInfo.vertexPaintDetectionHitActor)) {

		if (calculateColorsInfo.detectInfoSettings.callbackSettings.runCallbackInterfaces && calculateColorsInfo.vertexPaintDetectionHitActor->GetClass()->ImplementsInterface(UVertexPaintDetectionInterface::StaticClass()))
			IVertexPaintDetectionInterface::Execute_GetAllVertexColorsOnlyOnActor(calculateColorsInfo.detectInfoSettings.actor, calculateColorsInfo.taskResult, calculateColorsInfo.meshVertexData, calculateColorsInfo.getAllVertexColorsSettings, calculateColorsInfo.amountOfPaintedColorsOfEachChannelAtMinAmount_BeforeApplyingColor, calculateColorsInfo.vertexColorsOnEachBone, calculateColorsInfo.additionalDataToPassThrough);
	}
}

void UVertexPaintFunctionLibrary::RunPaintAtLocationCallbacks(const FCalculateColorsInfo& calculateColorsInfo, const FVertexDetectClosestVertexDataResultStruct& closestVertexColorResult, const FVertexDetectEstimatedColorAtHitLocationResultStruct& estimatedColorAtHitLocationResult, const FVertexDetectAvarageColorInAreaInfo& avarageColor) {


	if (IsValid(calculateColorsInfo.initiatedByComponent)) {

		calculateColorsInfo.initiatedByComponent->PaintOnMeshAtLocationTaskFinished();

		if (calculateColorsInfo.fundementalStruct.callbackSettings.runCallbackDelegate)
			calculateColorsInfo.initiatedByComponent->vertexColorsPaintedAtLocationDelegate.Broadcast(calculateColorsInfo.taskResult, calculateColorsInfo.meshVertexData, calculateColorsInfo.paintOnMeshAtLocationSettings, closestVertexColorResult, estimatedColorAtHitLocationResult, avarageColor, calculateColorsInfo.amountOfPaintedColorsOfEachChannelAtMinAmount_AfterApplyingColor, calculateColorsInfo.vertexColorsOnEachBone, calculateColorsInfo.additionalDataToPassThrough);
	}


	if (IsValid(calculateColorsInfo.vertexPaintDetectionHitActor)) {

		if (calculateColorsInfo.fundementalStruct.callbackSettings.runCallbackInterfaces && calculateColorsInfo.vertexPaintDetectionHitActor->GetClass()->ImplementsInterface(UVertexPaintDetectionInterface::StaticClass()))
			IVertexPaintDetectionInterface::Execute_PaintedOnActor_AtLocation(calculateColorsInfo.paintOnMeshAtLocationSettings.actor, calculateColorsInfo.taskResult, calculateColorsInfo.meshVertexData, calculateColorsInfo.paintOnMeshAtLocationSettings, closestVertexColorResult, estimatedColorAtHitLocationResult, avarageColor, calculateColorsInfo.amountOfPaintedColorsOfEachChannelAtMinAmount_AfterApplyingColor, calculateColorsInfo.vertexColorsOnEachBone, calculateColorsInfo.additionalDataToPassThrough);

		// Calls interface on the painted actor so they can run custom things if they get painted on
		if (calculateColorsInfo.fundementalStruct.callbackSettings.runCallbackInterfaces && calculateColorsInfo.vertexPaintDetectionHitActor->GetClass()->ImplementsInterface(UVertexPaintDetectionInterface::StaticClass()))
			IVertexPaintDetectionInterface::Execute_ColorsAppliedOnActor(calculateColorsInfo.vertexPaintDetectionHitActor, calculateColorsInfo.taskResult, calculateColorsInfo.meshVertexData, calculateColorsInfo.paintOnMeshAtLocationSettings, calculateColorsInfo.amountOfPaintedColorsOfEachChannelAtMinAmount_AfterApplyingColor, calculateColorsInfo.vertexColorsOnEachBone, calculateColorsInfo.additionalDataToPassThrough);
	}
}

void UVertexPaintFunctionLibrary::RunPaintWithinAreaCallbacks(const FCalculateColorsInfo& calculateColorsInfo) {


	if (IsValid(calculateColorsInfo.initiatedByComponent)) {

		calculateColorsInfo.initiatedByComponent->PaintOnMeshWithinAreaTaskFinished();

		if (calculateColorsInfo.fundementalStruct.callbackSettings.runCallbackDelegate)
			calculateColorsInfo.initiatedByComponent->vertexColorsPaintedMeshWithinAreaDelegate.Broadcast(calculateColorsInfo.taskResult, calculateColorsInfo.meshVertexData, calculateColorsInfo.paintOnMeshWithinAreaSettings, calculateColorsInfo.amountOfPaintedColorsOfEachChannelAtMinAmount_AfterApplyingColor, calculateColorsInfo.vertexColorsOnEachBone, calculateColorsInfo.additionalDataToPassThrough);
	}


	if (IsValid(calculateColorsInfo.vertexPaintDetectionHitActor)) {

		if (calculateColorsInfo.fundementalStruct.callbackSettings.runCallbackInterfaces && calculateColorsInfo.vertexPaintDetectionHitActor->GetClass()->ImplementsInterface(UVertexPaintDetectionInterface::StaticClass()))
			IVertexPaintDetectionInterface::Execute_PaintedOnActor_WithinArea(calculateColorsInfo.paintOnMeshWithinAreaSettings.actor, calculateColorsInfo.taskResult, calculateColorsInfo.meshVertexData, calculateColorsInfo.paintOnMeshWithinAreaSettings, calculateColorsInfo.amountOfPaintedColorsOfEachChannelAtMinAmount_AfterApplyingColor, calculateColorsInfo.vertexColorsOnEachBone, calculateColorsInfo.additionalDataToPassThrough);

		// Calls interface on the painted actor so they can run custom things if they get painted on
		if (calculateColorsInfo.fundementalStruct.callbackSettings.runCallbackInterfaces && calculateColorsInfo.vertexPaintDetectionHitActor->GetClass()->ImplementsInterface(UVertexPaintDetectionInterface::StaticClass()))
			IVertexPaintDetectionInterface::Execute_ColorsAppliedOnActor(calculateColorsInfo.vertexPaintDetectionHitActor, calculateColorsInfo.taskResult, calculateColorsInfo.meshVertexData, calculateColorsInfo.paintOnMeshWithinAreaSettings, calculateColorsInfo.amountOfPaintedColorsOfEachChannelAtMinAmount_AfterApplyingColor, calculateColorsInfo.vertexColorsOnEachBone, calculateColorsInfo.additionalDataToPassThrough);
	}
}

void UVertexPaintFunctionLibrary::RunPaintEntireMeshCallbacks(const FCalculateColorsInfo& calculateColorsInfo) {


	if (IsValid(calculateColorsInfo.initiatedByComponent)) {

		calculateColorsInfo.initiatedByComponent->PaintOnEntireMeshTaskFinished();

		if (calculateColorsInfo.fundementalStruct.callbackSettings.runCallbackDelegate)
			calculateColorsInfo.initiatedByComponent->vertexColorsPaintedEntireMeshDelegate.Broadcast(calculateColorsInfo.taskResult, calculateColorsInfo.meshVertexData, calculateColorsInfo.paintOnEntireMeshSettings, calculateColorsInfo.amountOfPaintedColorsOfEachChannelAtMinAmount_AfterApplyingColor, calculateColorsInfo.vertexColorsOnEachBone, calculateColorsInfo.additionalDataToPassThrough);
	}


	if (IsValid(calculateColorsInfo.vertexPaintDetectionHitActor)) {

		if (calculateColorsInfo.fundementalStruct.callbackSettings.runCallbackInterfaces && calculateColorsInfo.vertexPaintDetectionHitActor->GetClass()->ImplementsInterface(UVertexPaintDetectionInterface::StaticClass()))
			IVertexPaintDetectionInterface::Execute_PaintedOnActor_EntireMesh(calculateColorsInfo.paintOnEntireMeshSettings.actor, calculateColorsInfo.taskResult, calculateColorsInfo.meshVertexData, calculateColorsInfo.paintOnEntireMeshSettings, calculateColorsInfo.amountOfPaintedColorsOfEachChannelAtMinAmount_AfterApplyingColor, calculateColorsInfo.vertexColorsOnEachBone, calculateColorsInfo.additionalDataToPassThrough);

		// Calls interface on the painted actor so they can run custom things if they get painted on
		if (calculateColorsInfo.fundementalStruct.callbackSettings.runCallbackInterfaces && calculateColorsInfo.vertexPaintDetectionHitActor->GetClass()->ImplementsInterface(UVertexPaintDetectionInterface::StaticClass()))
			IVertexPaintDetectionInterface::Execute_ColorsAppliedOnActor(calculateColorsInfo.vertexPaintDetectionHitActor, calculateColorsInfo.taskResult, calculateColorsInfo.meshVertexData, calculateColorsInfo.paintOnEntireMeshSettings, calculateColorsInfo.amountOfPaintedColorsOfEachChannelAtMinAmount_AfterApplyingColor, calculateColorsInfo.vertexColorsOnEachBone, calculateColorsInfo.additionalDataToPassThrough);
	}
}

void UVertexPaintFunctionLibrary::RunPaintColorSnippetCallbacks(const FCalculateColorsInfo& calculateColorsInfo) {


	if (IsValid(calculateColorsInfo.initiatedByComponent)) {

		calculateColorsInfo.initiatedByComponent->PaintColorSnippetOnMeshTaskFinished();

		if (calculateColorsInfo.fundementalStruct.callbackSettings.runCallbackDelegate)
			calculateColorsInfo.initiatedByComponent->vertexColorsPaintColorSnippetDelegate.Broadcast(calculateColorsInfo.taskResult, calculateColorsInfo.meshVertexData, calculateColorsInfo.paintOnColorSnippetSettings, calculateColorsInfo.amountOfPaintedColorsOfEachChannelAtMinAmount_AfterApplyingColor, calculateColorsInfo.vertexColorsOnEachBone, calculateColorsInfo.additionalDataToPassThrough);
	}

	if (IsValid(calculateColorsInfo.vertexPaintDetectionHitActor)) {

		// Calls interface on the painted actor so they can run custom things if they get painted on
		if (calculateColorsInfo.fundementalStruct.callbackSettings.runCallbackInterfaces && calculateColorsInfo.vertexPaintDetectionHitActor->GetClass()->ImplementsInterface(UVertexPaintDetectionInterface::StaticClass()))
			IVertexPaintDetectionInterface::Execute_PaintedOnActor_PaintColorSnippet(calculateColorsInfo.paintOnColorSnippetSettings.actor, calculateColorsInfo.taskResult, calculateColorsInfo.meshVertexData, calculateColorsInfo.paintOnColorSnippetSettings, calculateColorsInfo.amountOfPaintedColorsOfEachChannelAtMinAmount_AfterApplyingColor, calculateColorsInfo.vertexColorsOnEachBone, calculateColorsInfo.additionalDataToPassThrough);

		// Calls interface on the painted actor so they can run custom things if they get painted on
		if (calculateColorsInfo.fundementalStruct.callbackSettings.runCallbackInterfaces && calculateColorsInfo.vertexPaintDetectionHitActor->GetClass()->ImplementsInterface(UVertexPaintDetectionInterface::StaticClass()))
			IVertexPaintDetectionInterface::Execute_ColorsAppliedOnActor(calculateColorsInfo.vertexPaintDetectionHitActor, calculateColorsInfo.taskResult, calculateColorsInfo.meshVertexData, calculateColorsInfo.paintOnColorSnippetSettings, calculateColorsInfo.amountOfPaintedColorsOfEachChannelAtMinAmount_AfterApplyingColor, calculateColorsInfo.vertexColorsOnEachBone, calculateColorsInfo.additionalDataToPassThrough);
	}
}

void UVertexPaintFunctionLibrary::RunPaintSetMeshColorsCallbacks(const FCalculateColorsInfo& calculateColorsInfo) {


	if (IsValid(calculateColorsInfo.initiatedByComponent)) {

		if (calculateColorsInfo.fundementalStruct.callbackSettings.runCallbackDelegate)
			calculateColorsInfo.initiatedByComponent->vertexColorsSetMeshColorsDelegate.Broadcast(calculateColorsInfo.taskResult, calculateColorsInfo.meshVertexData, calculateColorsInfo.setMeshComponentVertexColorsSettings, calculateColorsInfo.amountOfPaintedColorsOfEachChannelAtMinAmount_AfterApplyingColor, calculateColorsInfo.vertexColorsOnEachBone, calculateColorsInfo.additionalDataToPassThrough);
	}

	if (IsValid(calculateColorsInfo.vertexPaintDetectionHitActor)) {

		// Calls interface on the painted actor so they can run custom things if they get painted on
		if (calculateColorsInfo.fundementalStruct.callbackSettings.runCallbackInterfaces && calculateColorsInfo.vertexPaintDetectionHitActor->GetClass()->ImplementsInterface(UVertexPaintDetectionInterface::StaticClass()))
			IVertexPaintDetectionInterface::Execute_ColorsAppliedOnActor(calculateColorsInfo.vertexPaintDetectionHitActor, calculateColorsInfo.taskResult, calculateColorsInfo.meshVertexData, calculateColorsInfo.setMeshComponentVertexColorsSettings, calculateColorsInfo.amountOfPaintedColorsOfEachChannelAtMinAmount_AfterApplyingColor, calculateColorsInfo.vertexColorsOnEachBone, calculateColorsInfo.additionalDataToPassThrough);
	}
}

void UVertexPaintFunctionLibrary::RunPaintSetMeshColorsUsingSerializedStringCallbacks(const FCalculateColorsInfo& calculateColorsInfo) {


	if (IsValid(calculateColorsInfo.initiatedByComponent)) {

		if (calculateColorsInfo.setMeshComponentVertexColorsUsingSerializedStringSettings.callbackSettings.runCallbackDelegate)
			calculateColorsInfo.initiatedByComponent->vertexColorsSetMeshColorsUsingSerializedStringDelegate.Broadcast(calculateColorsInfo.taskResult, calculateColorsInfo.meshVertexData, calculateColorsInfo.setMeshComponentVertexColorsUsingSerializedStringSettings, calculateColorsInfo.amountOfPaintedColorsOfEachChannelAtMinAmount_AfterApplyingColor, calculateColorsInfo.vertexColorsOnEachBone, calculateColorsInfo.additionalDataToPassThrough);
	}


	if (IsValid(calculateColorsInfo.vertexPaintDetectionHitActor)) {

		// Calls interface on the painted actor so they can run custom things if they get painted on
		if (calculateColorsInfo.setMeshComponentVertexColorsUsingSerializedStringSettings.callbackSettings.runCallbackInterfaces && calculateColorsInfo.vertexPaintDetectionHitActor->GetClass()->ImplementsInterface(UVertexPaintDetectionInterface::StaticClass()))
			IVertexPaintDetectionInterface::Execute_ColorsAppliedOnActor(calculateColorsInfo.vertexPaintDetectionHitActor, calculateColorsInfo.taskResult, calculateColorsInfo.meshVertexData, calculateColorsInfo.setMeshComponentVertexColorsUsingSerializedStringSettings, calculateColorsInfo.amountOfPaintedColorsOfEachChannelAtMinAmount_AfterApplyingColor, calculateColorsInfo.vertexColorsOnEachBone, calculateColorsInfo.additionalDataToPassThrough);
	}
}


//-------------------------------------------------------

// Check Valids

bool UVertexPaintFunctionLibrary::CheckValidFundementals(UPrimitiveComponent* component, bool printLogsToScreen, bool printLogsToOutputLog, float printLogsToScreen_Duration) {


	if (!IsValid(component)) {

		if (printLogsToOutputLog)
			UE_LOG(LogTemp, Warning, TEXT("Runtime Vertex Color Paint and Detection Plugin - Trying to Paint/Detect but Component provided is not Valid! Have you connected anything to the component pin on the Node?"));

		return false;
	}

	if (!IsWorldValid(component->GetWorld())) {

		if (printLogsToOutputLog)
			UE_LOG(LogTemp, Warning, TEXT("Runtime Vertex Color Paint and Detection Plugin - World not Valid"));

		return false;
	}

	if (!IsInGameThread()) {

		if (printLogsToScreen || printLogsToOutputLog)
			UKismetSystemLibrary::PrintString(component->GetWorld(), FString::Printf(TEXT("Runtime Vertex Color Paint and Detection Plugin - Trying to Start a Task but not from Game Thread!")), printLogsToScreen, printLogsToOutputLog, FColor::Red, printLogsToScreen_Duration);

		return false;
	}


	if (!IsValid(component->GetOwner()->GetLevel()->GetOuter())) {

		if (printLogsToScreen || printLogsToOutputLog)
			UKismetSystemLibrary::PrintString(component->GetWorld(), FString::Printf(TEXT("Runtime Vertex Color Paint and Detection Plugin - Actor not in valid sub level!")), printLogsToScreen, printLogsToOutputLog, FColor::Red, printLogsToScreen_Duration);

		return false;
	}


	UStaticMeshComponent* staticMeshComp = nullptr;
	USkeletalMeshComponent* skelMeshComp = nullptr;
	USkeletalMesh* skelMesh_Local = nullptr;

	staticMeshComp = Cast<UStaticMeshComponent>(component);

	if (!Cast<UStaticMeshComponent>(component)) {

		skelMeshComp = Cast<USkeletalMeshComponent>(component);
	}


	if (!staticMeshComp && !skelMeshComp) {

		if (printLogsToScreen || printLogsToOutputLog)
			UKismetSystemLibrary::PrintString(component->GetWorld(), FString::Printf(TEXT("Runtime Vertex Color Paint and Detection Plugin - Trying to Paint/Detect but Component %s is not a Static, Spline or Skeletal Mesh!"), *component->GetName()), printLogsToScreen, printLogsToOutputLog, FColor::Red, printLogsToScreen_Duration);

		return false;
	}



	if (staticMeshComp) {

		if (!IsValid(staticMeshComp->GetStaticMesh())) {

			if (printLogsToScreen || printLogsToOutputLog)
				UKismetSystemLibrary::PrintString(component->GetWorld(), FString::Printf(TEXT("Runtime Vertex Color Paint and Detection Plugin - Trying to Paint/Detect on Static Mesh Component %s but it has no Mesh is Set"), *staticMeshComp->GetName()), printLogsToScreen, printLogsToOutputLog, FColor::Red, printLogsToScreen_Duration);

			return false;
		}

		if (!staticMeshComp->GetStaticMesh()->bAllowCPUAccess) {


			if (printLogsToScreen || printLogsToOutputLog)
				UKismetSystemLibrary::PrintString(component->GetWorld(), FString::Printf(TEXT("Runtime Vertex Color Paint and Detection Plugin - Trying to Paint/Detect on Static Mesh but it's CPU Access is not set to True")), printLogsToScreen, printLogsToOutputLog, FColor::Red, printLogsToScreen_Duration);

			return false;
		}


#if ENGINE_MAJOR_VERSION == 5

		// Note ->NaniteSettings was only available in Editor, so had to use this to check if nanite is enabled. 
		if (staticMeshComp->GetStaticMesh().Get()->HasValidNaniteData()) {

			if (printLogsToScreen || printLogsToOutputLog)
				UKismetSystemLibrary::PrintString(component->GetWorld(), FString::Printf(TEXT("Runtime Vertex Color Paint and Detection Plugin - Trying to Paint/Detect on Static Mesh that has Nanite Enabled! Vertex Painting on Nanite Meshes is currently not supported. ")), printLogsToScreen, printLogsToOutputLog, FColor::Red, printLogsToScreen_Duration);

			return false;
		}

#endif


		if (!staticMeshComp->GetBodySetup()) {

			if (printLogsToScreen || printLogsToOutputLog)
				UKismetSystemLibrary::PrintString(component->GetWorld(), FString::Printf(TEXT("Runtime Vertex Color Paint and Detection Plugin - Trying to Paint/Detect on Static Mesh but it doesn't have a Body Setup")), printLogsToScreen, printLogsToOutputLog, FColor::Red, printLogsToScreen_Duration);

			return false;
		}

		if (printLogsToScreen || printLogsToOutputLog)
			UKismetSystemLibrary::PrintString(component->GetWorld(), FString::Printf(TEXT("Runtime Vertex Color Paint and Detection Plugin - Trying to Paint/Detect on Static Mesh Component, with Source Mesh: %s"), *staticMeshComp->GetStaticMesh()->GetName()), printLogsToScreen, printLogsToOutputLog, FColor::Cyan, printLogsToScreen_Duration);
	}

	else if (skelMeshComp) {


#if ENGINE_MAJOR_VERSION == 4

		skelMesh_Local = skelMeshComp->SkeletalMesh;

#elif ENGINE_MAJOR_VERSION == 5

#if ENGINE_MINOR_VERSION == 0

		skelMesh_Local = skelMeshComp->SkeletalMesh.Get();

#else

		skelMesh_Local = skelMeshComp->GetSkeletalMeshAsset();

#endif
#endif


		if (!IsValid(skelMesh_Local)) {

			if (printLogsToScreen || printLogsToOutputLog)
				UKismetSystemLibrary::PrintString(component->GetWorld(), FString::Printf(TEXT("Runtime Vertex Color Paint and Detection Plugin - Trying to Paint/Detect on Skeletal Mesh Component but source mesh is null")), printLogsToScreen, printLogsToOutputLog, FColor::Red, printLogsToScreen_Duration);

			return false;
		}

		else {

			if (printLogsToScreen || printLogsToOutputLog)
				UKismetSystemLibrary::PrintString(component->GetWorld(), FString::Printf(TEXT("Runtime Vertex Color Paint and Detection Plugin - Trying to Paint/Detect on Skeletal Mesh Component, with Source Mesh: %s"), *skelMesh_Local->GetName()), printLogsToScreen, printLogsToOutputLog, FColor::Cyan, printLogsToScreen_Duration);
		}


		if (!skelMeshComp->GetSkeletalMeshRenderData()) {

			if (printLogsToScreen || printLogsToOutputLog)
				UKismetSystemLibrary::PrintString(component->GetWorld(), FString::Printf(TEXT("Runtime Vertex Color Paint and Detection Plugin - Trying to Paint/Detect on Skeletal Mesh but it hasn't properly been initialized yet because it's Skeletal Mesh Render Data isn't valid. ")), printLogsToScreen, printLogsToOutputLog, FColor::Red, printLogsToScreen_Duration);

			return false;
		}

		if (!skelMeshComp->GetSkeletalMeshRenderData()->IsInitialized()) {

			if (printLogsToScreen || printLogsToOutputLog)
				UKismetSystemLibrary::PrintString(component->GetWorld(), FString::Printf(TEXT("Runtime Vertex Color Paint and Detection Plugin - Trying to Paint/Detect on Skeletal Mesh %s but it's SkeletalMeshRenderData isn't Initialized yet. "), *component->GetOwner()->GetName()), printLogsToScreen, printLogsToOutputLog, FColor::Red, printLogsToScreen_Duration);

			return false;
		}


		if (!skelMesh_Local->GetResourceForRendering()) {

			if (printLogsToScreen || printLogsToOutputLog)
				UKismetSystemLibrary::PrintString(component->GetWorld(), FString::Printf(TEXT("Runtime Vertex Color Paint and Detection Plugin - Trying to Paint/Detect on Skeletal Mesh %s but it has invalid ResourceForRendering!"), *component->GetOwner()->GetName()), printLogsToScreen, printLogsToOutputLog, FColor::Red, printLogsToScreen_Duration);

			return false;
		}


		// Could get a crash very rarely if switching skeletal meshes and painting every frame, so added these for extra checks so we hopefully can't create a task if these aren't valid. We also have these in the task itself in case they become invalid after this. 
		for (int currentLOD_Local = 0; currentLOD_Local < skelMeshComp->GetSkeletalMeshRenderData()->LODRenderData.Num(); currentLOD_Local++) {


			const FSkeletalMeshLODRenderData& skelMeshRenderData_Local = skelMeshComp->GetSkeletalMeshRenderData()->LODRenderData[currentLOD_Local];

			if (skelMeshRenderData_Local.HasClothData()) {

				if (!skelMeshRenderData_Local.ClothVertexBuffer.IsInitialized()) {

					if (printLogsToScreen || printLogsToOutputLog)
						UKismetSystemLibrary::PrintString(component->GetWorld(), FString::Printf(TEXT("Runtime Vertex Color Paint and Detection Plugin - Trying to Paint/Detect on Skeletal Mesh with Cloth but cloth vertex buffer hasn't properly been initialized yet, i.e. the paint job is run to early. ")), printLogsToScreen, printLogsToOutputLog, FColor::Red, printLogsToScreen_Duration);

					return false;
				}
			}

			if (!skelMeshRenderData_Local.StaticVertexBuffers.StaticMeshVertexBuffer.IsInitialized() || !skelMeshRenderData_Local.StaticVertexBuffers.StaticMeshVertexBuffer.TangentsVertexBuffer.IsInitialized() || !skelMeshRenderData_Local.StaticVertexBuffers.StaticMeshVertexBuffer.GetTangentData()) {

				if (printLogsToScreen || printLogsToOutputLog)
					UKismetSystemLibrary::PrintString(component->GetWorld(), FString::Printf(TEXT("Runtime Vertex Color Paint and Detection Plugin - Trying to Paint/Detect on Skeletal Mesh but it hasn't properly been initialized yet, i.e. the paint job is run to early. ")), printLogsToScreen, printLogsToOutputLog, FColor::Red, printLogsToScreen_Duration);

				return false;
			}
		}
	}

	return true;
}


bool UVertexPaintFunctionLibrary::CheckValidVertexPaintSettings(FVertexPaintStruct vertexPaintSettings, bool printLogsToScreen, bool printLogsToOutputLog, float printLogsToScreen_Duration) {

	if (!IsValid(vertexPaintSettings.actor)) return false;
	if (!IsValid(vertexPaintSettings.actor->GetWorld())) return false;
	if (!IsValid(vertexPaintSettings.meshComponent)) return false;


	// Can ignore the task queue limit, in case you need to load a game and queue a bunch of paint task but just once
	if (!vertexPaintSettings.ignoreTaskQueueLimit) {

		int maxAllowedTasks_Local = GetDefault<UVertexPaintDetectionSettings>()->vertexPaintAndDetectionPlugin_MaxAmountOfAllowedTasksPerMesh;

		if (UVertexPaintFunctionLibrary::GetCalculateColorsPaintTasks_Wrapper(vertexPaintSettings.meshComponent->GetWorld()).FindRef(vertexPaintSettings.meshComponent).calculateColorsQueueArray.Num() >= maxAllowedTasks_Local) {

			// Always prints this to log so users can see in the output that something may not be fully optimized
			UKismetSystemLibrary::PrintString(vertexPaintSettings.actor->GetWorld(), FString::Printf(TEXT("Runtime Vertex Color Paint and Detection Plugin - Mesh: %s Has over %i Allowed Paint Tasks Queue Limit and we don't Allow any more per Mesh as the Performance gets affected if the queue per mesh becomes too big since the TMaps become more expensive to use! You can change this in the Project Settings, but if the queue grows to big you will get unwanted result as well since it may take a while for a paint job to show it's effect. \nRecommend Reviewing how often you Add new Tasks. You can for instance Add a New Task when the Old one is Finished instead of adding them every frame. "), *vertexPaintSettings.meshComponent->GetName(), maxAllowedTasks_Local), printLogsToScreen, true, FColor::Red, printLogsToScreen_Duration);

			return false;
		}
	}


	if (vertexPaintSettings.overrideLOD.overrideLODToPaintUpTo) {

		if (vertexPaintSettings.overrideLOD.amountOfLODsToPaint <= 0) {

			if (printLogsToScreen || printLogsToOutputLog)
				UKismetSystemLibrary::PrintString(vertexPaintSettings.actor->GetWorld(), FString::Printf(TEXT("Runtime Vertex Color Paint and Detection Plugin - Trying to Paint and Override LOD to paint on but LOD given is <= 0!")), printLogsToScreen, printLogsToOutputLog, FColor::Red, printLogsToScreen_Duration);

			return false;
		}
	}


	// If anything is added onto the array
	if (vertexPaintSettings.canOnlyApplyPaintOnTheseActors.Num() > 0) {

		// If the actor we're trying to paint on isn't in the array. 
		if (!vertexPaintSettings.canOnlyApplyPaintOnTheseActors.Contains(vertexPaintSettings.actor)) {

			if (printLogsToScreen || printLogsToOutputLog)
				UKismetSystemLibrary::PrintString(vertexPaintSettings.meshComponent->GetWorld(), FString::Printf(TEXT("Runtime Vertex Color Paint and Detection Plugin - Actor: %s  isn't in the canOnlyApplyPaintOnTheseActors array that has been set. Either add the actor to it, or don't fill the array."), *vertexPaintSettings.actor->GetName()), printLogsToScreen, printLogsToOutputLog, FColor::Red, printLogsToScreen_Duration);

			return false;
		}
	}


	if (auto skelMeshComp = Cast<USkeletalMeshComponent>(vertexPaintSettings.meshComponent)) {

		if (skelMeshComp->GetSkeletalMeshRenderData()) {

			if (skelMeshComp->GetSkeletalMeshRenderData()->LODRenderData.IsValidIndex(skelMeshComp->GetPredictedLODLevel())) {

				// If current viewable LOD has cloth then requires bWaitForParallelClothTask to be true
				if (skelMeshComp->GetSkeletalMeshRenderData()->LODRenderData[skelMeshComp->GetPredictedLODLevel()].HasClothData()) {

					if (!skelMeshComp->bWaitForParallelClothTask && vertexPaintSettings.affectClothPhysics) {

						if (printLogsToScreen || printLogsToOutputLog)
							UKismetSystemLibrary::PrintString(vertexPaintSettings.meshComponent->GetWorld(), FString::Printf(TEXT("Runtime Vertex Color Paint and Detection Plugin - Trying to Detect/Paint on Skeletal Mesh with Cloth but the Skeletal Mesh Component bWaitForParallelClothTask is false!")), printLogsToScreen, printLogsToOutputLog, FColor::Red, printLogsToScreen_Duration);

						return false;
					}
				}
			}
		}
	}


	return true;
}


bool UVertexPaintFunctionLibrary::CheckValidVertexPaintColorSettings(FVertexPaintColorSettingStruct colorSettings, bool printLogsToScreen, bool printLogsToOutputLog, float printLogsToScreen_Duration) {

	if (!IsValid(colorSettings.actor)) return false;
	if (!IsValid(colorSettings.actor->GetWorld())) return false;
	if (!IsValid(colorSettings.meshComponent)) return false;


	if (colorSettings.applyVertexColorSettings.applyColorsUsingPhysicsSurface.applyVertexColorUsingPhysicsSurface) {


		if (!GetVertexPaintMaterialDataAsset()) {

			if (printLogsToScreen || printLogsToOutputLog)
				UKismetSystemLibrary::PrintString(colorSettings.actor->GetWorld(), FString::Printf(TEXT("Runtime Vertex Color Paint and Detection Plugin - Set to Apply Colors Using Physics Surface, but no Material Data Asset is set in the Project Settings. This means that we can't get what Physics Surface is on each Vertex Color Channel. ")), printLogsToScreen, printLogsToOutputLog, FColor::Red, printLogsToScreen_Duration);

			return false;
		}

		// If set to paint with Physics Surface with Add, but the Paint Strength is 0 so no difference will be made
		if (colorSettings.applyVertexColorSettings.applyColorsUsingPhysicsSurface.paintStrengthToApply == 0 && colorSettings.applyVertexColorSettings.applyColorsUsingPhysicsSurface.paintStrengtOnChannelsWithoutThePhysicsSurface == 0 && colorSettings.applyVertexColorSettings.applyVertexColorSetting == EApplyVertexColorSetting::EAddVertexColor) {

			if (printLogsToScreen || printLogsToOutputLog)
				UKismetSystemLibrary::PrintString(colorSettings.actor->GetWorld(), FString::Printf(TEXT("Runtime Vertex Color Paint and Detection Plugin - Set to Apply Colors Using Physics Surface, but Paint Strength Multiplier is 0. ")), printLogsToScreen, printLogsToOutputLog, FColor::Red, printLogsToScreen_Duration);

			return false;
		}

		// If set to paint with Physics Surface but it's set to Default
		if (colorSettings.applyVertexColorSettings.applyColorsUsingPhysicsSurface.physicsSurfaceToApply == EPhysicalSurface::SurfaceType_Default) {

			if (printLogsToScreen || printLogsToOutputLog)
				UKismetSystemLibrary::PrintString(colorSettings.actor->GetWorld(), FString::Printf(TEXT("Runtime Vertex Color Paint and Detection Plugin - Set to Apply Colors Using Physics Surface, but it's set to Default. ")), printLogsToScreen, printLogsToOutputLog, FColor::Red, printLogsToScreen_Duration);

			return false;
		}
	}

	// If not set to paint with Physics Surface and it's paint strength multiplier, then we can fail the check here if no difference is going to be made
	if (colorSettings.applyVertexColorSettings.applyVertexColorSetting == EApplyVertexColorSetting::EAddVertexColor && !colorSettings.applyVertexColorSettings.applyColorsUsingPhysicsSurface.applyVertexColorUsingPhysicsSurface) {

		// If all apply color is 0
		if (colorSettings.applyVertexColorSettings.redColor == 0 && colorSettings.applyVertexColorSettings.greenColor == 0 && colorSettings.applyVertexColorSettings.blueColor == 0 && colorSettings.applyVertexColorSettings.alphaColor == 0) {

			if (printLogsToScreen || printLogsToOutputLog)
				UKismetSystemLibrary::PrintString(colorSettings.actor->GetWorld(), FString::Printf(TEXT("Runtime Vertex Color Paint and Detection Plugin - Trying to Apply Colors with 0 Strength in Colors which wouldn't make any difference. ")), printLogsToScreen, printLogsToOutputLog, FColor::Red, printLogsToScreen_Duration);

			return false;
		}
	}


	return true;
}


bool UVertexPaintFunctionLibrary::CheckHaveReachedMaxDetectionTask(FVertexDetectFundementalStruct fundementalSettings, bool printLogsToScreen, bool printLogsToOutputLog, float printLogsToScreen_Duration) {

	if (!IsValid(fundementalSettings.actor)) return false;
	if (!IsValid(fundementalSettings.actor->GetWorld())) return false;
	if (!IsValid(fundementalSettings.meshComponent)) return false;


	// Can ignore the Task queue limit, which may be useful for Save / Load Systems, in case you need to detect on a bunch of meshes but just once
	if (!fundementalSettings.ignoreTaskQueueLimit) {

		int maxAllowedTasks_Local = GetDefault<UVertexPaintDetectionSettings>()->vertexPaintAndDetectionPlugin_MaxAmountOfAllowedTasksPerMesh;

		if (UVertexPaintFunctionLibrary::GetCalculateColorsDetectionTasks_Wrapper(fundementalSettings.meshComponent->GetWorld()).FindRef(fundementalSettings.meshComponent).calculateColorsQueueArray.Num() >= maxAllowedTasks_Local) {

			// Always prints this to log so users can see in the output that something may not be fully optimized
			UKismetSystemLibrary::PrintString(fundementalSettings.meshComponent->GetWorld(), FString::Printf(TEXT("Runtime Vertex Color Paint and Detection Plugin - Mesh: %s Has over %i Allowed Detect Tasks Queue Limit and we don't Allow any more per Mesh as the Performance gets affected if the queue per mesh becomes to big since the TMaps become more expensive to use! You can change this in the Project Settings, but if the queue grows too big you will get unwanted result as well since it may take a while for a detect job run and the callback will run. \nRecommend Reviewing how often you Add new Tasks. You can for instance Add a New Task when the Old one is Finished instead of adding them every frame. "), *fundementalSettings.meshComponent->GetName(), maxAllowedTasks_Local), printLogsToScreen, true, FColor::Red, printLogsToScreen_Duration);

			return true;
		}
	}

	else {

		return false;
	}


	return false;
}
