// Copyright 2022 Alexander Floden, Alias Alex River. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "Chaos/ChaosEngineInterface.h" // Had to include since we're using EPhysicsSurface
#include "Engine/EngineTypes.h"
#include "GameplayTagContainer.h"
#include "VertexPaintDetectionStructs.generated.h"


// Forward Declarations as we're just defining them here and not accessing anything
class UVertexPaintColorSnippetDataAsset;
class UVertexPaintColorSnippetRefs;
class UVertexPaintDetectionComponent;
class UVertexPaintOptimizationDataAsset;
class UVertexPaintMaterialDataAsset;
class UClothingAssetBase;
class UMaterial;
class UMaterialInterface;
class FColorVertexBuffer;
class USplineMeshComponent;
class ALandscape;

//-------------------------------------------------------

enum class EVertexPaintDetectionType : uint8 {

	Detection,
	PaintAtLocation,
	PaintWithinArea,
	PaintEntireMesh,
	PaintColorSnippet,
	ApplyVertexDataDirectly
};


//-------------------------------------------------------

UENUM(BlueprintType)
enum class Enum_PaintEntireMeshAtRandomVertices_PropogateToLODsMethod : uint8 {

	ModifiedEngineMethod = 0 UMETA(DisplayName = "Modified Engine Method"),
	DontPropogateLOD0ToAllLODs = 1 UMETA(DisplayName = "Dont Propogate LOD0 To All LODs"),
};


//-------------------------------------------------------

enum class EVertexPaintDetectionTaskResult : uint8 {

	Success,
	Fail,
	FailWithInvalidMesh
};


//-------------------------------------------------------

UENUM(BlueprintType)
enum class Enum_SurfaceAtChannel : uint8 {

	Default = 0 UMETA(DisplayName = "Default"),
	RedChannel = 1 UMETA(DisplayName = "Red Channel"),
	GreenChannel = 2 UMETA(DisplayName = "Green Channel"),
	BlueChannel = 3 UMETA(DisplayName = "Blue Channel"),
	AlphaChannel = 4 UMETA(DisplayName = "Alpha Channel"),
};


UENUM(BlueprintType)
enum class EVertexColorChannel : uint8 {

	RedChannel = 0 UMETA(DisplayName = "Red Channel"),
	GreenChannel = 1 UMETA(DisplayName = "Green Channel"),
	BlueChannel = 2 UMETA(DisplayName = "Blue Channel"),
	AlphaChannel = 3 UMETA(DisplayName = "Alpha Channel"),
};


//-------------------------------------------------------

UENUM(BlueprintType)
enum class Enum_PaintWithinAreaShape : uint8 {

	isSquareOrRectangleShape = 0 UMETA(DisplayName = "Is Square or Rectangle Shape"),
	isSphereShape = 1 UMETA(DisplayName = "Is Sphere Shape"),
	isComplexShape = 2 UMETA(DisplayName = "Is Complex Shape"),
};


//-------------------------------------------------------

// Since the engines own EThreadPriority isn't blueprint type i had to make our own 

UENUM(BlueprintType)
enum class EThreadPriorityWrapper : uint8 {

	TPri_Normal = 0 UMETA(DisplayName = "Normal"),
	TPri_AboveNormal = 1 UMETA(DisplayName = "Above Normal"),
	TPri_BelowNormal = 2 UMETA(DisplayName = "Below Normal"),
	TPri_Highest = 3 UMETA(DisplayName = "Highest"),
	TPri_Lowest = 4 UMETA(DisplayName = "Lowest"),
	TPri_SlightlyBelowNormal = 5 UMETA(DisplayName = "Slightly Below Normal"),
	TPri_TimeCritical = 6 UMETA(DisplayName = "Time Critical"),
	TPri_Num = 7 UMETA(DisplayName = "Num"),
};


//-------------------------------------------------------

// Apply Vertex Color Setting

UENUM(BlueprintType)
enum class EApplyVertexColorSetting : uint8 {

	EAddVertexColor = 0 UMETA(DisplayName = "Add Vertex Color"),
	ESetVertexColor = 1 UMETA(DisplayName = "Set Vertex Color"),
};


//-------------------------------------------------------

// Paint Within Area Falloff Type

UENUM(BlueprintType)
enum class EVertexPaintWithinAreaFallOffType : uint8 {

	SphericalFromCenter = 0 UMETA(DisplayName = "Spherical From Center"),
	GradiantUpward = 1 UMETA(DisplayName = "Gradiant Upward"),
	GradiantDownward = 2 UMETA(DisplayName = "Gradiant Downward"),
};

//-------------------------------------------------------

// Most Painted Surface - Used by the component when detecting surface so we can get the most prominant one.

USTRUCT(BlueprintType)
struct FVertexPaintMultiThreadSettingsStruct {

	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "Whether to use Multithreading for Calculations instead of using Game Thread. Is recommended since you get will get better FPS. Each Mesh will have their own Calculation Task queue where each paint job is based off of the result of the previous. Detection Jobs will be set to be first in the queue so they will run the fastest. "))
	bool useMultithreadingForCalculations = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin")
	EThreadPriorityWrapper multithreadPriority = EThreadPriorityWrapper::TPri_Highest;
};


//-------------------------------------------------------

// Color Snippet Reference Data

USTRUCT(BlueprintType)
struct FVertexDetectColorSnippetReferenceDataStruct {

	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "The Vertex Color Snippet Data Asset that stores the snippet datas for this mesh, if any has been added from the Editor Widget when in editor. NOTE Can't be added in runtime! \nCan be used if you for instance want a bunch of zombies with different blood patterns on them, then you can store a bunch of different snippets and randomize which to use when spawning a zombie. When we look them up the data asset is loaded into memory but not cached so it can get GC since we only need to use it for the remainder of the execution. "))
	TMap<FString, TSoftObjectPtr<UVertexPaintColorSnippetDataAsset>> vertexColorSnippetsAndDataAssetsStoredOn;
};


//-------------------------------------------------------

// Serialized Colors Per LOD Struct - Used if setting in Callback Setting to return colors as serialized json string

USTRUCT(BlueprintType)
struct FVertexDetectSerializedColorsPerLODStruct {

	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin")
	int lod = 0;

	UPROPERTY(/*VisibleAnywhere, */BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin")
	FString colorsAtLODAsJSonString;
};


//-------------------------------------------------------

// Vertex Detect Mesh Data Per LOD Struct

USTRUCT(BlueprintType)
struct FVertexDetectMeshDataPerLODStruct {

	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin")
	int lod = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin")
	int amountOfVerticesAtLOD = 0;

	UPROPERTY(/*VisibleAnywhere, */BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "Only returns filled in the callback event if set to include vertex colors in the callback settings. "))
	TArray<FColor> meshVertexColorsPerLODArray;

	UPROPERTY(/*VisibleAnywhere, */BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin")
	FVertexDetectSerializedColorsPerLODStruct serializedVertexColorsData;

	UPROPERTY(/*VisibleAnywhere, */BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "Only returns filled in the callback event if set to include vertex positions in the callback settings. "))
	TArray<FVector> meshVertexPositionsInComponentSpacePerLODArray;

	UPROPERTY(/*VisibleAnywhere, */BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "Only returns filled in the callback event if set to include vertex normals in the callback settings. "))
	TArray<FVector> meshVertexNormalsPerLODArray;
};


//-------------------------------------------------------

// Color Snippet Data

USTRUCT(BlueprintType)
struct FVertexDetectColorSnippetDataStruct {

	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "Object the colors belong to"))
	TSoftObjectPtr<UObject> objectColorSnippetBelongsTo = nullptr;


	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "The Color Snippet Data Per LOD"))
	TArray<FVertexDetectMeshDataPerLODStruct> colorSnippetDataPerLOD;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "The Color Snippet Vertex Colors Serialized with Rapid JSon"))
	TArray<FVertexDetectSerializedColorsPerLODStruct> colorSnippetPerLODAsRapidJSon;
};


//-------------------------------------------------------

// Bone Colors Struct

USTRUCT(BlueprintType)
struct FVertexDetectBoneColorsStruct {

	GENERATED_BODY()

	UPROPERTY(/*VisibleAnywhere, */BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin")
	TArray<FColor> boneVertexColors;
};

//-------------------------------------------------------

// Vertex Paint Mesh Component Data Struct - Holds an Actors Components, their name and vertex colors etc.

USTRUCT(BlueprintType)
struct FVertexDetectMeshDataStruct {

	GENERATED_BODY()


	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "The Skeletal or Static Mesh Source"))
	TSoftObjectPtr<UObject> meshSource = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "Soft ptr to the Primitive Mesh Component"))
	TSoftObjectPtr<UPrimitiveComponent> meshComp = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "The Vertex Data Per LOD"))
	TArray<FVertexDetectMeshDataPerLODStruct> meshDataPerLOD;

	TArray<FColor> GetVertexColorDataAtLOD0() {

		if (meshDataPerLOD.Num() > 0)
			return meshDataPerLOD[0].meshVertexColorsPerLODArray;

		return TArray<FColor>();
	}

	TArray<FVector> GetVertexPositionDataAtLOD0() {

		if (meshDataPerLOD.Num() > 0)
			return meshDataPerLOD[0].meshVertexPositionsInComponentSpacePerLODArray;

		return TArray<FVector>();
	}

	TArray<FVector> GetVertexNormalDataAtLOD0() {

		if (meshDataPerLOD.Num() > 0)
			return meshDataPerLOD[0].meshVertexNormalsPerLODArray;

		return TArray<FVector>();
	}
};


//-------------------------------------------------------

// Found Vertex Position Data Struct

USTRUCT(BlueprintType)
struct FVertexDetectClosestVertexPositionStruct {

	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Runtime Vertex Color Paint and Detection Plugin")
	FVector closestVertexPositionWorld = FVector(ForceInitToZero);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Runtime Vertex Color Paint and Detection Plugin")
	FVector closestVertexPositionActorLocal = FVector(ForceInitToZero);
};


//-------------------------------------------------------

// Found Vertex Normala Data Struct

USTRUCT(BlueprintType)
struct FVertexDetectClosestVertexNormalStruct {

	GENERATED_BODY()


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Runtime Vertex Color Paint and Detection Plugin")
	FVector closestVertexNormal = FVector(ForceInitToZero);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Runtime Vertex Color Paint and Detection Plugin")
	FVector closestVertexNormalLocal = FVector(ForceInitToZero);
};


//-------------------------------------------------------

// Closest Vertex Data 

USTRUCT(BlueprintType)
struct FVertexDetectClosestVertexGeneralInfoStruct {

	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "The Closest Vertex inded of Hit Location"))
	int closestVertexIndex = -1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "The Closest Section the Closest Vertex was at. May be useful if painting / detecting on skeletal mesh components. "))
	int closestSection = -1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "The Closest Vertexes Colors"))
	FLinearColor closestVertexColors = FLinearColor(0, 0, 0, 0);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Runtime Vertex Color Paint and Detection Plugin")
	FVertexDetectClosestVertexPositionStruct closestVertexPositionInfo;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Runtime Vertex Color Paint and Detection Plugin")
	FVertexDetectClosestVertexNormalStruct closestVertexNormalInfo;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "The Material at the Closest Vertex"))
	UMaterialInterface* closestVertexMaterial = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "UVs at the Closest Vertex, each element in the array is for each UV Channel"))
	TArray<FVector2D> closestVertexUVAtEachUVChannel;
};


//-------------------------------------------------------

// Closest Vertex Surface Data - At Default

USTRUCT(BlueprintType)
struct FVertexDetectPhysicsSurfaceDataStruct_AtDefault {

	GENERATED_BODY()


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "If the Material is registered in the Material Data Asset to Include Default Channel. The Default should be set to be the surface that is on the Material when no colors are painted, for example if Cobble is Default, and then you can paint grass and sand on top of it that blends into it then it should be included. "))
	bool materialRegisteredToIncludeDefaultChannel = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Runtime Vertex Color Paint and Detection Plugin")
	TEnumAsByte<EPhysicalSurface> physicalSurfaceAtDefault = EPhysicalSurface::SurfaceType_Default;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Runtime Vertex Color Paint and Detection Plugin")
	FString physicalSurfaceAsStringAtDefault = "Default";

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Runtime Vertex Color Paint and Detection Plugin")
	float physicalSurfaceValueAtDefault = 0.0f;
};


//-------------------------------------------------------

// Closest Vertex Surface Data - At Red

USTRUCT(BlueprintType)
struct FVertexDetectPhysicsSurfaceDataStruct_AtRed {

	GENERATED_BODY()


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Runtime Vertex Color Paint and Detection Plugin")
	TEnumAsByte<EPhysicalSurface> physicalSurfaceAtRedChannel = EPhysicalSurface::SurfaceType_Default;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Runtime Vertex Color Paint and Detection Plugin")
	FString physicalSurfaceAsStringAtRedChannel = "None";

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Runtime Vertex Color Paint and Detection Plugin")
	float physicalSurfaceValueAtRedChannel = 0.0f;
};


//-------------------------------------------------------

// Closest Vertex Surface Data - At Green

