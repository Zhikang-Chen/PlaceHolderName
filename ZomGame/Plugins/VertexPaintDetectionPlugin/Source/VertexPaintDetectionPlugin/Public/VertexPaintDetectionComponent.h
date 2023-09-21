// Copyright 2022 Alexander Floden, Alias Alex River. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "VertexPaintDetectionStructs.h"
#include "Engine/StreamableManager.h"
#include "VertexPaintDetectionComponent.generated.h"


class UVertexPaintDetectionGISubSystem;


//-------------------------------------------------------


DECLARE_DYNAMIC_MULTICAST_DELEGATE_NineParams(FVertexColorGetClosestVertexData, FVertexDetectTaskResultInfo, taskResultInfo, FVertexDetectMeshDataStruct, meshVertexData, FVertexDetectInfoStruct, detectedMeshWithSettings, FVertexDetectClosestVertexDataResultStruct, closestVertexInfo, FVertexDetectEstimatedColorAtHitLocationResultStruct, estimatedColorAtHitLocationInfo, FVertexDetectAvarageColorInAreaInfo, avarageColorInAreaInfo, FVertexDetectAmountOfPaintedColorsOfEachChannel, amountOfPaintedColorsOfEachChannel, FVertexDetectVertexColorsOnEachBone, vertexColorsOnEachBone, FVertexDetectAdditionalDataToPassThrough, additionalData);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FVertexColorGetAllVertexColorsOnly, FVertexDetectTaskResultInfo, taskResultInfo, FVertexDetectMeshDataStruct, meshVertexData, FVertexDetectGetColorsOnlyStruct, gotAllVertexColorsWithSettings, FVertexDetectAmountOfPaintedColorsOfEachChannel, amountOfPaintedColorsOfEachChannel, FVertexDetectVertexColorsOnEachBone, vertexColorsOnEachBone, FVertexDetectAdditionalDataToPassThrough, additionalData);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_NineParams(FVertexColorPaintedAtLocation, FVertexDetectTaskResultInfo, taskResultInfo, FVertexDetectMeshDataStruct, meshVertexData, FVertexPaintAtLocationStruct, meshPaintedWithSettings, FVertexDetectClosestVertexDataResultStruct, closestVertexInfoAfterApplyingColor, FVertexDetectEstimatedColorAtHitLocationResultStruct, estimatedColorAtHitLocationInfo, FVertexDetectAvarageColorInAreaInfo, avarageColorInAreaInfo, FVertexDetectAmountOfPaintedColorsOfEachChannel, amountOfPaintedColorsOfEachChannel, FVertexDetectVertexColorsOnEachBone, vertexColorsOnEachBone, FVertexDetectAdditionalDataToPassThrough, additionalData);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FVertexColorPaintedWithinArea, FVertexDetectTaskResultInfo, taskResultInfo, FVertexDetectMeshDataStruct, meshVertexData, FVertexPainthWithinAreaStruct, areaWithinMeshPaintedWithSettings, FVertexDetectAmountOfPaintedColorsOfEachChannel, amountOfPaintedColorsOfEachChannel, FVertexDetectVertexColorsOnEachBone, vertexColorsOnEachBone, FVertexDetectAdditionalDataToPassThrough, additionalData);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FVertexColorPaintedEntireMesh, FVertexDetectTaskResultInfo, taskResultInfo, FVertexDetectMeshDataStruct, meshVertexData, FVertexPaintOnEntireMeshStruct, entireMeshPaintedWithSettings, FVertexDetectAmountOfPaintedColorsOfEachChannel, amountOfPaintedColorsOfEachChannel, FVertexDetectVertexColorsOnEachBone, vertexColorsOnEachBone, FVertexDetectAdditionalDataToPassThrough, additionalData);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FVertexColorPaintColorSnippet, FVertexDetectTaskResultInfo, taskResultInfo, FVertexDetectMeshDataStruct, meshVertexData, FVertexPaintColorSnippetStruct, paintColorSnippetWithSettings, FVertexDetectAmountOfPaintedColorsOfEachChannel, amountOfPaintedColorsOfEachChannel, FVertexDetectVertexColorsOnEachBone, vertexColorsOnEachBone, FVertexDetectAdditionalDataToPassThrough, additionalData);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FSetMeshComponentVertexColors, FVertexDetectTaskResultInfo, taskResultInfo, FVertexDetectMeshDataStruct, meshVertexData, FVertexPaintStruct, setMeshComponentVertexColorSettings, FVertexDetectAmountOfPaintedColorsOfEachChannel, amountOfPaintedColorsOfEachChannel, FVertexDetectVertexColorsOnEachBone, vertexColorsOnEachBone, FVertexDetectAdditionalDataToPassThrough, additionalData);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FSetMeshComponentVertexColorsUsingSerializedString, FVertexDetectTaskResultInfo, taskResultInfo, FVertexDetectMeshDataStruct, meshVertexData, FVertexPaintSetMeshComponentVertexColorsUsingSerializedString, setMeshComponentVertexColorUsingSerializedStringSettings, FVertexDetectAmountOfPaintedColorsOfEachChannel, amountOfPaintedColorsOfEachChannel, FVertexDetectVertexColorsOnEachBone, vertexColorsOnEachBone, FVertexDetectAdditionalDataToPassThrough, additionalData);


