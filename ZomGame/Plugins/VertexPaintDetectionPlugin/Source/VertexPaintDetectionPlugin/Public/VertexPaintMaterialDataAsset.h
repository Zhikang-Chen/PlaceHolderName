// Copyright 2022 Alexander Floden, Alias Alex River. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "VertexPaintDetectionStructs.h"
#include "VertexPaintMaterialDataAsset.generated.h"


/**
 *
 */
UCLASS(Blueprintable, BlueprintType)
class VERTEXPAINTDETECTIONPLUGIN_API UVertexPaintMaterialDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:


	//---------- REGISTERED MATERIALS ----------//

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "If the Material is added in the paint on material TMap. Soft UMaterialInterface Ptr so it can be used with UMaterialInterface directly gotten from GetSoftPointerObjectsOfClass without resolving and casting, i.e. bringing them into memory. "))
		bool IsMaterialAddedToPaintOnMaterialDataAsset(TSoftObjectPtr<UMaterialInterface> material);

	UFUNCTION(BlueprintCallable, Category = "Runtime Vertex Color Paint and Detection Plugin")
		void RemoveFromVertexPaintMaterialInterface(TSoftObjectPtr<UMaterialInterface> material);

	UFUNCTION(BlueprintCallable, Category = "Runtime Vertex Color Paint and Detection Plugin")
		void AddToVertexPaintMaterialInterface(TSoftObjectPtr<UMaterialInterface> material, FVertexPaintMaterialDataAssetStruct materialDataAssetStruct) { vertexPaintMaterialInterfaces.Add(material, materialDataAssetStruct); }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Runtime Vertex Color Paint and Detection Plugin")
		TMap<TSoftObjectPtr<UMaterialInterface>, FVertexPaintMaterialDataAssetStruct> GetVertexPaintMaterialInterface() { return vertexPaintMaterialInterfaces; }


	UMaterialInterface* GetRegisteredMaterialInstanceOrParentMaterial(UMaterialInterface* material);



	//---------- PARENT & CHILD PHYSICS SURFACES ----------//

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Runtime Vertex Color Paint and Detection Plugin")
		TMap<TEnumAsByte<EPhysicalSurface>, FVertexPaintParentPhysicsSurfaceStruct> GetPhysicsSurfaceFamilies() { return physicsSurfaceFamilies; }

	UFUNCTION(BlueprintCallable, Category = "Runtime Vertex Color Paint and Detection Plugin", meta = (ToolTip = "Checks if physicalSurface is either a Parent itself, or a Child of a Family of Physics Surfaces. physicalSurface to check can't be Default. "))
		TArray<TEnumAsByte<EPhysicalSurface>> GetPhysicsSurfaceFamilyParentsOfPhysicsSurface(TEnumAsByte<EPhysicalSurface> physicalSurface);


#if WITH_EDITOR

	UFUNCTION(BlueprintCallable, Category = "Runtime Vertex Color Paint and Detection Plugin")
		void AddPhysicsSurfaceFamily(TEnumAsByte<EPhysicalSurface> parentPhysicsSurface, FVertexPaintParentPhysicsSurfaceStruct parentPhysicsSurfaceStruct) { physicsSurfaceFamilies.Add(parentPhysicsSurface, parentPhysicsSurfaceStruct); }

	UFUNCTION(BlueprintCallable, Category = "Runtime Vertex Color Paint and Detection Plugin")
		void RemovePhysicsSurfaceFamily(TEnumAsByte<EPhysicalSurface> parentPhysicsSurface);

	UFUNCTION(BlueprintCallable, Category = "Runtime Vertex Color Paint and Detection Plugin")
		void AddChildToPhysicsSurfaceFamily(TEnumAsByte<EPhysicalSurface> parentPhysicsSurface, TEnumAsByte<EPhysicalSurface> childPhysicsSurface);

	UFUNCTION(BlueprintCallable, Category = "Runtime Vertex Color Paint and Detection Plugin")
		void RemoveChildFromPhysicsSurfaceFamily(TEnumAsByte<EPhysicalSurface> parentPhysicsSurface, TEnumAsByte<EPhysicalSurface> childPhysicsSurface);

	UFUNCTION(BlueprintCallable, Category = "Runtime Vertex Color Paint and Detection Plugin")
		void ClearAllPhysicsSurfaceFamilies() { physicsSurfaceFamilies.Empty(); }

#endif
	

private:

	// TMap of All Materials that uses Vertex Colors
	UPROPERTY(VisibleAnywhere, Category = "Runtime Vertex Color Paint and Detection Plugin")
		TMap<TSoftObjectPtr<UMaterialInterface>, FVertexPaintMaterialDataAssetStruct> vertexPaintMaterialInterfaces;


	UPROPERTY(VisibleAnywhere, Category = "Runtime Vertex Color Paint and Detection Plugin")
		TMap<TEnumAsByte<EPhysicalSurface>, FVertexPaintParentPhysicsSurfaceStruct> physicsSurfaceFamilies;
};