USTRUCT(BlueprintType)
struct FVertexDetectPhysicsSurfaceDataStruct_AtGreen {

	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Runtime Vertex Color Paint and Detection Plugin")
	TEnumAsByte<EPhysicalSurface> physicalSurfaceAtGreenChannel = EPhysicalSurface::SurfaceType_Default;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Runtime Vertex Color Paint and Detection Plugin")
	FString physicalSurfaceAsStringAtGreenChannel = "None";

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Runtime Vertex Color Paint and Detection Plugin")
	float physicalSurfaceValueAtGreenChannel = 0.0f;
};


//-------------------------------------------------------

// Closest Vertex Surface Data - At Blue

USTRUCT(BlueprintType)
struct FVertexDetectPhysicsSurfaceDataStruct_AtBlue {

	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Runtime Vertex Color Paint and Detection Plugin")
	TEnumAsByte<EPhysicalSurface> physicalSurfaceAtBlueChannel = EPhysicalSurface::SurfaceType_Default;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Runtime Vertex Color Paint and Detection Plugin")
	FString physicalSurfaceAsStringAtBlueChannel = "None";

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Runtime Vertex Color Paint and Detection Plugin")
	float physicalSurfaceValueAtBlueChannel = 0.0f;
};


//-------------------------------------------------------

// Closest Vertex Surface Data - At Alpha

USTRUCT(BlueprintType)
struct FVertexDetectPhysicsSurfaceDataStruct_AtAlpha {

	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Runtime Vertex Color Paint and Detection Plugin")
	TEnumAsByte<EPhysicalSurface> physicalSurfaceAtAlphaChannel = EPhysicalSurface::SurfaceType_Default;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Runtime Vertex Color Paint and Detection Plugin")
	FString physicalSurfaceAsStringAtAlphaChannel = "None";

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Runtime Vertex Color Paint and Detection Plugin")
	float physicalSurfaceValueAtAlphaChannel = 0.0f;
};


//-------------------------------------------------------

// Most Painted Surface - Used by the component when detecting surface so we can get the most prominant one. 

USTRUCT(BlueprintType)
struct FVertexDetectMostDominantPhysicsSurfaceInfo {

	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "Which EPhysical Surface had the most Color Value. Based on what surfaces is registered in the Material Data Asset for RGBA"))
	TEnumAsByte<EPhysicalSurface> mostDominantPhysicsSurface = EPhysicalSurface::SurfaceType_Default;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin")
	FString mostDominantPhysicsSurfaceAsString = "Default";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "Amount that the most painted color had"))
	float mostDominantPhysicstSurfaceValue = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "Which channel it was that had the most painted color, R, B, G or A. It can be multiple in case the Most Painted Surface was made up of blendade surfaces from different channels, then this will be from those channels"))
	TArray<Enum_SurfaceAtChannel> mostDominantPhysicsSurfaceAtVertexColorChannels;
};


//-------------------------------------------------------

// Closest Vertex Surface Data

USTRUCT(BlueprintType)
struct FVertexDetectPhysicsSurfaceDataStruct {

	GENERATED_BODY()


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "If we got the Physics Surface at Material"))
	bool physicsSurfaceSuccessfullyAcquired = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "Information about the Most painted surface"))
	FVertexDetectMostDominantPhysicsSurfaceInfo mostDominantPhysicsSurfaceInfo;



	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Runtime Vertex Color Paint and Detection Plugin")
	FVertexDetectPhysicsSurfaceDataStruct_AtDefault physicsSurface_AtDefault;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Runtime Vertex Color Paint and Detection Plugin")
	FVertexDetectPhysicsSurfaceDataStruct_AtRed physicsSurface_AtRed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Runtime Vertex Color Paint and Detection Plugin")
	FVertexDetectPhysicsSurfaceDataStruct_AtGreen physicsSurface_AtGreen;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Runtime Vertex Color Paint and Detection Plugin")
	FVertexDetectPhysicsSurfaceDataStruct_AtBlue physicsSurface_AtBlue;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Runtime Vertex Color Paint and Detection Plugin")
	FVertexDetectPhysicsSurfaceDataStruct_AtAlpha physicsSurface_AtAlpha;



	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin")
	TArray<TEnumAsByte<EPhysicalSurface>> physicalSurfacesAsArray;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin")
	TArray<FString> surfacesAsStringArray;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin")
	TArray<float> surfaceValuesArray;
};


//-------------------------------------------------------

// Paint & Detect Fundementals

USTRUCT(BlueprintType)
struct FVertexDetectAndPaintFundementalsStruct {

	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin")
	FVector hitLocation = FVector(ForceInitToZero);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Runtime Vertex Color Paint and Detection Plugin")
	FVector hitLocationInComponentSpace = FVector(ForceInitToZero);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin")
	FVector hitNormal = FVector(ForceInitToZero);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "If Detecting/Painting on Skeletal Mesh then we need the bone. "))
	FName hitBone;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "What it was Painted/Detected with, can be used when getting back the Result to do different things depending on what it was, for exampel Foot, Boot, Gunshot. "))
	FString runTaskFor;
};


//-------------------------------------------------------

// Include Amount Of Painted Colors Of Each Channel

USTRUCT(BlueprintType)
struct FVertexDetectIncludeAmountOfPaintedColorsOfEachChannel {

	GENERATED_BODY()


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "If we get amount of vertices that had color for each channel, the avergage color of each channel, and amount of percent each channel has as well. Really useful if you have something like splaboon, where you want to know, how much in Percent of Red and Blue is on this Mesh above this minimum amount. \nNote that if we're running a paint job that doesn't have to loop through the vertices, like Detect with Get All Colors Only, Paint Job where we are Setting Colors instead of Adding, SetMeshColorsUFromColorArray, or Paint Color Snippet, then this will make it loop through the vertices after so it can correctly fill the struct. This means that a task like Paint Color Snippet, that is usually very fast can take a bit longer to finish, unlike tasks like Paint at Location which won't get their calculation speed affected since they're always looping through the Vertices. \nSo this can be set to False if you know that you're not going to be using the data. "))
	bool includeAmountOfPaintedColorsOfEachChannel = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "If Min this Amount of color value on R, G, B or A then they will be accounted for. "))
	float includeAmountOfPaintedColorsOfEachChannel_IfMinColorAmount = 0.01;
};


//-------------------------------------------------------

// Compare Mesh Colors To Color Array

USTRUCT(BlueprintType)
struct FVertexDetectCompareMeshColorsToColorArrayStruct {

	GENERATED_BODY()


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "If True, then in the Task Result Info struct we will fill a CompareMeshColorsToColorArrayResult where you can get the Matching Percent between the Meshes Current Vertex Colors (If paint job then colors after paint was applied) and the ones we send in here. \n\nThis is useful if you for instance have a Drawing Game, or a Tutorial where the player has to fill in a pattern that has been pre-filled, or just Mimick the pattern an AI is painting. "))
	bool compareMeshColorsToColorArrayAtLOD0 = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "Color Array we will compare it to. Have to match the Vertex Color Array Amount of the Mesh we're going to Paint/Detect. You can get both the vertex color array from any paint or detect job if you set to include them in the callback settings, so you can get them from a mesh and store it for later use. "))
	TArray<FColor> colorArrayToCompareWith;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "If a Vertex on colorArrayToCompareWith is above 0 on a channel, then the vertex on the mesh color array has to be within this range on that vertex channel to it for it to be considered the same. If a Vertex on colorArrayToCompareWith is 0, then the vertex on the mesh color array also has to be 0. \nFor instance if the colors on colorArrayToCompareWith is at 0.2 average on any channel, then with this set to 0.8, it should be considered matched even if the mesh is fully painetd at 1.0. Or if colorArrayToCompareWith is at 1.0, then you could set the range to be at a low value like 0.1 if you want to only have it be matched if the mesh colors is above 0.9.  "))
	float rangeMeshColorHasToBeWithin = 0.8;
};


//-------------------------------------------------------

// Compare Mesh Colors To Color Array

USTRUCT(BlueprintType)
struct FVertexDetectCompareMeshColorsToColorArrayResultStruct {

	GENERATED_BODY()


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "If we successfully compared the Vertex Colors from the Mesh, and the ones the user sent in to compare them with. "))
	bool successfullyComparedMeshColorsToColorArray = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "The Matching Percent between 0-100, this also includes vertices that had no color, as long as the mesh vertex color and the compare vertex colors where the same and within range, they're taken into account here. "))
	float matchingPercent = 0;

};


//-------------------------------------------------------

// Event Settings

USTRUCT(BlueprintType)
struct FVertexDetectEventSettings {

	GENERATED_BODY()


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "If we should run the Delegate whether it was a Success or Fail. "))
	bool runCallbackDelegate = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "If we should run the interface on the Actor we Painted / Detected On. "))
	bool runCallbackInterfaces = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "If we only return data for LOD0. This can save performance if the mesh is extremely heavy. There was a noticable difference on a mesh that had 980k vertices if only returned LOD0 and not the other LODs. Should only be false if you actually plan on using the data for the other LODs. "))
	bool includeVertexDataOnlyForLOD0 = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "If looping through all vertices then will get the colors of each bone at LOD0 as well and include it in the callback settings. \nIf True and it is a Task that by default don't loop through vertices like Paint Color Snippet, Paint Entire Mesh with Set, or Detect with GetColorsOnly, then this will still make it loop through the vertices afterwards so it can get data for this. So those tasks that are usually extremely quick will take a bit longer to calculate if this is true, so only recommend setting to true if you actually need the data. "))
	bool includeColorsOfEachBone = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "If we set amount of vertices that had color for each channel, and amount of percent each channel has as well. Really useful if you have something like splaboon, where you want to know, how much in Percent of Red and Blue is on this Mesh. \nNote that if we're running a paint job that doesn't have to loop through the vertices, like Detect with Get Colors Only, or Paint Color Snippet, then this will make it loop through the vertices after so it can correctly fill the struct. This means that a task like Paint Color Snippet that is usually extremely quick can take a bit longer to finish, unlike tasks like Paint at Location which won't get their calculation speed affected since they're already looping through the Vertices. "))
	FVertexDetectIncludeAmountOfPaintedColorsOfEachChannel includeAmountOfPaintedColorsOfEachChannel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "With this you can get the Matching Percent between the Meshes Current Vertex Colors (If paint job then colors after paint was applied) and the ones we send in here. \n\nThis is useful if you for instance have a Drawing Game, or a Tutorial where the player has to fill in a pattern that has been pre-filled, or just Mimick the pattern an AI is painting. "))
	FVertexDetectCompareMeshColorsToColorArrayStruct compareMeshColorsToColorArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "If we include the color data, if false, we can save some performance if the mesh is extremely heavy. There was a noticable difference on a mesh that had 980k vert, but if it's just 50k or so there's not much difference.  "))
	bool includeVertexColorData = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "If true then we will Serialize the Color Array for each LOD and return an array of strings, each representing color data for each LOD that can be De-Serialized to get the Color Data in Array format again. "))
	bool includeSerializedVertexColorData = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "If we include the position data, if false, we can save some performance if the mesh is extremely heavy. There was a noticable difference on a mesh that had 980k vertices if we didn't include everything. \nIf True and it is a Task that by default don't loop through all vertices, like Paint Color Snippet, Paint Entire Mesh with Set, or Detect with GetColorsOnly, then this will still make it loop through the vertices afterwards so it can get data for this. Only recommend setting to true if you actually need the data. "))
	bool includeVertexPositionData = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "If we include the Normal data, if false, we can save some performance if the mesh is extremely heavy. There was a noticable difference on a mesh that had 980k vertices if we didn't include everything. \nIf True and it is a Task that by default don't loop through vertices like Paint Color Snippet, Paint Entire Mesh with Set, or Detect with GetColorsOnly, then this will still make it loop through the vertices afterwards so it can get data for this. Only recommend setting to true if you actually need the data. "))
	bool includeVertexNormalData = false;
};


//-------------------------------------------------------

// Fundemental Struct

USTRUCT(BlueprintType)
struct FVertexDetectFundementalStruct {

	GENERATED_BODY()


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "Actor to Paint / Detect"))
	AActor* actor = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "Skeletal or Static Mesh Component to Paint / Detect"))
	UPrimitiveComponent* meshComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "You can set what you can to return. If you're Painting / Detecting on extremely large meshes like 1 million vertices big, then it can make a bit difference if you don't return any colors, position and normals. "))
	FVertexDetectEventSettings callbackSettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "Recommend running with Multithreading True so the game thread doesn't get affected by the Paint/Detect Job, meaning your FPS won't get affected as much. Remember to be aware of the Paint Task Queue, if you for instance paint every single frame on a very heavy mesh, then it won't have time to finish one task before another is being added to the queue, which means it will grow very large and you won't see the difference of your paint input until we get to that task, which may take a few seconds if the queue has grown very large. \nDepending on what you're doing you can have a small delay if painting every frame, so you paint every other frame, or you can add the next task when the current task is finished since you get a callback event. "))
	FVertexPaintMultiThreadSettingsStruct multiThreadSettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "If True, we ignore the task queue limit that's set in the Project Settings. It's not recommended to change this, as we don't want the task queue to grow to large as it will affect performance and quality if it becomes to big, if it grows extremely large than you should review the implementation where you paint/detect colors. \n\nThis is mostly for things like Save/Load Systems, where if you want to Load a Saved Game and have saved vertex colors on a bunch of Meshes, and need to queue up possibly several hundreds of paint tasks but just once. "))
	bool ignoreTaskQueueLimit = false;
};