//-------------------------------------------------------

// NOTE Not blueprint spawnable so only the Blueprint Component that inherits from this can be added to BPs in case we add something on that layer
UCLASS(Blueprintable, BlueprintType, ClassGroup = (Custom))
class VERTEXPAINTDETECTIONPLUGIN_API UVertexPaintDetectionComponent : public UActorComponent {

	GENERATED_BODY()


protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:


	//---------- PAINT & DETECT FUNCTIONS ----------//

	UVertexPaintDetectionComponent();

	void GetClosestVertexDataOnMesh(FVertexDetectInfoStruct getClosestVertexDataStruct, FVertexDetectAdditionalDataToPassThrough additionalDataToPassThrough);
	void GetClosestVertexDataOnMeshTaskFinished();

	void GetAllVertexColorsOnly(FVertexDetectGetColorsOnlyStruct getAllVertexColorsStruct, FVertexDetectAdditionalDataToPassThrough additionalDataToPassThrough);
	void GetAllVertexColorsOnlyTaskFinished();

	void PaintOnMeshAtLocation(FVertexPaintAtLocationStruct paintAtLocationStruct, FVertexDetectAdditionalDataToPassThrough additionalDataToPassThrough);
	void PaintOnMeshAtLocationTaskFinished();

	void PaintOnMeshWithinArea(FVertexPainthWithinAreaStruct paintWithinAreaStruct, FVertexDetectAdditionalDataToPassThrough additionalDataToPassThrough);
	void PaintOnMeshWithinAreaTaskFinished();

	void PaintOnEntireMesh(FVertexPaintOnEntireMeshStruct paintOnEntireMeshStruct, FVertexDetectAdditionalDataToPassThrough additionalDataToPassThrough);
	void PaintOnEntireMeshTaskFinished();

	void PaintColorSnippetOnMesh(FVertexPaintColorSnippetStruct paintColorSnippetStruct, FVertexDetectAdditionalDataToPassThrough additionalDataToPassThrough);
	void PaintColorSnippetOnMeshTaskFinished();

