// Copyright 2022 Alexander Floden, Alias Alex River. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include <Net/UnrealNetwork.h>
#include "VertexPaintClothOverlapComponent.generated.h"


class USkeletalMeshComponent;
class UClothingAssetBase;


USTRUCT()
struct FVertexPaintClothOverlapComponentsStruct {

	GENERATED_BODY()

	TArray<UPrimitiveComponent*> overlappingPrimitiveComponents;
};


UCLASS(Blueprintable, BlueprintType, ClassGroup = (Custom))
class VERTEXPAINTDETECTIONPLUGIN_API UVertexPaintClothOverlapComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UVertexPaintClothOverlapComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	UFUNCTION(BlueprintPure, Category = "Cloth Overlap Settings", meta = (ToolTip = "Returns the Cloth Objects currently set to sphere trace. "))
		TArray<TEnumAsByte<EObjectTypeQuery>> GetClothObjectsToSphereTrace() { return clothObjectsToSphereTrace; };

	UFUNCTION(BlueprintCallable, Category = "Cloth Overlap Settings", meta = (ToolTip = "Sets the Cloth Objects it should sphere trace to. "))
		void SetClothObjectsToSphereTrace(TArray<TEnumAsByte<EObjectTypeQuery>> objects) { clothObjectsToSphereTrace = objects; };

	UFUNCTION(BlueprintCallable, Category = "Cloth Overlap Settings", meta = (ToolTip = "Activates / DeActivates Cloth Overlap Tracing. Useful if you don't want to have it activated all the time but enable it when you want to have more attention to detail on the cloth, like in a cutscene or something similar. "))
		void SetClothOverlapTracingEnabled(bool enableClothTracing);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_ClothOverlapTracingEnabledRep, Category = "Cloth Overlap Settings", meta = (ToolTip = "If we should run the traces"))
		bool clothOverlapTracingEnabled = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cloth Overlap Settings", meta = (ToolTip = "Size of each sphere trace on the cloth vertex position. Can be larger if you opt to only trace every other vertex or similar. "))
		float clothVertexTraceRadius = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cloth Overlap Settings", meta = (ToolTip = "0 or 1 = Traces every vertex, 2 every other vertex etc. \nCan be used to optimize the tracing!"))
		int clothVertexTraceIntervall = 7;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cloth Overlap Settings", meta = (ToolTip = "Show the sphere trace locations"))
		bool debugClothSphereTraces = false;

private:

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const;

	UFUNCTION()
		virtual void OnRep_ClothOverlapTracingEnabledRep();

	void ActivateClothOverlapTracing();
	void DeActivateClothOverlapTracing();

	UPROPERTY()
		USkeletalMeshComponent* parentSkeletalMeshComponent;

	UPROPERTY()
		TArray<UClothingAssetBase*> parentSkeletalMeshComponentClothingAssets;

	UPROPERTY(EditDefaultsOnly, Category = "Cloth Overlap Settings", meta = (ToolTip = "Can set which channels it should overlap here, so in case you want it to overlap other channels other than the skeletal mesh component its attached to that is possible. "))
	TArray<TEnumAsByte<EObjectTypeQuery>> clothObjectsToSphereTrace;
	
	UPROPERTY()
		TMap<AActor*, FVertexPaintClothOverlapComponentsStruct> clothOverlappingActorsAndComponent;

	UPROPERTY()
		TMap<UClothingAssetBase*, FTransform> clothBoneTransformsInComponentSpaceTemp;

	UPROPERTY()
		TMap<UClothingAssetBase*, FQuat> clothBoneQuaternionsInComponentSpaceTemp;
};