//-------------------------------------------------------

// Estimated Color at Hit Location

USTRUCT(BlueprintType)
struct FVertexDetectEstimatedColorAtHitLocationStruct {

	GENERATED_BODY()


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "This will get the color as close to the actual Hit Location as possible, which is very useful if you have Meshes with few vertices and you want to run SFX or VFX based on the Color of the Hit location and not the closest vertex color in case there is a diff. \nThe task may take a bit longer to calculate if Mesh has alot of vertices, so you have the option to always get it, or only get it if the Mesh is below a certain amount of vertices. "))
	bool getEstimatedColorAtHitLocation = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "If True and getEstimatedColorAtHitLocation is True, then will only get the estimated color at hit location if the Mesh has Max Amount of Vertices than what's set below. "))
	bool onlyGetIfMeshHasMaxAmountOfVertices = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "Will only get the estimated color if the mesh this max amount of vertices"))
	int maxAmountOfVertices = 5000;
};


USTRUCT(BlueprintType)
struct FVertexDetectGetAverageColorStruct {

	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "The Range we will get the average colors from. Has to be above 0 to get the average color. "))
	float areaRangeToGetAvarageColorFrom = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "If the Vertex Normal and Hit Normal Dot is Min this then takes that vertex colors into account. -1 will take all vertices in the area into account, 1 only the vertices that has the same normal as the Hit Normal. So if you for instance want to get the average of one side of a wall, then you could have it to be 1, otherwise if it's -1 then it will take the vertices on the other side of the wall as well. "))
	float VertexNormalToHitNormal_MinimumDotProductToBeAccountedFor = -1;
};


//-------------------------------------------------------

// Vertex Detect Info

USTRUCT(BlueprintType)
struct FVertexDetectInfoStruct : public FVertexDetectFundementalStruct {

	GENERATED_BODY()


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "Can get the Average Color in an Area Range, which could be useful if you have for instance a Make-up game and want to see if the player has painted the majority of a characters cheek. Area Range has to be higher than 0 to get the average color. "))
	FVertexDetectGetAverageColorStruct getAverageColorInAreaSettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin")
	FVertexDetectAndPaintFundementalsStruct hitFundementals;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "This will Return the color as close to the actual Hit Location as possible, which is very useful if you have Meshes with few vertices and you want to run SFX or VFX based on the Color of the Hit location and not the closest vertex color in case there is a diff, which it might have if the mesh have few vertices. \nThe task may take a bit longer to calculate if Mesh has alot of vertices, so you have the option to only get it if the Mesh has a Max Amount of Vertices. \NOTE If getVertexColorSetting is set to onlyGetColors, but this is True, then it will still loop through colors. "))
	FVertexDetectEstimatedColorAtHitLocationStruct getEstimatedColorAtHitLocationSettings;
};



//-------------------------------------------------------

// Get Colors Only

USTRUCT(BlueprintType)
struct FVertexDetectGetColorsOnlyStruct : public FVertexDetectFundementalStruct {

	GENERATED_BODY()

	FVertexDetectGetColorsOnlyStruct() {

		callbackSettings.includeVertexColorData = true;
	}
};


//-------------------------------------------------------

// Vertex Painted Channel Result Struct

USTRUCT(BlueprintType)
struct FVertexDetectClosestVertexDataResultStruct {

	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Runtime Vertex Color Paint and Detection Plugin")
	bool closestVertexDataSuccessfullyAcquired = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Runtime Vertex Color Paint and Detection Plugin")
	FVertexDetectClosestVertexGeneralInfoStruct closestVertexGeneralInfo;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Runtime Vertex Color Paint and Detection Plugin")
	FVertexDetectPhysicsSurfaceDataStruct closestVertexPhysicalSurfaceInfo;
};


//-------------------------------------------------------

// Estimated Color At Hit Location Result Struct 

USTRUCT(BlueprintType)
struct FVertexDetectEstimatedColorAtHitLocationResultStruct {

	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "If we successfully got an estimated color close or at the Hit Location. "))
	bool estimatedColorAtHitLocationDataSuccessfullyAcquired = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "The Estimated Color Close or At Hit Location. Could be useful if you want to run SFX or VFX, and the mesh has few vertices. If you have alot of vertices and the hit location is often very close to the closest vertex then you might not need to use this. \nThe way we calculate this is by getting the direction from the closest vertex to the hit location, and the nearest vertices around the hit location. Then when finished looping through LOD0 and we have them all and the closest vertex, we check, which of the nearby vertices has the best dot from to the direction from closest to hit location, i.e. which is the most optimal to scale toward. "))
	FLinearColor estimatedColorAtHitLocation = FLinearColor(0, 0, 0, 0);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Runtime Vertex Color Paint and Detection Plugin")
	FVertexDetectPhysicsSurfaceDataStruct physicalSurfaceInfo;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "The location between the closest vertex and the optimal we scaled against to get the estimated color at hit location. "))
	FVector worldSpaceLocationWeEstimatedTheColorAt = FVector(ForceInitToZero);
};


//-------------------------------------------------------

// Color To Apply To Vertex Channels Depending On Physics Material

USTRUCT(BlueprintType)
struct FVertexPaintPhysicsSurfaceBlendingSettings {

	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin")
	TSoftObjectPtr<UMaterialInterface> associatedMaterial = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin")
	TArray<TEnumAsByte<EPhysicalSurface>> physicsSurfacesThatCanBlend;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin")
	float minAmountOnEachSurfaceToBeAbleToBlend = 0.3f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin")
	TEnumAsByte<EPhysicalSurface> physicsSurfaceToResultIn = EPhysicalSurface::SurfaceType_Default;
};


//-------------------------------------------------------

// Vertex Paint Material Data Asset Struct - When Creating Data Assets, inherit them from this which is where you will set material and what is at what at each channel

USTRUCT(BlueprintType)
struct FVertexPaintMaterialDataAssetStruct {

	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "Will include the default channel when running detection where it will return 1 atDefault value in the VertexPaintDetectionComponent if nothing is painted over it. 0.5 if one thing is painted, 0.33 if two things is fully painted over it etc. \nCan be set to false if you have a material setup where whatever you're painting completely hides what is default."))
	bool includeDefaultChannelWhenDetecting = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Runtime Vertex Color Paint and Detection Plugin")
	TEnumAsByte<EPhysicalSurface> atDefault = EPhysicalSurface::SurfaceType_Default;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Runtime Vertex Color Paint and Detection Plugin")
	TEnumAsByte<EPhysicalSurface> paintedAtRed = EPhysicalSurface::SurfaceType_Default;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Runtime Vertex Color Paint and Detection Plugin")
	TEnumAsByte<EPhysicalSurface> paintedAtGreen = EPhysicalSurface::SurfaceType_Default;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Runtime Vertex Color Paint and Detection Plugin")
	TEnumAsByte<EPhysicalSurface> paintedAtBlue = EPhysicalSurface::SurfaceType_Default;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Runtime Vertex Color Paint and Detection Plugin")
	TEnumAsByte<EPhysicalSurface> paintedAtAlpha = EPhysicalSurface::SurfaceType_Default;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin")
	TMap<TEnumAsByte<EPhysicalSurface>, FVertexPaintPhysicsSurfaceBlendingSettings> physicsSurfaceBlendingSettings;
};


//-------------------------------------------------------

// Parent Physics Surface Struct

USTRUCT(BlueprintType)
struct FVertexPaintParentPhysicsSurfaceStruct {

	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin")
	TArray <TEnumAsByte<EPhysicalSurface>> childSurfaces;
};


//-------------------------------------------------------

// Vertex Paint On Mesh Limits - Used so something may not go over a certain amount, for instance rain might only paint up to 0.5 on blue channel, while being in lake can paint up to 1 and  make you fully drenched. 

USTRUCT(BlueprintType)
struct FVertexPaintingLimitStruct {

	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "Paint Limits are useful if you have something like light Rain that should be able to make characters completely drenched but only a bit wet. Then you can use this to limit how much the Rain can paint on whatever Channel. "))
	bool usePaintLimits = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "If the Color was already over the limit before we Applied any, then should we clamp that color or not. Should be false if you for instance have a water drop that paints with limit of 0.75, that goes over an already fully watered surface, then the drop shouldn't change and make the surface has less water. "))
	bool limitColorIfTheColorWasAlreadyOverTheLimit = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin")
	float paintLimitOnRedChannel = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin")
	float paintLimitOnGreenChannel = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin")
	float paintLimitOnBlueChannel = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin")
	float paintLimitOnAlphaChannel = 1;
};


//-------------------------------------------------------

// Vertex Detect Cloth Physics - UChaosClothingInteractor::SetDamping()

USTRUCT(BlueprintType)
struct FVertexDetectClothPhysicsDampingSettingsStruct {

	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "The amount of global damping applied to the cloth velocities, also known as point damping. Point damping improves simulation stability, but can also cause an overall slow-down effect and therefore is best left to very small percentage amounts."))
	float SetDamping_dampingCoefficient = 0.01;
};


//-------------------------------------------------------

// Vertex Detect Cloth Physics - UChaosClothingInteractor::SetGravity()

USTRUCT(BlueprintType)
struct FVertexDetectClothPhysicsGravitySettingsStruct {

	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "Scale factor applied to the world gravity and also to the clothing simulation interactor gravity. Does not affect the gravity if set using the override below."))
	float SetGravity_gravityScale = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "Use the config gravity value instead of world gravity."))
	bool SetGravity_overrideGravity = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "The gravitational acceleration vector [cm/s^2]"))
	FVector SetGravity_gravityOverride = FVector(0, 0, -980.664978);
};


//-------------------------------------------------------

// Vertex Detect Cloth Physics - UChaosClothingInteractor::SetWind()

USTRUCT(BlueprintType)
struct FVertexDetectClothPhysicsWindSettingsStruct {

	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "The aerodynamic coefficient of drag applying on each particle. If an enabled Weight Map (Mask with values in the range [0;1]) targeting the 'Drag' is added to the cloth, then both the Low and High values will be used in conjunction with the per particle Weight stored in the Weight Map to interpolate the final value from them. Otherwise only the Low value is meaningful and sufficient to set the aerodynamic drag."))
	FVector2D SetWind_drag = FVector2D(0.07, 0.5);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "The aerodynamic coefficient of lift applying on each particle. If an enabled Weight Map (Mask with values in the range [0;1]) targeting the 'Lift' is added to the cloth, then both the Low and High values will be used in conjunction with the per particle Weight stored in the Weight Map to interpolate the final value from them. Otherwise only the Low value is meaningful and sufficient to set the aerodynamic lift."))
	FVector2D SetWind_lift = FVector2D(0.07, 0.5);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin")
	float SetWind_airDensity = 0.000001225f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin")
	FVector SetWind_windVelocity = FVector(0, 0, 0);
};


//-------------------------------------------------------

// Vertex Detect Cloth Physics - UChaosClothingInteractor::SetAnimDrive()

USTRUCT(BlueprintType)
struct FVertexDetectClothPhysicsAnimDriveSettingsStruct {

	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "The strength of the constraint driving the cloth towards the animated goal mesh. If an enabled Weight Map (Mask with values in the range [0;1]) targeting the 'Anim Drive Stiffness' is added to the cloth, then both the Low and High values will be used in conjunction with the per particle Weight stored in the Weight Map to interpolate the final value from them. Otherwise only the Low value is meaningful and sufficient to enable this constraint."))
	FVector2D SetAnimDrive_Stiffness = FVector2D(0, 1);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "The damping amount of the anim drive. If an enabled Weight Map (Mask with values in the range [0;1]) targeting the 'Anim Drive Damping' is added to the cloth, then both the Low and High values will be used in conjunction with the per particle Weight stored in the Weight Map to interpolate the final value from them. Otherwise only the Low value is sufficient to work on this constraint."))
	FVector2D SetAnimDrive_Damping = FVector2D(0, 1);
};


//-------------------------------------------------------

// Vertex Detect Cloth Physics - UChaosClothingInteractor::SetCollision()

USTRUCT(BlueprintType)
struct FVertexDetectClothPhysicsCollisionSettingsStruct {

	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "The added thickness of collision shapes."))
	float SetCollision_CollisionThickness = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "Friction coefficient for cloth - collider interaction."))
	float SetCollision_FrictionCoefficient = 0.8;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "Use continuous collision detection (CCD) to prevent any missed collisions between fast moving particles and colliders. This has a negative effect on performance compared to when resolving collision without using CCD."))
	bool SetCollision_UseCCD = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "Enable self collision."))
	float SetCollision_SelfCollisionThickness = 2;
};


//-------------------------------------------------------

// Vertex Detect Cloth Physics - UChaosClothingInteractor::SetLongRangeAttachment()

