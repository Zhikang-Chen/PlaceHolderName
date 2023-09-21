// Copyright 2022 Alexander Floden, Alias Alex River. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Misc/QueuedThreadPool.h"
#include "VertexPaintDetectionStructs.h"
#include "VertexPaintDetectionGISubSystem.generated.h"


// Had to move Task Queue Logic to a Game Instance Subsystem from the Engine Subsystem because there was issues otherwise when playing Multiplayer in PIE since then the clients have the same engine subsystem and thus the same queue, which meant that it got 2 tasks added for the same job. 



DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FVertexPaintTaskFinished, FVertexDetectTaskResultInfo, taskResultInfo, FVertexDetectMeshDataStruct, meshVertexData, FVertexDetectAmountOfPaintedColorsOfEachChannel, amountOfPaintedColorsOfEachChannel, FVertexDetectVertexColorsOnEachBone, vertexColorsOnEachBone, FVertexDetectAdditionalDataToPassThrough, additionalData);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FVertexDetectTaskFinished, FVertexDetectTaskResultInfo, taskResultInfo, FVertexDetectMeshDataStruct, meshVertexData, FVertexDetectAmountOfPaintedColorsOfEachChannel, amountOfPaintedColorsOfEachChannel, FVertexDetectVertexColorsOnEachBone, vertexColorsOnEachBone, FVertexDetectAdditionalDataToPassThrough, additionalData);


/**
 *
 */
UCLASS()
class VERTEXPAINTDETECTIONPLUGIN_API UVertexPaintDetectionGISubSystem : public UGameInstanceSubsystem {

	GENERATED_BODY()

public:


	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;


	void ClearMeshPaintedSinceSessionStarted() { meshesPaintedOnSinceSessionStarted.Empty(); }

	TMap<UPrimitiveComponent*, int> GetMeshPaintedSinceSessionStarted() { return meshesPaintedOnSinceSessionStarted; }

	TArray<UPhysicalMaterial*> GetAllCachedPhysicsMaterialAssets() { return cachedPhysicalMaterialAssets; }



	//---------- TASK QUEUE FUNCTIONS ----------//

	void AddCalculateColorsTaskToQueue(FCalculateColorsInfo calculateColorsInfo);

	void StartCalculateColorsTask(const FCalculateColorsInfo& calculateColorsInfo);


	void RemoveCalculateColorsMeshFromPaintTaskQueue(UPrimitiveComponent* meshComp) { if (calculateColorsPaintQueue.Contains(meshComp)) calculateColorsPaintQueue.Remove(meshComp); };

	TMap<UPrimitiveComponent*, FVertexPaintCalculateColorsQueueArrayStruct> GetCalculateColorsPaintTasks() { return calculateColorsPaintQueue; }

	void CalculateColorsTaskFinished(const FCalculateColorsInfo& calculateColorsInfo, const EVertexPaintDetectionTaskResult& paintTaskResult, const TArray<FString>& calculateColorsResultMessage);

	void ClearCalculateColorsPaintTasks() { calculateColorsPaintQueue.Empty(); }

	TMap<UPrimitiveComponent*, int> GetCalculateColorsPaintTasksAmount();


	void RemoveCalculateColorsMeshFromDetectionTaskQueue(UPrimitiveComponent* meshComp) { if (calculateColorsDetectionQueue.Contains(meshComp)) calculateColorsDetectionQueue.Remove(meshComp); };

	TMap<UPrimitiveComponent*, FVertexPaintCalculateColorsQueueArrayStruct> GetCalculateColorsDetectionTasks() { return calculateColorsDetectionQueue; }

	void ClearCalculateColorsDetectionTasks() { calculateColorsDetectionQueue.Empty(); }

	TMap<UPrimitiveComponent*, int> GetCalculateColorsDetectionTasksAmount();


	UPROPERTY(BlueprintAssignable, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "Broadcasts when Finished a Paint Task."))
		FVertexPaintTaskFinished vertexPaintTaskFinished; // Broadcasts when Finished a Paint Task. NOTE you may have to type in .AddDynamic() manually if it doesn't show

	UPROPERTY(BlueprintAssignable, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "Broadcasts when Finished a  Detect Task."))
		FVertexDetectTaskFinished vertexDetectTaskFinished; // Broadcasts when Finished a Detect Task. NOTE you may have to type in .AddDynamic() manually if it doesn't show