	UFUNCTION(BlueprintCallable, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "This returns the amount of Tasks that this component has queued up but have not finished yet. Can be useful if you for instance want to make sure another task of a certain type doesn't start if there's already one going. "))
		void GetCurrentTasksInitiatedByComponent(int& totalAmountOfTasks, int& amountOfGetClosestVertexDataTasks, int& amountOfGetAllVertexColorsOnlyTasks, int& amountOfPaintAtLocationTasks, int& amountOfPaintWithinAreaTasks, int& amountOfPaintEntireMeshTasks, int& amountOfPaintColorSnippetTasks);


	//---------- VERTEX PAINT / DETECTION SETTINGS ----------//

	UPROPERTY(BlueprintAssignable, Category = "Runtime Vertex Color Paint and Detection Plugin|Detection")
		FVertexColorGetClosestVertexData getClosestVertexDataDelegate; // Broadcasts when Finished Getting Vertex Data, either successfully or unsuccessfully. NOTE you may have to type in .AddDynamic() manually if it doesn't show


	UPROPERTY(BlueprintAssignable, Category = "Runtime Vertex Color Paint and Detection Plugin|Detection", meta = (ToolTip = "Broadcasts when Finished Getting All Vertex Colors Only, either successfully or unsuccessfully."))
		FVertexColorGetAllVertexColorsOnly getAllVertexColorsOnlyDelegate; // Broadcasts when Finished Getting Vertex Data, either successfully or unsuccessfully. NOTE you may have to type in .AddDynamic() manually if it doesn't show


	UPROPERTY(BlueprintAssignable, Category = "Runtime Vertex Color Paint and Detection Plugin|Painting", meta = (ToolTip = "Broadcasts when Finished Painting Vertex Colors, either successfully or unsuccessfully."))
		FVertexColorPaintedAtLocation vertexColorsPaintedAtLocationDelegate; // Broadcasts when Finished Painting Vertex Colors, either successfully or unsuccessfully. NOTE you may have to type in .AddDynamic() manually if it doesn't show

	UPROPERTY(BlueprintAssignable, Category = "Runtime Vertex Color Paint and Detection Plugin|Painting", meta = (ToolTip = "Broadcasts when Finished Painting Vertex Colors on Mesh within Area, either successfully or unsuccessfully."))
		FVertexColorPaintedWithinArea vertexColorsPaintedMeshWithinAreaDelegate; // Broadcasts when Finished Painting Vertex Colors on Mesh within Area, either successfully or unsuccessfully. NOTE you may have to type in .AddDynamic() manually if it doesn't show

	UPROPERTY(BlueprintAssignable, Category = "Runtime Vertex Color Paint and Detection Plugin|Painting", meta = (ToolTip = "Broadcasts when Finished Painting Vertex Colors on Entire Mesh, either successfully or unsuccessfully."))
		FVertexColorPaintedEntireMesh vertexColorsPaintedEntireMeshDelegate; // Broadcasts when Finished Painting Vertex Colors on Entire Mesh, either successfully or unsuccessfully. NOTE you may have to type in .AddDynamic() manually if it doesn't show

	UPROPERTY(BlueprintAssignable, Category = "Runtime Vertex Color Paint and Detection Plugin|Painting", meta = (ToolTip = "Broadcasts when Finished Paint Color Snippet, either successfully or unsuccessfully. "))
		FVertexColorPaintColorSnippet vertexColorsPaintColorSnippetDelegate; // Broadcasts when Finished Paint Color Snippet, either successfully or unsuccessfully. NOTE you may have to type in .AddDynamic() manually if it doesn't show

	UPROPERTY(BlueprintAssignable, Category = "Runtime Vertex Color Paint and Detection Plugin|Painting", meta = (ToolTip = "Broadcasts when VertexPaintDetectionGISubSystem has run it's SetMeshComponentVertexColors IF you provided a component. Useful if you're loading vertex data and want to start each task when the previous is finished. "))
		FSetMeshComponentVertexColors vertexColorsSetMeshColorsDelegate; // Broadcasts when VertexPaintDetectionGISubSystem has run it's SetMeshComponentVertexColors IF you provided a component. Useful if you're loading vertex data and want to start each task when the previous is finished. NOTE you may have to type in .AddDynamic() manually if it doesn't show


	UPROPERTY(BlueprintAssignable, Category = "Runtime Vertex Color Paint and Detection Plugin|Painting", meta = (ToolTip = "Broadcasts when VertexPaintDetectionGISubSystem has run it's SetMeshComponentVertexColorsUsingSerializedString IF you provided a component. Useful if you're loading vertex data and want to start each task when the previous is finished. "))
		FSetMeshComponentVertexColorsUsingSerializedString vertexColorsSetMeshColorsUsingSerializedStringDelegate; // Broadcasts when VertexPaintDetectionGISubSystem has run it's SetMeshComponentVertexColorsUsingSerializedString IF you provided a component. Useful if you're loading vertex data and want to start each task when the previous is finished. NOTE you may have to type in .AddDynamic() manually if it doesn't show


	//---------- DEBUG SETTINGS ----------//

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin|Debug Settings|Logs", meta = (ToolTip = "If should print logs to screen."))
		bool printLogsToScreen = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin|Debug Settings|Logs", meta = (ToolTip = "Duration of the Print Strings on the Screen"))
		float printLogsToScreen_Duration = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin|Debug Settings|Logs", meta = (ToolTip = "If should print logs to output log."))
		bool printLogsToOutputLog = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin|Debug Settings|Vertex Symbols", meta = (ToolTip = "Draws a Debug Point for Each Vertex of the Mesh. \nNOTE! Only possible if running the Paint Job with Multithreading set to False, and can be quite expensive if the Mesh has a lot of Vertices"))
		bool atGameThread_drawVertexPositionDebugPoint = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin|Debug Settings|Vertex Symbols", meta = (ToolTip = "Draws a Debug Arrow for each Vertex Normal. \nNOTE! Only possible if running the Paint Job with Multithreading set to False, and can be quite expensive if the Cloth has a lot of Vertices"))
		bool atGameThread_drawVertexNormalDebugArrow = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin|Debug Settings|Vertex Symbols", meta = (ToolTip = "Draws a Debug Point for the Vertex of the Mesh that was within range and got paint applied. \nNOTE! Only possible if running the Paint Job with Multithreading set to False, and can be quite expensive if the Mesh has a lot of Vertices"))
		bool atGameThread_drawVertexPositionDebugPointOnVertexThatGotPaintApplied = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin|Debug Settings|Vertex Symbols", meta = (ToolTip = "Draws a Debug Point for Each Vertex of Cloths of the Mesh we're looping through, not just the ones that are getting painted. \nNOTE! Only possible if running the Paint Job with Multithreading set to False, and can be quite expensive if the Cloth has a lot of Vertices"))
		bool atGameThread_drawClothVertexPositionDebugPoint = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin|Debug Settings|Vertex Symbols", meta = (ToolTip = "Duration of the Drawn Vertex. Can be very low if you're painting every frame. "))
		float atGameThread_DrawVertexPointsDuration = 0.15;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin|Debug Settings|Paint Condition Symbols", meta = (ToolTip = "This will for instance Draw the Trace Line if using Condition where Vertex has to have Line of Sight to an Actor / Position to get painted. "))
		bool atGameThread_DrawPaintConditionsDebugSymbols = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin|Debug Settings|Paint Condition Symbols", meta = (ToolTip = "This will for instance Draw the Trace Line if using Condition where Vertex has to have Line of Sight to an Actor / Position to get painted. "))
		float atGameThread_DrawPaintConditionsDebugSymbolsDuration = 0.25f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin|Debug Settings|Get Closest Vertex Data Symbols", meta = (ToolTip = "Draws a Debug Box of Closest Vertex and an Arrow of its Normal"))
		bool drawGetClosestVertexDataDebugSymbols = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin|Debug Settings|Get Closest Vertex Data Symbols")
		float drawGetClosestVertexDataDebugSymbolsDuration = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin|Debug Settings|Get Estimated Color At Hit Location Symbols", meta = (ToolTip = "This will Draw Debug Symbols related to getting the Vertex Data at the Hit Location, such as the nearby vertices of the closest vertex, which vertex that got choosen when we want to find the color between the closest vertex and the one toward the direction from the closest to the hit location, since we scale between them. Also arrows that indicate the direction."))
		bool drawGetEstimatedColorAtHitLocationDebugSymbols = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin|Debug Settings|Get Estimated Color At Hit Location Symbols")
		float drawGetEstimatedColorAtHitLocationDebugSymbolsDuration = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin|Debug Settings|Paint Within Area Symbols", meta = (ToolTip = "This will Draw the Bounds that we use when running Paint Within Area. "))
		bool drawPaintWithinAreaDebugSymbols = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin|Debug Settings|Paint Within Area Symbols")
		float drawPaintWithinAreaDebugSymbolsDuration = 0.5f;


private:

	void PaintColorSnippetOnMesh_FinishedLoadingColorSnippetDataAsset(int finishedID, FVertexPaintColorSnippetStruct paintColorSnippetStruct, FVertexDetectAdditionalDataToPassThrough additionalDataToPassThrough);

	void FillCalculateColorsInfoDebugSettings(FCalculateColorsInfo& calculateColorsInfo);


	//---------- PROPERTIES ----------//

	TMap<int, TSharedPtr<FStreamableHandle>> colorSnippetAsyncLoadHandleMap;

	UPROPERTY()
		UVertexPaintDetectionGISubSystem* vertexPaintGameInstanceSubsystem_Global = nullptr;

	int maxAllowedTasksPerMesh = 50;

	int amountOfGetClosestVertexDataTasksCache = 0;
	int amountOfGetAllVertexColorsOnlyTasksCache = 0;
	int amountOfPaintAtLocationTasksCache = 0;
	int amountOfPaintWithinAreaTasksCache = 0;
	int amountOfPaintEntireMeshTasksCache = 0;
	int amountOfPaintColorSnippetTasksCache = 0;
};