USTRUCT(BlueprintType)
struct FVertexDetectClothPhysicsLongRangeAttachmentSettingsStruct {

	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "The tethers' stiffness of the long range attachment constraints. The long range attachment connects each of the cloth particles to its closest fixed point with a spring constraint. This can be used to compensate for a lack of stretch resistance when the iterations count is kept low for performance reasons. Can lead to an unnatural pull string puppet like behavior. If an enabled Weight Map (Mask with values in the range [0;1]) targeting the 'Tether Stiffness' is added to the cloth, then both the Low and High values will be used in conjunction with the per particle Weight stored in the Weight Map to interpolate the final value from them. Otherwise only the Low value is meaningful and sufficient to enable this constraint. Use 0, 0 to disable."))
	FVector2D LongRangeAttachment_TetherThickness = FVector2D(1, 1);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "The limit scale of the long range attachment constraints (aka tether limit). If an enabled Weight Map (Mask with values in the range [0;1]) targeting the 'Tether Scale' is added to the cloth, then both the Low and High values will be used in conjunction with the per particle Weight stored in the Weight Map to interpolate the final value from them. Otherwise only the Low value is meaningful and sufficient to set the tethers' scale."))
	FVector2D LongRangeAttachment_TetherScale = FVector2D(1, 1);
};


//-------------------------------------------------------

// Vertex Detect Cloth Physics - UChaosClothingInteractor::SetMaterial()

USTRUCT(BlueprintType)
struct FVertexDetectClothPhysicsMaterialSettingsStruct {

	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "The Stiffness of segments constraints. Increase the iteration count for stiffer materials. If an enabled Weight Map (Mask with values in the range [0;1]) targeting the 'Edge Stiffness' is added to the cloth, then both the Low and High values will be used in conjunction with the per particle Weight stored in the Weight Map to interpolate the final value from them. Otherwise only the Low value is meaningful and sufficient to enable this constraint."))
	FVector2D Material_EdgeStiffness = FVector2D(1, 1);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "The Stiffness of cross segments and bending elements constraints. Increase the iteration count for stiffer materials. If an enabled Weight Map (Mask with values in the range [0;1]) targeting the 'Bend Stiffness' is added to the cloth, then both the Low and High values will be used in conjunction with the per particle Weight stored in the Weight Map to interpolate the final value from them. Otherwise only the Low value is meaningful and sufficient to enable this constraint."))
	FVector2D Material_BendingStiffness = FVector2D(1, 1);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "The stiffness of the surface area preservation constraints. Increase the iteration count for stiffer materials. If an enabled Weight Map (Mask with values in the range [0;1]) targeting the 'Bend Stiffness' is added to the cloth, then both the Low and High values will be used in conjunction with the per particle Weight stored in the Weight Map to interpolate the final value from them. Otherwise only the Low value is meaningful and sufficient to enable this constraint."))
	FVector2D Material_AreaStiffness = FVector2D(1, 1);
};


//-------------------------------------------------------

// Vertex Detect Cloth Physics - UChaosClothingInteractor::SetVelocityScale()

USTRUCT(BlueprintType)
struct FVertexDetectClothPhysicsVelocityScaleSettingsStruct {

	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "The amount of linear velocities sent to the local cloth space from the reference bone (the closest bone to the root on which the cloth section has been skinned, or the root itself if the cloth isn't skinned)."))
	FVector PhysicsVelocityScale_LinearVelocityScale = FVector(0.75, 0.75, 0.75);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "The amount of angular velocities sent to the local cloth space from the reference bone the closest bone to the root on which the cloth section has been skinned, or the root itself if the cloth isn't skinned)."))
	float PhysicVelocityScale_AngularVelocityScale = 0.75;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "The portion of the angular velocity that is used to calculate the strength of all fictitious forces (e.g. centrifugal force). This parameter is only having an effect on the portion of the reference bone's angular velocity that has been removed from the simulation via the Angular Velocity Scale parameter. This means it has no effect when AngularVelocityScale is set to 1 in which case the cloth is simulated with full world space angular velocitiesand subjected to the true physical world inertial forces. Values range from 0 to 2, with 0 showing no centrifugal effect, 1 full centrifugal effect, and 2 an overdriven centrifugal effect."))
	float PhysicsVelocityScale_FictitiousAngularVelocityScale = 1;
};


//-------------------------------------------------------

// Vertex Detect Cloth Physics - UChaosClothingInteractor::SetPressure()

USTRUCT(BlueprintType)
struct FVertexDetectClothPhysicsSetAirPressureStruct {

	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "Pressure force strength applied in the normal direction(use negative value to push toward backface) If an enabled Weight Map (Mask with values in the range [0;1]) targeting the Pressure is added to the cloth, then both the Low and High values will be used in conjunction with the per particle Weight stored in the Weight Map to interpolate the final value from them. Otherwise only the Low value is meaningful and sufficient to set the pressure. "))
	FVector2D SetPressure_Pressure = FVector2D(ForceInitToZero);
};


//-------------------------------------------------------

// Cloth Physics Settings At Vertex Color Struct

USTRUCT(BlueprintType)
struct FVertexDetectClothPhysicsSettingsAtVertexColorStruct {

	GENERATED_BODY()


	// Damping
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin")
	bool SetDamping = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin")
	FVertexDetectClothPhysicsDampingSettingsStruct clothDampingSettingsWithNoColorPaintedAtChannel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin")
	FVertexDetectClothPhysicsDampingSettingsStruct clothDampingSettingsWithFullColorPaintedAtChannel;


	// Gravity
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin")
	bool SetGravity = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin")
	FVertexDetectClothPhysicsGravitySettingsStruct clothGravitySettingsWithNoColorPaintedAtChannel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin")
	FVertexDetectClothPhysicsGravitySettingsStruct clothGravitySettingsWithFullColorPaintedAtChannel;


	// Wind
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin")
	bool SetWind = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin")
	FVertexDetectClothPhysicsWindSettingsStruct clothWindSettingsWithNoColorPaintedAtChannel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin")
	FVertexDetectClothPhysicsWindSettingsStruct clothWindSettingsWithFullColorPaintedAtChannel;


	// Anim Drive
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin")
	bool SetAnimDrive = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin")
	FVertexDetectClothPhysicsAnimDriveSettingsStruct clothAnimDriveSettingsWithNoColorPaintedAtChannel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin")
	FVertexDetectClothPhysicsAnimDriveSettingsStruct clothAnimDriveSettingsWithFullColorPaintedAtChannel;


	// Collision
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin")
	bool SetCollision = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin")
	FVertexDetectClothPhysicsCollisionSettingsStruct clothCollisionSettingsWithNoColorPaintedAtChannel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin")
	FVertexDetectClothPhysicsCollisionSettingsStruct clothCollisionSettingsWithFullColorPaintedAtChannel;


	// Long Range Attachment
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin")
	bool SetLongRangeAttachment = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin")
	FVertexDetectClothPhysicsLongRangeAttachmentSettingsStruct clothLongRangeAttachmentSettingsWithNoColorPaintedAtChannel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin")
	FVertexDetectClothPhysicsLongRangeAttachmentSettingsStruct clothLongRangeAttachmentSettingsWithFullColorPaintedAtChannel;


	// Material
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin")
	bool SetMaterial = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin")
	FVertexDetectClothPhysicsMaterialSettingsStruct clothMaterialSettingsWithNoColorPaintedAtChannel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin")
	FVertexDetectClothPhysicsMaterialSettingsStruct clothMaterialSettingsWithFullColorPaintedAtChannel;


	// Physics Velocity Scale
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin")
	bool SetPhysicsVelocityScale = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin")
	FVertexDetectClothPhysicsVelocityScaleSettingsStruct clothPhysicsVelocityScaleSettingsWithNoColorPaintedAtChannel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin")
	FVertexDetectClothPhysicsVelocityScaleSettingsStruct clothPhysicsVelocityScaleSettingsWithFullColorPaintedAtChannel;


	// Set Air Pressure
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin")
	bool SetAirPressure = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin")
	FVertexDetectClothPhysicsSetAirPressureStruct clothPhysicssAirPressureWithNoColorPaintedAtChannel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin")
	FVertexDetectClothPhysicsSetAirPressureStruct clothPhysicssAirPressureWithFullColorPaintedAtChannel;
};


//-------------------------------------------------------

// Cloth Settings Struct

USTRUCT(BlueprintType)
struct FVertexDetectClothSettingsStruct {

	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "What the Cloth Physics Settings should be on the Red Channel. If Red is 0-1, how a cloths physics should get affected. "))
	FVertexDetectClothPhysicsSettingsAtVertexColorStruct clothPhysicsSettingsAtRedChannel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "What the Cloth Physics Settings should be on the Green Channel. If Red is 0-1, how a cloths physics should get affected. "))
	FVertexDetectClothPhysicsSettingsAtVertexColorStruct clothPhysicsSettingsAtGreenChannel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "What the Cloth Physics Settings should be on the Blue Channel. If Red is 0-1, how a cloths physics should get affected. "))
	FVertexDetectClothPhysicsSettingsAtVertexColorStruct clothPhysicsSettingsAtBlueChannel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "What the Cloth Physics Settings should be on the Alpha Channel. If Red is 0-1, how a cloths physics should get affected. "))
	FVertexDetectClothPhysicsSettingsAtVertexColorStruct clothPhysicsSettingsAtAlphaChannel;
};


//-------------------------------------------------------

// Cloth Physics Settings At Vertex Color Struct

USTRUCT(BlueprintType)
struct FVertexDetectChaosClothPhysicsSettings {

	GENERATED_BODY()


	// Damping
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin")
	bool SetDamping = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin")
	FVertexDetectClothPhysicsDampingSettingsStruct clothDampingSettings;


	// Gravity
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin")
	bool SetGravity = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin")
	FVertexDetectClothPhysicsGravitySettingsStruct clothGravitySettings;


	// Wind
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin")
	bool SetWind = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin")
	FVertexDetectClothPhysicsWindSettingsStruct clothWindSettings;


	// Anim Drive
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin")
	bool SetAnimDrive = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin")
	FVertexDetectClothPhysicsAnimDriveSettingsStruct clothAnimDriveSettings;


	// Collision
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin")
	bool SetCollision = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin")
	FVertexDetectClothPhysicsCollisionSettingsStruct clothCollisionSettings;


	// Long Range Attachment
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin")
	bool SetLongRangeAttachment = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin")
	FVertexDetectClothPhysicsLongRangeAttachmentSettingsStruct clothLongRangeAttachmentSettings;


	// Material
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin")
	bool SetMaterial = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin")
	FVertexDetectClothPhysicsMaterialSettingsStruct clothMaterialSettings;


	// Physics Velocity Scale
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin")
	bool SetPhysicsVelocityScale = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin")
	FVertexDetectClothPhysicsVelocityScaleSettingsStruct clothPhysicsVelocityScaleSettings;


	// Air Pressure
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin")
	bool SetAirPressure = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin")
	FVertexDetectClothPhysicsSetAirPressureStruct clothPhysicssAirPressureSettings;

	// Reset function so we can have default values when the users are creating the structs, but when we're actually constructing the settings with all the combined vertex color channels settings etc. we can easily reset the struct we create so it's all based from 0, so we're not accidentically adding on to the default values. 
	void ResetAllClothSettings() {
		clothDampingSettings.SetDamping_dampingCoefficient = 0;

		clothGravitySettings.SetGravity_gravityOverride = FVector(ForceInitToZero);
		clothGravitySettings.SetGravity_gravityScale = 0;
		clothGravitySettings.SetGravity_overrideGravity = false;

		clothWindSettings.SetWind_airDensity = 0;
		clothWindSettings.SetWind_drag = FVector2D(ForceInitToZero);
		clothWindSettings.SetWind_lift = FVector2D(ForceInitToZero);
		clothWindSettings.SetWind_windVelocity = FVector(ForceInitToZero);

		clothAnimDriveSettings.SetAnimDrive_Damping = FVector2D(ForceInitToZero);
		clothAnimDriveSettings.SetAnimDrive_Stiffness = FVector2D(ForceInitToZero);

		clothCollisionSettings.SetCollision_CollisionThickness = 0;
		clothCollisionSettings.SetCollision_FrictionCoefficient = 0;
		clothCollisionSettings.SetCollision_SelfCollisionThickness = 0;
		clothCollisionSettings.SetCollision_UseCCD = false;

		clothLongRangeAttachmentSettings.LongRangeAttachment_TetherScale = FVector2D(ForceInitToZero);
		clothLongRangeAttachmentSettings.LongRangeAttachment_TetherThickness = FVector2D(ForceInitToZero);

		clothMaterialSettings.Material_AreaStiffness = FVector2D(ForceInitToZero);
		clothMaterialSettings.Material_BendingStiffness = FVector2D(ForceInitToZero);
		clothMaterialSettings.Material_EdgeStiffness = FVector2D(ForceInitToZero);

		clothPhysicsVelocityScaleSettings.PhysicsVelocityScale_FictitiousAngularVelocityScale = 0;
		clothPhysicsVelocityScaleSettings.PhysicsVelocityScale_LinearVelocityScale = FVector(ForceInitToZero);
		clothPhysicsVelocityScaleSettings.PhysicVelocityScale_AngularVelocityScale = 0;

		clothPhysicssAirPressureSettings.SetPressure_Pressure = FVector2D(ForceInitToZero);
	}
};


