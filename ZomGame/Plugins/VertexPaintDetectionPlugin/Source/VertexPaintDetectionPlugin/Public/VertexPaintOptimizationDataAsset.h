// Copyright 2022 Alexander Floden, Alias Alex River. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "VertexPaintOptimizationDataAsset.generated.h"


class UStaticMesh;
class USkeletalMesh;

//-------------------------------------------------------

// Paint On LOD Settings Struct

USTRUCT(BlueprintType)
struct FPaintOnLODSettingsStruct {

	GENERATED_BODY()

		UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Runtime Vertex Color Paint and Detection Plugin")
		int maxAmountOfLODsToPaint = 0;
};


//-------------------------------------------------------

UCLASS(Blueprintable, BlueprintType)
class VERTEXPAINTDETECTIONPLUGIN_API UVertexPaintOptimizationDataAsset : public UDataAsset {

	GENERATED_BODY()


public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Runtime Vertex Color Paint and Detection Plugin|Optimization|Paint on LODs", meta = (ToolTip = "How many LODs to Paint on Static Meshes that are Added here, given that it has that many LODs. Also if the amount set is the max amount that should be stored. Meshes that aren't added here will be painted on all their LODs. "))
		TMap<UStaticMesh*, FPaintOnLODSettingsStruct> staticMeshNumOfLODsToPaint;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Runtime Vertex Color Paint and Detection Plugin|Optimization|Paint on LODs", meta = (ToolTip = "How many LODs to Paint on Static Meshes that are Added here, given that it has that many LODs. Also if the amount set is the max amount that should be stored. Meshes that aren't added here will be painted on all their LODs. "))
		TMap<USkeletalMesh*, FPaintOnLODSettingsStruct> skeletalMeshNumOfLODsToPaint;
};