private:


	//---------- PAINT / DETECTION WRAP UPS ----------//

	void GetClosestVertexDataOnMesh_WrapUp(const FCalculateColorsInfo& calculateColorsInfo, const FVertexDetectInfoStruct& detectStruct, const FColor& closestColor, const FColor& avarageColor, const FLinearColor& estimatedColorAtHitLocation, const TArray<FVector2D>& closestVertexUV_Detection);

	void GetAllVertexColorsOnly_WrapUp(const FCalculateColorsInfo& calculateColorsInfo);

	void PaintOnMeshAtLocation_WrapUp(const FCalculateColorsInfo& calculateColorsInfo, const TArray<FVector2D>& closestVertexUV_Painting);

	void PaintWithinArea_WrapUp(const FCalculateColorsInfo& calculateColorsInfo);

	void PaintOnEntireMesh_WrapUp(const FCalculateColorsInfo& calculateColorsInfo);

	void PaintColorSnippet_WrapUp(const FCalculateColorsInfo& calculateColorsInfo);

	void ApplyColorsDirectly_WrapUp(const FCalculateColorsInfo& calculateColorsInfo);

	void CalculateColorsTask_WrapUp(FCalculateColorsInfo calculateColorsInfo, const EVertexPaintDetectionTaskResult& taskResult);


	//---------- UTILITIES ----------//

	FVertexDetectClosestVertexDataResultStruct GetClosestVertexData(UPrimitiveComponent* meshComp, FVertexDetectClosestVertexDataResultStruct closestVertexDataResult, const int& closestVertexIndex, const FColor& closestVertexColor, const FVector& closestVertexPosition, const FVector& closestVertexNormal, const FCalculateColorsInfo& calculateColorsInfo);

	FVertexDetectPhysicsSurfaceDataStruct GetPhysicsSurfaceData(const FColor& vertexColor, const FCalculateColorsInfo& calculateColorsInfo, UMaterialInterface* material);

	void PrintClosestVertexColorResults(const FVertexDetectClosestVertexDataResultStruct& closestVertexDataResult, const FVertexDetectEstimatedColorAtHitLocationResultStruct& estimatedColorAtHitLocationResult, const float& avarageColorOfEffect, const FCalculateColorsInfo& calculateColorsInfo);

	void ResetThreadPool();

	void CleanUpOldVertexColorBuffer();

	void AddColorVertexBufferToBufferCleanup(UWorld* world, FColorVertexBuffer* colorVertexBuffer);

	void ApplyVertexColorsToMeshComponent(UPrimitiveComponent* meshComp, TArray<FColorVertexBuffer*> newVertexColorBuffer);

	void BroadcastFinishedPaintTask(const FCalculateColorsInfo& calculateColorsInfo);

	void BroadcastFinishedDetectTask(const FCalculateColorsInfo& calculateColorsInfo);


	//---------- PROPERTIES ----------//

	UPROPERTY()
		TMap<UPrimitiveComponent*, FVertexPaintCalculateColorsQueueArrayStruct> calculateColorsPaintQueue;

	UPROPERTY()
		TMap<UPrimitiveComponent*, FVertexPaintCalculateColorsQueueArrayStruct> calculateColorsDetectionQueue;

	UPROPERTY()
		TMap<int32, FCalculateColorsInfo> onGoingAsyncTasks;

	UPROPERTY()
		FTimerHandle vertexColorPaint_QueueThreadPool_ResetTimer;

	UPROPERTY()
		UWorld* vertexColorPaint_QueueThreadPool_ResetTimerWorld = nullptr;

	FQueuedThreadPool* vertexColorPaint_QueueThreadPool = nullptr;

	TArray<FColorVertexBuffer*> oldVertexColorBufferPtrs;

	UPROPERTY()
		TArray<FTimerHandle> cleanupOldVertexColorBufferTimers;

	// TMap so we can always just run .Add, instead of array where we have to run .Contain and only add if not in it
	UPROPERTY()
		TMap<UPrimitiveComponent*, int> meshesPaintedOnSinceSessionStarted;

	UPROPERTY()
		TArray<UPhysicalMaterial*> cachedPhysicalMaterialAssets;

	UPROPERTY()
		UVertexPaintColorSnippetRefs* vertexPaintColorSnippetReferenceDataAssetCache; // Caches these so they're in memory at all time, so we don't have to worry about a use case where they may not be in memory when a asynctask has started, and that we will get a crash because we can't load them on the async thread

	UPROPERTY()
		UVertexPaintOptimizationDataAsset* vertexPaintOptimizationDataAssetCache;

	UPROPERTY()
		UVertexPaintMaterialDataAsset* vertexPaintMaterialDataAssetCache;
};