//-------------------------------------------------------

// Paint Color Condition - Is Bone

USTRUCT(BlueprintType)
struct FVertexDetectPaintColorCondition_IsBoneStruct {

	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "If Painting on Skeletal Mesh then can set to only Paint on specific Bone. If Static Mesh then we won't run this condition. Useful if you for instance have a Boss Fight and you should be able to Freeze exposed parts which could be just an Arm or a Leg etc. "))
	FName ifVertexIsAtBone = "None";
};


//-------------------------------------------------------

// Paint Color Condition - Is Within Color Range

USTRUCT(BlueprintType)
struct FVertexDetectPaintColorCondition_IsWithinColorRangeStruct {

	GENERATED_BODY()


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "If this is set to something other then Default, then we will for each Section/Material the Mesh is using, check if it's registered in the Editor Widget and what Physics Surface is set on each channel, then do the Within Color Range for whatever Channel that is. Very useful if several Materials might not be synched with eachother and have something like Puddle on different channels, so you don't have to remember which channel it's on. "))
	TEnumAsByte<EPhysicalSurface> ifChannelWithPhysicsSurfaceIsWithinColorRange = EPhysicalSurface::SurfaceType_Default;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "If physicsSurfaceWithinColorRange is Default, then we will use this Color Channel is a certain Range. Gives the Developer more control of when paint is applied. Can for instance be used to Mask out Areas where you don't want it to be applied, like if it's Raining and you don't want the underside of Rooftops to get wet, then you can paint them with a channel the material isn't using, and have a condition that the channel has to be at 0 for the Wetness to be applied. "))
	Enum_SurfaceAtChannel ifVertexColorChannelWithinColorRange = Enum_SurfaceAtChannel::RedChannel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "If Current Vertex Color is >= than this. Gives the Developer more control of when paint is applied. Can for instance be used to Mask out Areas where you don't want it to be applied, like if it's Raining and you don't want the underside of Rooftops to get wet, then you can paint them with a channel the material isn't using, and have a condition that the channel has to be at 0 for the Wetness to be applied. "))
	float ifCurrentVertexColorValueIsHigherOrEqualThan = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "If Current Vertex Color is <= than this. Gives the Developer more control of when paint is applied. Can for instance be used to Mask out Areas where you don't want it to be applied, like if it's Raining and you don't want the underside of Rooftops to get wet, then you can paint them with a channel the material isn't using, and have a condition that the channel has to be at 0 for the Wetness to be applied. "))
	float ifCurrentVertexColorValueIsLessOrEqualThan = 0;
};


//-------------------------------------------------------

// Paint Color Condition - Has Line Of Sight

USTRUCT(BlueprintType)
struct FVertexDetectPaintColorCondition_HasLineOfSightStruct {

	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "If Set to an Actor then will use this Actors Location at Task Creation. "))
	AActor* ifVertexHasLineOfSightToActor = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "If not set to use Line of Sight to Actor, then will use this. "))
	FVector ifVertexHasLineOfSightToPosition = FVector(ForceInitToZero);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "How far out from the vertex position we start the Trace toward the Position / Actor Location. If the Mesh Component you're trying to paint has collision that completely covers the Mesh and the Vertices, then this can be increased so you start tracing outside of that collision and doesn't hit itself. \nYou can also choose to ignore the Actor you're trying to paint, but then you may not get the desired result if the Position / Actor Location is on one side of the mesh component, and you expect only the vertices on that side of the mesh component to get painted. If ignoring then even the verts on the otherside will get painted since it can't hit itself. "))
	float distanceFromVertexPositionToStartTrace = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "Object Types to Trace if we have Line of Sight. "))
	TArray<TEnumAsByte<EObjectTypeQuery>> checkLineOfSightAgainstObjectTypes = { EObjectTypeQuery::ObjectTypeQuery1, EObjectTypeQuery::ObjectTypeQuery2, EObjectTypeQuery::ObjectTypeQuery3, EObjectTypeQuery::ObjectTypeQuery4, EObjectTypeQuery::ObjectTypeQuery5 };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "If we should ignore the Actor we're trying to paint, useful if the location you want to check for may be on the other side of the mesh component and you don't want it to hit itself"))
	bool ignoreMeshActorTryingToApplyPaintOn = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin")
	TArray<AActor*> lineOfSightTraceActorsToIgnore;
};


//-------------------------------------------------------

// Paint Color Condition

USTRUCT(BlueprintType)
struct FVertexDetectPaintColorConditionStruct {

	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "If Physics Surface or Color Channel is a certain Range. Gives the Developer more control of when paint is applied. Can for instance be used to Mask out Areas where you don't want it to be applied, like if it's Raining and you don't want the underside of Rooftops to get wet, then you can paint them with a channel the material isn't using, and have a condition that the channel has to be at 0 for the Wetness to be applied. "))
	TArray<FVertexDetectPaintColorCondition_IsWithinColorRangeStruct> ifVertexColorIsWithinRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "If Vertex has Line of Sight to something, for instance the Sun, then only Apply/Remove Colors then. Useful if you for instance only want to Dry a Mesh where the Sun is actually hitting the Mesh. \nThis will Trace to check if it has line of sight, so may make the Task take longer, especially if playing in the Editor. "))
	TArray<FVertexDetectPaintColorCondition_HasLineOfSightStruct> ifVertexHasLineOfSightTo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "If Painting on Skeletal Mesh then can set to only Paint on specific Bone. Useful if you for instance have a Boss Fight and you should be able to Freeze exposed parts which could be just an Arm or a Leg etc. "))
	TArray<FVertexDetectPaintColorCondition_IsBoneStruct> ifVertexIsOnBone;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "Whether we should apply a Different Color Strength if the Condition Failed. For instance if we want to Dry Mesh if in Line of Sight to a Sun, but it failed, the vertex is in a Shadow, then maybe we still want to Dry it, just by not as much. "))
	bool applyColorStrengthIfConditionIsNotMet = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "Whether we should apply a Different Color Strength if the Condition Failed. For instance if we want to Dry Mesh if in Line of Sight to a Sun, but it failed, the vertex is in a Shadow, then maybe we still want to Dry it, just by not as much. "))
	float colorStrengthIfConditionIsNotMet = 0;
};


//-------------------------------------------------------

//  Apply Vertex Colors Struct

USTRUCT(BlueprintType)
struct FVertexDetectApplyColorsUsingPhysicsSurfaceStruct {

	GENERATED_BODY()


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "This is useful so you don't have to hardcode what to paint on which Vertex Color Channel, but just want to paint a physics surface, if it exists, or if it doesn't exist but it is a Parent / Child of the one we  set. It's also useful if your Materials Vertex Color Channels isn't synched up, where for instance a Puddle could be on different Channels on different materials. It's also useful if you don't want to apply any colors at all if a surface isn't registered to the Material. It requires that the Material and what Physics Surface is on each channel is registered in the Editor Widget. Make sure to setup the Physics Surface Families as well, for example if you have Sand as a Parent, and Cobble-Sand as a Child, and a character that has Sand on their Red Vertex Channel, if they're walking over a floor with Cobble-Sand, it can Apply Colors on the Channel with Sand, i.e. the Red Channel."))
	bool applyVertexColorUsingPhysicsSurface = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "The Physics Surface you want to Paint/Remove Colors on. This can be a Parent or Child of a Parent Physics Surface as well, for example if you have Sand as a Parent, and Cobble-Sand as a Child, and a character that has Sand on their Red Vertex Channel. Then if they're walking over a floor with Cobble-Sand, it will Apply Colors on the Channel with Sand, i.e. the Red Channel. "))
	TEnumAsByte<EPhysicalSurface> physicsSurfaceToApply = EPhysicalSurface::SurfaceType_Default;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "How much paint strength we should Apply/Remove on the Channels that we could get colors to apply on, i.e. had physics surface registered. Should be between -1 - 1. If painting with Add and not Set, then this and paintStrengtOnChannelsWithoutThePhysicsSurface can't both be 0, since then no difference will occur so no reason to start the task. "))
	float paintStrengthToApply = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "Will check if the Physics Surface is a Child of a Parent, for instance if it's Cobble-Sand which could be a child of Parent Sand. Then if trying to Paint with Cobble-Sand, but it can't find any channel that has it, but it can find Sand, then it applies color on that channel. \nThis is useful on for instance characters that may have fundemental parent surfaces such as Sand, Mud etc. and can walk over different types of Sandy Surfaces like Asphalt-Sand, Cobble-Sand etc. and we want them to be able to Rub off on whatever channel that the character has Sand on. "))
	bool applyOnChannelsThatIsAParentOfPhysicsSurface = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "Checks if a Physics Surface we're trying to Paint is a part of a Registered Physics Surface Family, and has a Parent. For instance Cobble-Sand may have Sand registered as Parent. So if this is True and you're Painting with a Parent Physics Surface, for instance Sand, but there is no Sand but there is Cobble-Sand child, then that channel it's registered to will get painted. "))
	bool applyOnChannelsThatsChildOfPhysicsSurface = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "For instance if Cobble-Mud is set to be a Blendable Surface in the Editor Widget like in the Example Project, where Sand (Red) and Wet (Blue) channels can mix and form that, then we can take that into account and apply it to either the Parent and/or the Child of a Physics Surface depending if applyOnChannelsThatIsAParentOfPhysicsSurface and/or applyOnChannelsThatsChildOfPhysicsSurface is True. If we're trying to Paint with the Blendable Surface then we can Apply colors on the channels that made it blend. \nThis means that if you for instance have Mud on a Characters Channel, and they're walking over Cobble-Mud, then we can apply color on whatever channel Mud is on since it is a parent of Cobble-Mud. Same the other way around, if a wheel with Mud is driving over a Cobble Material that has Cobble-Mud registered then it can Apply color on whatever channels that results in Cobble-Mud since it is a child of Mud. "))
	bool applyOnChannelsThatResultsInBlendableSurfaces = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "If True then if was unsuccessfull in getting any colors to apply on any channel using physics surface, then we fallback to whatever that was set on the RGBA Channels. "))
	bool fallbackToRGBAColorsIfFailedToApplyColorsUsingPhysicsSurface = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "Color Condition to use on whatever Vertex Color Channels that was successfull in getting what colors to apply. "))
	FVertexDetectPaintColorConditionStruct paintUsingPhysicsSurfaceConditions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "With this you can Apply a Strength to the Channels that we where unsuccessfull in getting colors for, i.e. channels that didn't have the surface registered etc. Useful if for Example if you want to paint Puddle, and Remove on the other Channels since water should clean away stuff on them, then you could set this to a minus value. Should be between -1 - 1. If painting with Add and not Set, then this and paintStrengthToApply can't both be 0, since then no difference will occur so no reason to start the task. "))
	float paintStrengtOnChannelsWithoutThePhysicsSurface = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "Color Condition to use on whatever Vertex Color Channels that was un-successfull in getting what colors to apply, i.e. the ones that will use paintStrengtOnChannelsWithoutThePhysicsSurface. "))
	FVertexDetectPaintColorConditionStruct paintConditionsOnChannelsWithoutThePhysicsSurface;
};


//-------------------------------------------------------

//  Apply Vertex Colors Struct

USTRUCT(BlueprintType)
struct FVertexDetectRGBAStruct {

	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "Add will Add onto the existing colors, meaning if you set higher than 0 in strength, it will increase the vertex colors up to maximum 1, if less than 0, it decrease down to 0. If Set to Set Vertex Color, it will set the colors to the strength, if 0 it will set the vertex colors to the lowest 0 for instance. "))
	EApplyVertexColorSetting applyVertexColorSetting = EApplyVertexColorSetting::EAddVertexColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "This is useful so you don't have to hardcode what to paint on which Vertex Color Channel, but just want to paint a physics surface, if it exists. It's also useful if your Materials Vertex Color Channels isn't synched up, where for instance a Puddle could be on different Channels on different materials. It's also useful if you don't want to apply any colors at all if a surface isn't registered to the Material. It requires that the Material and what Physics Surface is on each channel is registered in the Editor Widget. Make sure to setup the Parent and Child Surfaces as well, for example if you have Mud as a Parent, and Cobble-Mud as a Child, and a character that has Mud on their Red Vertex Channel, if they're walking over a floor with Cobble-Mud, it can Apply Colors on the Channel with Mud, i.e. the Red Channel."))
	FVertexDetectApplyColorsUsingPhysicsSurfaceStruct applyColorsUsingPhysicsSurface;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "Strength on the Red Vertex Channel to Apply. Should be between -1.0 and 1.0. "))
	float redColor = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "Apply Color on the Red Channel if specified conditions are met."))
	FVertexDetectPaintColorConditionStruct redColorCondition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "Strength on the Green Vertex Channel to Apply. Should be between -1.0 and 1.0. "))
	float greenColor = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "Apply Color on the Green Channel if specified conditions are met."))
	FVertexDetectPaintColorConditionStruct greenColorCondition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "Strength on the Blue Vertex Channel to Apply. Should be between -1.0 and 1.0. "))
	float blueColor = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "Apply Color on the Blue Channel if specified conditions are met."))
	FVertexDetectPaintColorConditionStruct blueColorCondition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "Strength on the Alpha Vertex Channel to Apply. Should be between -1.0 and 1.0. "))
	float alphaColor = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "Apply Color on the Alpha Channel if specified conditions are met."))
	FVertexDetectPaintColorConditionStruct alphaColorCondition;
};


//-------------------------------------------------------

// Only Affect Vertices With Direction To Actor Or Location

USTRUCT(BlueprintType)
struct FVertexPaintOnlyAffectVerticesWithDirectionToActorOrLocation {

	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "If true then only Vertices where the Vertex Normal is above a Min Dot toward an Actor or Location will get affected. May be useful if you need a fast way to get control over on what side of a mesh to paint, like if you have a hose that is spraying water on a character, but you only want the part of the character that is facing the hose to get affected and not the backside. There is also an a Paint Condition with Line of Sight to an Actor and Location, the difference between that and this is that it will trace and see if something is blocking, which will make the task take longer. "))
	bool onlyAffectVerticesWithDirectionToActorOrLocation = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "If Vertex Normal to Actor or Location is at least above this Dot, only then the vertex will get affected. It can be between -1 and 1, where -1 is the opposite (i.e. if set to -1 then all vertices will get affected), 0 is straight to the side, and 1 is straight toward the location. Depending on what you're doing you may want to adjust this, for instance a fire hose spraying a wide amount of water over a large mesh, then you want all of the vertices within the AoE to get affected which means this may have to be at min 0.5"))
	float minDotProductToActorOrLocation = 0.8;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "If this is set then will use this Actors Location, otherwise the location below. "))
	AActor* actor = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "If Actor is set then will use that, otherwise fall back to this. "))
	FVector location = FVector(ForceInitToZero);
};


//-------------------------------------------------------

// Override Paint Up To LOD

USTRUCT(BlueprintType)
struct FVertexPaintOverridePaintUpToLOD {

	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "If we override the amount of LODs to Paint so only Paint up to a Certain Amount. Can save performance by alot. You can also set in the Editor Widget the Standard Amount of LODs to Paint on Skeletal and Static Meshes. "))
	bool overrideLODToPaintUpTo = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin")
	int amountOfLODsToPaint = 1;
};


//-------------------------------------------------------

// Vertex Paint

USTRUCT(BlueprintType)
struct FVertexPaintStruct : public FVertexDetectFundementalStruct {

	GENERATED_BODY()


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "Whether we should Apply the change in Vertex Colors from the Paint Job to the Mesh. You can set this to false if you want to go through the entire paint job and get information such as Amount of Painted Colors of Each Channel, Compare Mesh Colors % or Task Duration etc. to know what they would've been IF the paint job had applied the colors. \nShould be a rare use case but the Example Project BP_RainExample for instance uses it so it can get the Task Durations when Drying and Applying Rain without having to actually apply the colors.   "))
	bool applyPaintJobToVertexColors = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "If we override the amount of LODs to Paint so only Paint up to a Certain Amount. Can save performance by alot. You can also set in the Editor Widget the Standard Amount of LODs to Paint on Skeletal and Static Meshes. "))
	FVertexPaintOverridePaintUpToLOD overrideLOD;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "Whether Cloth Physics should get affected at all. "))
	bool affectClothPhysics = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin|Painting", meta = (ToolTip = "If you only want to be able to paint on just certain actors then add them here. \nUseful if you for instance have a faucet that drips and paints out puddles of water etc. with sphere traces but doesn't want the player to get painted as well if they walk into the sphere but only the floor. "))
	TArray<AActor*> canOnlyApplyPaintOnTheseActors;
};


//-------------------------------------------------------

// Vertex Paint Color Setting

USTRUCT(BlueprintType)
struct FVertexPaintColorSettingStruct : public FVertexPaintStruct {

	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "If we should Add or Set Vertex Colrs to be what we set here. The Strength to Apply to Colors should be between -1.0 to 1.0. If you Set vertex colors then 0 to 1 is what matters. "))
	FVertexDetectRGBAStruct applyVertexColorSettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "Whether to Limit Paint Color up to a certain amount, useful for stuff like light Rain that shouldn't make characters completely Drenched but just a bit wet. "))
	FVertexPaintingLimitStruct limitPaintColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "If true then only Vertices where the Vertex Normal is above a Min Dot toward an Actor or Location will get affected. May be useful if you need a fast way to get control over on what side of a mesh to paint, like if you have a hose that is spraying water on a character, but you only want the part of the character that is facing the hose to get affected and not the backside. There is also an a Paint Condition with Line of Sight to an Actor and Location, the difference between that and this is that it will trace and see if something is blocking, which will make the task take longer."))
	FVertexPaintOnlyAffectVerticesWithDirectionToActorOrLocation onlyAffectVerticesWithDirectionToLocation;
};


//-------------------------------------------------------

// Paint at Location Area Settings

USTRUCT(BlueprintType)
struct FVertexDetectPaintAtLocationAreaSettings {

	GENERATED_BODY()


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "Area of Effect of the Paint Job"))
	float areaOfEffect = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "Area that will always have Full Color and not get any falloff"))
	float areaOfEffectWithFullColor = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "Falloff from Center out to the edge of the Area of Effect"))
	float fallOffStrength = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "If the Vertex Normal and Hit Normal Dot is Min this then apply colors. Great if you for instance have a Snow Storm and you only want one side of a House or something to get Painted with Snow as the Particles hits it. \nIf 1, then only the vertices on the same side of the Hit Normal get affected, so if it's a wall only the same wall. With -0.1 the vertices around Corners can get painted as well, with -1 to Paint all Vertices within the Area of effect gets painted. Note that this means Paint can become applied on the other side of a wall as well which may be undesirable. "))
	float VertexNormalToHitNormal_MinimumDotProductToGetPainted_StaticMesh = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "If the Vertex Normal and Hit Normal Dot is Min this then apply colors. Great if you for instance have a Snow Storm and you only want one side of a House or something to get Painted with Snow as the Particles hits it. \nIf 1, then only the vertices on the same side of the Hit Normal get affected, so if it's a wall only the same wall. With -0.1 the vertices around Corners can get painted as well, with -1 to Paint all Vertices within the Area of effect gets painted. Note that this means Paint can become applied on the other side of a wall as well which may be undesirable. "))
	float VertexNormalToHitNormal_MinimumDotProductToGetPainted_SkeletalMesh = -1;
};


//-------------------------------------------------------

// Detect When Painting

USTRUCT(BlueprintType)
struct FVertexDetectWhenPaintingSettings {

	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "If we run Get Closest Vertex Data on Mesh and it's Event Before Painting. Useful if you for instance want to Run SFX based on the colors before we applied the Paint Job, like if a Rain Drop landed on a Dry Cap, so it's SFX could be that of the Dry Cap and not the Wet Cap, since it wasn't wet before the rain drop hit, but it got wet when it did. "))
	bool runGetClosestVertexDataOnMeshBeforeApplyingPaint = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "If we run Get Closest Vertex Data on Mesh and it's Event After Painting. "))
	bool runGetClosestVertexDataOnMeshAfterApplyingPaint = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "Can get the Average Color in an Area Range, which could be useful if you have for instance a Make-up game and want to see if the player has painted the majority of a characters cheek. Area Range has to be above 0 to get the average in that range. "))
	FVertexDetectGetAverageColorStruct getAverageColorInAreaSettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "If false it will use the Paint at Location Hit Fundementals. If true will use the detect fundementals passed through here. "))
	bool useCustomHitFundementalsForDetection = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "By having a seperate detect hit, you can Paint on one location, but Detect on another location, within the same job. For instance if you have something dripping on a wall, and you want to detect ahead of where you want to paint, and depending on the result shrink/enlarge the drip. "))
	FVertexDetectAndPaintFundementalsStruct detectHitFundementals;
};


//-------------------------------------------------------

// Vertex Paint At Location

USTRUCT(BlueprintType)
struct FVertexPaintAtLocationStruct : public FVertexPaintColorSettingStruct {

	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin")
	FVertexDetectAndPaintFundementalsStruct hitFundementals;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "Area of Effect of the Paint Job"))
	FVertexDetectPaintAtLocationAreaSettings paintAtAreaSettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "This will Return the color as close to the actual Hit Location as possible, which is very useful if you have Meshes with few vertices and you want to run SFX or VFX based on the Color of the Hit location and not the closest vertex color in case there is a diff, which it might have if the mesh have few vertices. \nThe task may take a bit longer to calculate if Mesh has alot of vertices, so you have the option to only get it if the Mesh has a Max Amount of Vertices. "))
	FVertexDetectEstimatedColorAtHitLocationStruct getEstimatedColorAtHitLocationSettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "If we should run Detection before and/or after Painting, useful if you for instance want to Run SFX before Painting, like if a Rain Drop landed on a Dry Cap, then the SFX of that should be that of a Dry Cap, then it should Paint where it hit Wet. Instead of making two seperate ones, a Detection and a Paint job, you can save performance but running the Detection before the Paint with this. You can even run the deteciton at a different hit locatino if you wish, so for instance if you have something dripping on a wall, and you want to detect ahead of where you want to paint, and depending on the result shrink/enlarge the drip.  "))
	FVertexDetectWhenPaintingSettings detection;
};


//-------------------------------------------------------

// Component To Check If Is Within Info

USTRUCT(BlueprintType)
struct FVertexPainthWithinArea_ComponentToCheckIfIsWithinInfo {

	GENERATED_BODY()


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "The Primitive Component to check if within, for example a Cube, Rectangle, Sphere or even a Skeletal Mesh if you opt for Complex Shape. "))
	UPrimitiveComponent* componentToCheckIfIsWithin = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "Which shape to use, i.e. shape of the Component passed in. Cube/Rectangle and Sphere is the Cheapest. Complex does a line trace on each vertex to check if its within it, which makes it more expensive, but can support any kind of shape. "))
	Enum_PaintWithinAreaShape paintWithinAreaShape = Enum_PaintWithinAreaShape::isSquareOrRectangleShape;

	FBoxSphereBounds componentToCheckIfIsWithin_CleanAggGeomBounds = FBoxSphereBounds();
	FBoxSphereBounds componentToCheckIfIsWithin_Bounds = FBoxSphereBounds();
	FVector componentToCheckIfIsWithin_ActorBounds_Origin = FVector(ForceInitToZero);
	FVector componentToCheckIfIsWithin_ActorBounds_Extent = FVector(ForceInitToZero);

	FTransform componentToCheckIfIsWithin_WorldTransform;

	FVector componentToCheckIfIsWithin_LocationInMeshComponentToPaintSpace = FVector(ForceInitToZero);
	FRotator componentToCheckIfIsWithin_RotationInMeshComponentToPaintSpace = FRotator(ForceInitToZero);
	FVector componentToCheckIfIsWithin_ScaleInMeshComponentToPaintSpace = FVector(ForceInitToZero);

	FVector componentToCheckIfIsWithin_RelativeScale = FVector(ForceInitToZero);

	float componentToCheckIfIsWithin_IsVertexWithinArea_CubeOrRectangle_SquareXHalfSize = 0;
	float componentToCheckIfIsWithin_IsVertexWithinArea_CubeOrRectangle_SquareYHalfSize = 0;
	float componentToCheckIfIsWithin_IsVertexWithinArea_CubeOrRectangle_SquareZHalfSize = 0;

	float componentToCheckIfIsWithin_IsVertexWithinArea_SphereShape = 0;

	FVector componentToCheckIfIsWithin_ForwardVector = FVector(ForceInitToZero);
	FVector componentToCheckIfIsWithin_RightVector = FVector(ForceInitToZero);
	FVector componentToCheckIfIsWithin_UpVector = FVector(ForceInitToZero);

	FVector componentToCheckIfIsWithin_ScaledBoxExtent = FVector(ForceInitToZero);

	bool componentToCheckIfIsWithin_IsBoxComponent = false;
	bool componentToCheckIfIsWithin_IsSphereComponent = false;
	bool componentToCheckIfIsWithin_IsStaticMeshComponent = false;
	bool componentToCheckIfIsWithin_IsSkeletalMeshComponent = false;
	bool componentToCheckIfIsWithin_IsSplineMeshComponent = false;
	bool componentToCheckIfIsWithin_IsLandscape = false;
};


//-------------------------------------------------------

// Vertex Paint On Mesh if within Area

USTRUCT(BlueprintType)
struct FVertexPainthWithinAreaStruct : public FVertexPaintColorSettingStruct {

	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "The Primitive Components to check if within, for example a Cube, Rectangle, Sphere or even a Skeletal Mesh if you opt for Complex Shape. "))
	TArray<FVertexPainthWithinArea_ComponentToCheckIfIsWithinInfo> componentsToCheckIfIsWithin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "Extra Extent to apply Paint, useful if for instance a character is standing in a body of water, and you want them to get wet slightly above where the water is as well, as if their clothes is soaking up the water. \nNOTE Only supports Square/Rectangle and Sphere Shapes, not Complex. "))
	float extraExtentToApplyPaintOn = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "Paint Within Area will get an Area of Effect which is its bounds extent for calculating the Falloff, this is the AoE in the center of the area that should get full color that doesn't get affected by Falloff. May be good if you for instance have a Torch and you want the center of the torch always for instance melt ice at full strength. "))
	float areaOfEffectWithFullColor = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "Paint Within Area supports Falloff as well, where you could have it so at the center of a Sphere you get more strength than at the edges. Can be useful if you have for instance a Torch that should Melt Snow with stronger effect at it's center. \nNote that with Cube/Rectangle it will scale the falloff to the outmost corner of the rectangle if using Spherical From Center. If using Gradiant Upward/Downward then it will scale it just to the Z Extent of the Bounds. "))
	float fallOffStrength = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "Whether we calculate the Falloff from the components center or from the components Z Height, so only the distance from the components Z will matter. This is great for large components like oceans if you want some falloff but the mesh being painted is very far from the components X and Y, but they won't matter since we only use the Z. "))
	EVertexPaintWithinAreaFallOffType falloffType = EVertexPaintWithinAreaFallOffType::SphericalFromCenter;
};


//-------------------------------------------------------

// Paint Entire Mesh Randomize Struct

USTRUCT(BlueprintType)
struct FVertexDetectPaintOnEntireMeshRandomizeSettingsStruct {

	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "If true then paints on unique Random Vertices over the Entire Mesh. "))
	bool paintAtRandomVerticesSpreadOutOverTheEntireMesh = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "If choosen to paint random vertices over the entire mesh then this is the % from 0-100. "))
	float paintAtRandomVerticesSpreadOutOverTheEntireMesh_PercentToPaint = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "If set to Not propogate LOD0 Colors to the Rest of the LODs, it will randomize for each LOD, meaning LOD0 can look pretty different from for instance LOD1 since they've both run the randomization and could've picked completely different vertices, but this option is the fastest. \nThe Modified Engine Method is copied from the engines own solution when you Mesh Paint in Editor Time but is modified so it can run in runtime and will loop through all vertices in LODs above 0 and make sure they get the color of the closest and most optimal LOD0 vertex. "))
	Enum_PaintEntireMeshAtRandomVertices_PropogateToLODsMethod paintAtRandomVerticesSpreadOutOverTheEntireMesh_PropogateLOD0ToAllLODsMethod = Enum_PaintEntireMeshAtRandomVertices_PropogateToLODsMethod::ModifiedEngineMethod;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "If True then we will only Randomize within an Area of Effect around the location set, and not bother using the Percent to Randomize over the Entire Mesh since we're just doing it within an area. Instead you have to set the Probability Factor yourself, how big the odds or that a vertex within the area will be painted. "))
	bool onlyRandomizeWithinAreaOfEffectAtLocation = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "The Area Of Effect to Randomize within. "))
	float onlyRandomizeWithinAreaOfEffectAtLocation_AreaOfEffect = 250;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "The Area of Effect based on this Location we should Randomize within. "))
	FVector onlyRandomizeWithinAreaOfEffectAtLocation_Location = FVector(ForceInitToZero);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "The Probability of a Vertex Within the Area to get randomly painted between 0-1. 0.1 = 10% chance a vertex within the area will get painted for instance. "))
	float onlyRandomizeWithinAreaOfEffectAtLocation_ProbabilityFactor = 0.1f;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "If True, then the Paint Job will use the Seed passed through here when Randomizing either on Entire Mesh or within Area of Effect. "))
	bool overrideRandomSeeds = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "The Seed to override with. "))
	FRandomStream seedToOverrideWith;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "No matter if you override the seed or not, this is the actual random seed used for the paint job. Can be used if the Server for instance want to send it to clients so they can paint entire mesh at random vertices with the same seed and get the same result. "))
	FRandomStream randomSeedsUsedInPaintJob;
};


//-------------------------------------------------------

// Vertex Paint On Entire Mesh

USTRUCT(BlueprintType)
struct FVertexPaintOnEntireMeshStruct : public FVertexPaintColorSettingStruct {

	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "Settings for Painting on Random Vertices over the Entire Mesh. Useful if you want to paint a mesh as if it's raining on it or something similar.  "))
	FVertexDetectPaintOnEntireMeshRandomizeSettingsStruct paintOnRandomVerticesSettings;
};


//-------------------------------------------------------

// Vertex Paint Color Snippet

USTRUCT(BlueprintType)
struct FVertexPaintColorSnippetStruct : public FVertexPaintStruct {

	GENERATED_BODY()


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "Color Snippet Tag To Paint, should be left to Default if you want to Randomize. Color Snippets can be created in the Editor Widget is very useful for Storing a snippet of exactly how a Skeletal or Static Mesh should look for later use, For instance Variations of Weapon Skins, Zombies, Military or even your Main Character and how they should look in a certain Level or Cutscene."))
	FGameplayTag colorSnippetTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "If colorSnippetTagToPaint isn't set we can Randomize a Snippet from under This Tag Category that is Registered to the Mesh, if there are any. For instance Zombies.Sewer to Randomize a Snippet under the Zombies Sewer Category. \nYou can select the Top Level Category if you want to randomize between All of the Available Snippets the Mesh has. "))
	FGameplayTag randomizeAnyColorSnippetTagUnderThisCategory;

	UPROPERTY(BlueprintReadOnly, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "The Color Snippet Tag but in FString Format"))
	FString colorSnippetID;

	// Gets set after we've loaded in the data asset so we in async task can de-serialize the color snippet from it and get the colors in TArray<FColor> Format. 
	UPROPERTY()
	UVertexPaintColorSnippetDataAsset* colorSnippetDataAssetSnippetIsStoredIn = nullptr;
};


//-------------------------------------------------------

// Set Mesh Component Vertex Colors

USTRUCT(BlueprintType)
struct FVertexPaintSetMeshComponentVertexColors : public FVertexPaintStruct {

	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "The Vertex Colors to Set at LOD0. "))
	TArray<FColor> vertexColorsAtLOD0ToSet;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin")
	bool printLogsToScreen = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin")
	float printLogsToScreen_Duration = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin")
	bool printLogsToOutputLog = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "If you want to print logs or callback events then you can pass through a component and it will run the events on that just like the other paint types. "))
	UVertexPaintDetectionComponent* optionalCallbackComponent = nullptr;
};


//-------------------------------------------------------

// Set Mesh Component Vertex Colors

USTRUCT(BlueprintType)
struct FVertexPaintSetMeshComponentVertexColorsUsingSerializedString : public FVertexPaintStruct {

	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "The Color Array Data that's been Serialized into a FString for LOD0. We will De-Serialize this on the Async Thread and if the number of elements matches what the meshComponent has, we will apply it. "))
	FString serializedColorDataAtLOD0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin")
	bool printLogsToScreen = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin")
	float printLogsToScreen_Duration = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin")
	bool printLogsToOutputLog = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "If you want to print logs or callback events then you can pass through a component and it will run the events on that just like the other paint types. "))
	UVertexPaintDetectionComponent* optionalCallbackComponent = nullptr;
};


//-------------------------------------------------------

// Avarage Color In Area Info

USTRUCT(BlueprintType)
struct FVertexDetectAvarageColorInAreaInfo {

	GENERATED_BODY()


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "The Avarage Vertex Colors within the Area when Detecting (if set an AoE), or the Avarage Vertex Colors when Painting at Location After Applying Vertex Colors. "))
	FLinearColor avarageVertexColorsWithinAreaOfEffect = FLinearColor(0, 0, 0, 0);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Runtime Vertex Color Paint and Detection Plugin")
	FVertexDetectPhysicsSurfaceDataStruct avaragePhysicalSurfaceInfoBasedOffTheClosestVertexMaterial;
};


//-------------------------------------------------------

// Amount of Painted Colors of Each Channel

USTRUCT(BlueprintType)
struct FVertexDetectAmountOfPaintedColorsOfEachChannel {

	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Runtime Vertex Color Paint and Detection Plugin")
	bool successfullyGotVerticesOfEachChannelAtTheMinAmount = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Runtime Vertex Color Paint and Detection Plugin")
	float amountOfVerticesWithAnyRedPaintedAtTheMinAmount = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Runtime Vertex Color Paint and Detection Plugin")
	float amountOfRedChannelPercentPaintedAtTheMinAmount = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Runtime Vertex Color Paint and Detection Plugin")
	float averageAmountOfRedColorAtTheMinAmount = 0;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Runtime Vertex Color Paint and Detection Plugin")
	float amountOfVerticesWithAnyGreenPaintedAtTheMinAmount = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Runtime Vertex Color Paint and Detection Plugin")
	float amountOfGreenChannelPercentPaintedAtTheMinAmount = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Runtime Vertex Color Paint and Detection Plugin")
	float averageAmountOfGreenColorAtTheMinAmount = 0;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Runtime Vertex Color Paint and Detection Plugin")
	float amountOfVerticesWithAnyBluePaintedAtTheMinAmount = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Runtime Vertex Color Paint and Detection Plugin")
	float amountOfBlueChannelPercentPaintedAtTheMinAmount = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Runtime Vertex Color Paint and Detection Plugin")
	float averageAmountOfBlueColorAtTheMinAmount = 0;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Runtime Vertex Color Paint and Detection Plugin")
	float amountOfVerticesWithAnyAlphaPaintedAtTheMinAmount = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Runtime Vertex Color Paint and Detection Plugin")
	float amountOfAlphaChannelPercentPaintedAtTheMinAmount = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Runtime Vertex Color Paint and Detection Plugin")
	float averageAmountOfAlphaColorAtTheMinAmount = 0;
};


//-------------------------------------------------------

// Vertex Colors on Each Bone

USTRUCT(BlueprintType)
struct FVertexDetectVertexColorsOnEachBone {

	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Runtime Vertex Color Paint and Detection Plugin")
	bool successFullyGotColorsForEachBone = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Runtime Vertex Color Paint and Detection Plugin")
	TMap<FName, FVertexDetectBoneColorsStruct> nameAndColorOfEachBone;
};


//-------------------------------------------------------

// Task Result

USTRUCT(BlueprintType)
struct FVertexDetectTaskResultInfo {

	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "If the Paint/Detect Task was Successfull"))
	bool taskSuccessfull = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "How long in Seconds the Task took to finish. \nCan be useful to sync paint jobs if you for instance is Painting many meshes at the same time, like if it's raining and you're running Paint Entire Mesh at Random Vertices. In those cases some tasks may finish much faster than others since they may have different amount of vertices, and if you start a new task when the old one is finished it means that some meshes will get wet too fast compared to other meshes. With this you can check, what is the highest duration a task ever took, and if a task finishes before that, maybe add a delay before you start a new task or something similar with the delay duration to be the difference to the finished task duration and the highest duration. "))
	float taskDuration = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "If Painted using Paint Component then it was used with this. There are some paint jobs like SetMeshComponentVertexColors and SetMeshComponentVertexColorsUsingSerializedString where using a component is optional. "))
	UVertexPaintDetectionComponent* associatedPaintComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "If the Finished Task was a Paint Job of some kind, and it changed any vertex colors. For example if you run paint at location at an area that already has the colors that you want to paint, then no difference would occur so we didn't apply any new colors, or if you tried to paint on a mesh that's already fully painted, or tried to remove colors if it didn't have any colors. "))
	bool anyVertexColorGotChangedByPaintJob = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "If you in the Callback Settings set to Compare Vertex Colors to a Color Array, to for exmaple check if the player has painted a certain pattern or something, then this will be the result of that. "))
	FVertexDetectCompareMeshColorsToColorArrayResultStruct compareMeshColorsToColorArrayResult;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "If Paint Job, then these are the Vertex Color Channels that got affected by that paint job, for instance if Adding Color, then any channel that was not set to 0 will be affected, if Set, then all channels. If painting with Physics Surface, then only the channel that had that physics surface registered. "))
	TArray<EVertexColorChannel> colorAppliedToVertexColorChannels;
};


//-------------------------------------------------------

// Additional Data To PassThrough - With this you can pass through data that you might need when the task is finished. Very useful if you have a series of tasks where later ones result may be dependent on the result of earlier in the series. Otherwise you would have to cache the data you want to use which may make the solution unclean. 

USTRUCT(BlueprintType)
struct FVertexDetectAdditionalDataToPassThrough {

	GENERATED_BODY()


	//------------- Extra Properties ---------------//

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "These can be used effectively if several tasks is dependent on the results of eachother, so you don't have to cache global variables but can just pass through the ones you want when the task is finished. "))
	AActor* passThrough_Actor1 = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "These can be used effectively if several tasks is dependent on the results of eachother, so you don't have to cache global variables but can just pass through the ones you want when the task is finished. "))
	AActor* passThrough_Actor2 = nullptr;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "These can be used effectively if several tasks is dependent on the results of eachother, so you don't have to cache global variables but can just pass through the ones you want when the task is finished. "))
	UPrimitiveComponent* passThrough_PrimitiveComponent1 = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "These can be used effectively if several tasks is dependent on the results of eachother, so you don't have to cache global variables but can just pass through the ones you want when the task is finished. "))
	UPrimitiveComponent* passThrough_PrimitiveComponent2 = nullptr;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "These can be used effectively if several tasks is dependent on the results of eachother, so you don't have to cache global variables but can just pass through the ones you want when the task is finished. "))
	bool passThrough_Bool1 = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "These can be used effectively if several tasks is dependent on the results of eachother, so you don't have to cache global variables but can just pass through the ones you want when the task is finished. "))
	bool passThrough_Bool2 = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "These can be used effectively if several tasks is dependent on the results of eachother, so you don't have to cache global variables but can just pass through the ones you want when the task is finished. "))
	bool passThrough_Bool3 = false;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "These can be used effectively if several tasks is dependent on the results of eachother, so you don't have to cache global variables but can just pass through the ones you want when the task is finished. "))
	uint8 passThrough_Byte1 = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "These can be used effectively if several tasks is dependent on the results of eachother, so you don't have to cache global variables but can just pass through the ones you want when the task is finished. "))
	uint8 passThrough_Byte2 = 0;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "These can be used effectively if several tasks is dependent on the results of eachother, so you don't have to cache global variables but can just pass through the ones you want when the task is finished. "))
	float passThrough_Float1 = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "These can be used effectively if several tasks is dependent on the results of eachother, so you don't have to cache global variables but can just pass through the ones you want when the task is finished. "))
	float passThrough_Float2 = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "These can be used effectively if several tasks is dependent on the results of eachother, so you don't have to cache global variables but can just pass through the ones you want when the task is finished. "))
	float passThrough_Float3 = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "These can be used effectively if several tasks is dependent on the results of eachother, so you don't have to cache global variables but can just pass through the ones you want when the task is finished. "))
	float passThrough_Float4 = 0;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "These can be used effectively if several tasks is dependent on the results of eachother, so you don't have to cache global variables but can just pass through the ones you want when the task is finished. "))
	int passThrough_Int1 = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "These can be used effectively if several tasks is dependent on the results of eachother, so you don't have to cache global variables but can just pass through the ones you want when the task is finished. "))
	int passThrough_Int2 = 0;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "These can be used effectively if several tasks is dependent on the results of eachother, so you don't have to cache global variables but can just pass through the ones you want when the task is finished. \nThis could for instance Location, Direction, Velocity, Normal etc. "))
	FVector passThrough_Vector1 = FVector(ForceInitToZero);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "These can be used effectively if several tasks is dependent on the results of eachother, so you don't have to cache global variables but can just pass through the ones you want when the task is finished. \nThis could for instance Location, Direction, Velocity etc. "))
	FVector passThrough_Vector2 = FVector(ForceInitToZero);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "These can be used effectively if several tasks is dependent on the results of eachother, so you don't have to cache global variables but can just pass through the ones you want when the task is finished. \nThis could for instance Location, Direction, Velocity etc. "))
	FVector passThrough_Vector3 = FVector(ForceInitToZero);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "These can be used effectively if several tasks is dependent on the results of eachother, so you don't have to cache global variables but can just pass through the ones you want when the task is finished. \nThis could for instance Location, Direction, Velocity etc. "))
	FVector passThrough_Vector4 = FVector(ForceInitToZero);


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "These can be used effectively if several tasks is dependent on the results of eachother, so you don't have to cache global variables but can just pass through the ones you want when the task is finished."))
	FRotator passThrough_Rotator1 = FRotator(ForceInitToZero);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "These can be used effectively if several tasks is dependent on the results of eachother, so you don't have to cache global variables but can just pass through the ones you want when the task is finished."))
	FRotator passThrough_Rotator2 = FRotator(ForceInitToZero);


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "These can be used effectively if several tasks is dependent on the results of eachother, so you don't have to cache global variables but can just pass through the ones you want when the task is finished. "))
	FString passThrough_String1 = "";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "These can be used effectively if several tasks is dependent on the results of eachother, so you don't have to cache global variables but can just pass through the ones you want when the task is finished. "))
	FString passThrough_String2 = "";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "These can be used effectively if several tasks is dependent on the results of eachother, so you don't have to cache global variables but can just pass through the ones you want when the task is finished. "))
	FName passThrough_Name1 = "";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "These can be used effectively if several tasks is dependent on the results of eachother, so you don't have to cache global variables but can just pass through the ones you want when the task is finished. "))
	FName passThrough_Name2 = "";


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "These can be used effectively if several tasks is dependent on the results of eachother, so you don't have to cache global variables but can just pass through the ones you want when the task is finished. "))
	TEnumAsByte<EPhysicalSurface> passThrough_PhysicsSurface1 = EPhysicalSurface::SurfaceType_Default;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "These can be used effectively if several tasks is dependent on the results of eachother, so you don't have to cache global variables but can just pass through the ones you want when the task is finished. "))
	TEnumAsByte<EPhysicalSurface> passThrough_PhysicsSurface2 = EPhysicalSurface::SurfaceType_Default;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "These can be used effectively if several tasks is dependent on the results of eachother, so you don't have to cache global variables but can just pass through the ones you want when the task is finished. "))
	UPhysicalMaterial* passThrough_PhysicalMaterial1 = nullptr;



	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "These can be used effectively if several tasks is dependent on the results of eachother, so you don't have to cache global variables but can just pass through the ones you want when the task is finished. "))
	TEnumAsByte<ECollisionChannel> passThrough_ObjectCollisionChannel = ECollisionChannel::ECC_WorldStatic;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "These can be used effectively if several tasks is dependent on the results of eachother, so you don't have to cache global variables but can just pass through the ones you want when the task is finished. "))
	TEnumAsByte<EObjectTypeQuery> passThrough_ObjectType = EObjectTypeQuery::ObjectTypeQuery1;



	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "These can be used effectively if several tasks is dependent on the results of eachother, so you don't have to cache global variables but can just pass through the ones you want when the task is finished. "))
	UObject* passThrough_UObject1 = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "These can be used effectively if several tasks is dependent on the results of eachother, so you don't have to cache global variables but can just pass through the ones you want when the task is finished. "))
	UObject* passThrough_UObject2 = nullptr;
};


//-------------------------------------------------------

// Calculate Colors Info

USTRUCT()
struct FCalculateColorsInfo {

	GENERATED_BODY()

	int32 taskID = -1;

	// Results
	FVertexDetectMeshDataStruct meshVertexData;
	FColor avarageColorWithinArea_Detect_BeforeApplyingColor = FColor();
	FColor avarageColorWithinArea_Detect_AfterApplyingColor = FColor();
	FColor avarageColorWithinArea_Paint = FColor();
	TMap<UClothingAssetBase*, FVertexDetectChaosClothPhysicsSettings> clothPhysicsSettings;
	bool paintWithinArea_TriedToApplyColorOnAnyVertex = false;
	bool shouldApplyVertexColors = false;
	int closestVertexFound_Paint = -1;
	int closestSectionFound_Paint = -1;
	UMaterialInterface* materialAtClosestPaintVertex = nullptr;
	int closestVertexFound_Detection = -1;
	int closestSectionFound_Detect = -1;
	UMaterialInterface* materialAtClosestDetectVertex = nullptr;
	TArray<FColorVertexBuffer*> newColorVertexBufferPerLOD;
	FColor closestVertexColor_BeforeApplyingColors_Detect = FColor();
	FColor closestVertexColor_AfterApplyingColors_Detect = FColor();
	FColor closestVertexColor_AfterApplyingColors_Paint = FColor();
	FVector closestVertexPositionToHitLocationAtMinLOD_World_Paint = FVector(ForceInitToZero);
	FVector closestVertexNormalToHitLocationAtMinLOD_World_Paint = FVector(ForceInitToZero);
	FVector closestVertexPositionToHitLocationAtMinLOD_World_Detect = FVector(ForceInitToZero);
	FVector closestVertexNormalToHitLocationAtMinLOD_World_Detect = FVector(ForceInitToZero);
	FVertexDetectAmountOfPaintedColorsOfEachChannel amountOfPaintedColorsOfEachChannelAbove0;
	FVertexDetectAmountOfPaintedColorsOfEachChannel amountOfPaintedColorsOfEachChannelAtMinAmount_BeforeApplyingColor;
	FVertexDetectAmountOfPaintedColorsOfEachChannel amountOfPaintedColorsOfEachChannelAtMinAmount_AfterApplyingColor;
	FVertexDetectVertexColorsOnEachBone vertexColorsOnEachBone;
	FLinearColor estimatedColorAtHitLocation_BeforeApplyingColors_Detect = FLinearColor();
	FLinearColor estimatedColorAtHitLocation_AfterApplyingColors_Detect = FLinearColor();
	FLinearColor estimatedColorAtHitLocation_AfterApplyingColors_Paint = FLinearColor();
	FVector estimatedColorAtHitLocation_LocationInWorldSpace_Paint = FVector(ForceInitToZero);
	FVector estimatedColorAtHitLocation_LocationInWorldSpace_Detect = FVector(ForceInitToZero);
	FVector estimatedColorAtHitLocation_VertexToLerpToward_LocationInWorldSpace_Paint = FVector(ForceInitToZero);
	FVector estimatedColorAtHitLocation_VertexToLerpToward_LocationInWorldSpace_Detect = FVector(ForceInitToZero);
	int estimatedColorAtHitLocation_VertexToLerpToward_Paint = -1;
	int estimatedColorAtHitLocation_VertexToLerpToward_Detect = -1;
	TArray<FVector> estimatedColorAtHitLocation_ClosestVertexWorldLocationsToTheHitLocation_Paint;
	TArray<FVector> estimatedColorAtHitLocation_ClosestVertexWorldLocationsToTheHitLocation_Detect;
	FVertexDetectTaskResultInfo taskResult;
	TArray<EVertexColorChannel> appliedToVertexColorChannels;

	// Ptrs
	AActor* vertexPaintDetectionHitActor = nullptr;
	UPrimitiveComponent* vertexPaintComponent = nullptr;
	UStaticMeshComponent* vertexPaintStaticMeshComponent = nullptr;
	USplineMeshComponent* vertexPaintSplineMeshComponent = nullptr;
	USkeletalMeshComponent* vertexPaintSkelComponent = nullptr;
	UObject* vertexPaintSourceMesh = nullptr;
	UWorld* worldTaskWasCreatedIn;

	// Settings
	UVertexPaintDetectionComponent* initiatedByComponent = nullptr;
	EVertexPaintDetectionType paintDetectionType;
	FVertexPaintStruct vertexPaintSettings;
	FVertexPaintColorSettingStruct paintOnMeshColorSettings;
	FVertexDetectFundementalStruct fundementalStruct;
	FVertexPaintAtLocationStruct paintOnMeshAtLocationSettings;
	FVertexDetectInfoStruct detectInfoSettings;
	FVertexPainthWithinAreaStruct paintOnMeshWithinAreaSettings;
	TArray<FVertexPainthWithinArea_ComponentToCheckIfIsWithinInfo> paintOnMeshWithinArea_ComponentsToCheckIfIsWithinInfo;
	FVertexPaintOnEntireMeshStruct paintOnEntireMeshSettings;
	FVertexPaintColorSnippetStruct paintOnColorSnippetSettings;
	FVertexPaintSetMeshComponentVertexColors setMeshComponentVertexColorsSettings;
	FVertexPaintSetMeshComponentVertexColorsUsingSerializedString setMeshComponentVertexColorsUsingSerializedStringSettings;
	FVertexDetectEstimatedColorAtHitLocationStruct estimatedColorAtHitLocationSettings;
	FVertexDetectGetColorsOnlyStruct getAllVertexColorsSettings;
	FVertexDetectAdditionalDataToPassThrough additionalDataToPassThrough;
	bool propogateLOD0ToAllLODs = false;
	float taskStartedTimeStamp = 0;
	int lodsToLoopThrough = 1;

	bool printLogsToScreen = false;
	float printLogsToScreen_Duration = 5;
	bool printLogsToOutputLog = false;

	bool drawVertexPositionDebugPoint = false;
	bool drawVertexNormalDebugArrow = false;
	bool drawVertexPositionDebugPointOnVertexThatGotPaintApplied = false;
	bool drawClothVertexPositionDebugPoint = false;
	float drawVertexesDebugPointsDuration = 0.25;

	bool drawGetClosestVertexDataDebugSymbols = false;
	float drawGetClosestVertexDataDebugSymbolsDuration = 5;
	bool drawGetEstimatedColorAtHitLocationDebugSymbols = false;
	float drawGetEstimatedColorAtHitLocationDebugSymbolsDuration = 5;

	bool drawPaintConditionsDebugSymbols = false;
	float drawPaintConditionsDebugSymbolsDuration = 0.25f;

	bool drawPaintWithinAreaDebugSymbols = false;
	float drawPaintWithinAreaDebugSymbolsDuration = 5;
};


//-------------------------------------------------------

// Calculate Colors Queue Array. Necessary so the TMap of painted tasks could be uproperty

USTRUCT()
struct FVertexPaintCalculateColorsQueueArrayStruct {

	GENERATED_BODY()

	UPROPERTY()
	TArray<FCalculateColorsInfo> calculateColorsQueueArray;
};
