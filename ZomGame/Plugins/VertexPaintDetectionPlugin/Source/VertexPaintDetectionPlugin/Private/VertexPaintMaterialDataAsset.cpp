// Copyright 2022 Alexander Floden, Alias Alex River. All Rights Reserved. 

#include "VertexPaintMaterialDataAsset.h"


//-------------------------------------------------------

// Is Material Added To Paint On Material Data Asset

bool UVertexPaintMaterialDataAsset::IsMaterialAddedToPaintOnMaterialDataAsset(TSoftObjectPtr<UMaterialInterface> material) {

	if (!material) return false;

	TArray<TSoftObjectPtr<UMaterialInterface>> storedMaterialsTemp;
	GetVertexPaintMaterialInterface().GenerateKeyArray(storedMaterialsTemp);

	// When moving materials around to different folder, we couldn't do a simple .Contains() since it didn't return true eventhough the data asset had updated the material to the latest path. So we had to restrt the editor. This fixes that issue without having to load the material. 
	for (auto materialsTemp : storedMaterialsTemp) {

		if (materialsTemp.ToSoftObjectPath().ToString() == material.ToSoftObjectPath().ToString())
			return true;
	}

	// return GetVertexPaintMaterialInterface().Contains(material);

	return false;
}


//-------------------------------------------------------

// Remove From Vertex Paint Material Interface

void UVertexPaintMaterialDataAsset::RemoveFromVertexPaintMaterialInterface(TSoftObjectPtr<UMaterialInterface> material) {


	// if (vertexPaintMaterialInterfaces.Contains(material))
	
	// NOTE Doesn't have a .Contains check here because then we can't remove it if the material was Forced deleted and the TMap had null
	vertexPaintMaterialInterfaces.Remove(material);
}


//-------------------------------------------------------

// Get Parents Of Physics Surface

TArray<TEnumAsByte<EPhysicalSurface>> UVertexPaintMaterialDataAsset::GetPhysicsSurfaceFamilyParentsOfPhysicsSurface(TEnumAsByte<EPhysicalSurface> physicalSurface) {


	TArray<TEnumAsByte<EPhysicalSurface>> parentPhysicsSurfaces_Local;

	if (physicalSurface == EPhysicalSurface::SurfaceType_Default) return parentPhysicsSurfaces_Local;


	TArray<TEnumAsByte<EPhysicalSurface>> parentPhysicsSurfacesTemp;
	physicsSurfaceFamilies.GetKeys(parentPhysicsSurfacesTemp);

	TArray<FVertexPaintParentPhysicsSurfaceStruct> childPhysicsSurfacesTemp;
	physicsSurfaceFamilies.GenerateValueArray(childPhysicsSurfacesTemp);


	for (int i = 0; i < parentPhysicsSurfacesTemp.Num(); i++) {

		// If registered children contain the surface, or the Parent itself Is the Physical Surface we're checking
		if (childPhysicsSurfacesTemp[i].childSurfaces.Contains(physicalSurface)) {

			parentPhysicsSurfaces_Local.Add(parentPhysicsSurfacesTemp[i]);

			// UE_LOG(LogTemp, Warning, TEXT("Child Surface Constains Parent Surface: %i  Had physicalSurface Param: %i "), parentPhysicsSurfacesTemp[i].GetValue(), physicalSurface.GetValue());
		}

		else if (parentPhysicsSurfacesTemp[i] == physicalSurface) {

			parentPhysicsSurfaces_Local.Add(parentPhysicsSurfacesTemp[i]);

			// UE_LOG(LogTemp, Warning, TEXT("Parent Matches Physicsal Surface Param: %i "), physicalSurface.GetValue());
		}
	}

	return parentPhysicsSurfaces_Local;
}


//-------------------------------------------------------

// Get Registered Material Or Parent

UMaterialInterface* UVertexPaintMaterialDataAsset::GetRegisteredMaterialInstanceOrParentMaterial(UMaterialInterface* material) {

	if (!IsValid(material)) return nullptr;

	if (GetVertexPaintMaterialInterface().Contains(material)) {

		return material;
	}

	else {

		// If not registered, then if it's an instance, then replaces material with the parent and checks that. Makes it possible so you don't have to register every single instance but can if you wish only register the parent
		if (auto materialInstanceTemp = Cast<UMaterialInstance>(material)) {

			return materialInstanceTemp->Parent;


			if (!GetVertexPaintMaterialInterface().Contains(material))
				return nullptr;
		}
		else {

			return nullptr;
		}
	}
}


#if WITH_EDITOR

//-------------------------------------------------------

// Add Child To Parent Physics Surfaces

void UVertexPaintMaterialDataAsset::AddChildToPhysicsSurfaceFamily(TEnumAsByte<EPhysicalSurface> parentPhysicsSurface, TEnumAsByte<EPhysicalSurface> childPhysicsSurface) {

	FVertexPaintParentPhysicsSurfaceStruct parentPhysicsSurfacesTemp;

	if (physicsSurfaceFamilies.Contains(parentPhysicsSurface)) {

		parentPhysicsSurfacesTemp = physicsSurfaceFamilies.FindRef(parentPhysicsSurface);
		parentPhysicsSurfacesTemp.childSurfaces.Add(childPhysicsSurface);

		physicsSurfaceFamilies.Add(parentPhysicsSurface, parentPhysicsSurfacesTemp);
	}

	else {

		parentPhysicsSurfacesTemp.childSurfaces.Add(childPhysicsSurface);

		physicsSurfaceFamilies.Add(parentPhysicsSurface, parentPhysicsSurfacesTemp);
	}
}


//-------------------------------------------------------

// Remove Child From Parent Physics Surface

void UVertexPaintMaterialDataAsset::RemoveChildFromPhysicsSurfaceFamily(TEnumAsByte<EPhysicalSurface> parentPhysicsSurface, TEnumAsByte<EPhysicalSurface> childPhysicsSurface) {


	if (physicsSurfaceFamilies.Contains(parentPhysicsSurface)) {

		if (physicsSurfaceFamilies.FindRef(parentPhysicsSurface).childSurfaces.Contains(childPhysicsSurface)) {

			FVertexPaintParentPhysicsSurfaceStruct parentPhysicsSurfacesTemp;

			parentPhysicsSurfacesTemp = physicsSurfaceFamilies.FindRef(parentPhysicsSurface);
			parentPhysicsSurfacesTemp.childSurfaces.Remove(childPhysicsSurface);

			if (parentPhysicsSurfacesTemp.childSurfaces.Num() > 0)
				physicsSurfaceFamilies.Add(parentPhysicsSurface, parentPhysicsSurfacesTemp);
			else
				physicsSurfaceFamilies.Remove(parentPhysicsSurface);
		}
	}
}


//-------------------------------------------------------

// Remove Parent Physics Surface

void UVertexPaintMaterialDataAsset::RemovePhysicsSurfaceFamily(TEnumAsByte<EPhysicalSurface> parentPhysicsSurface) {

	if (physicsSurfaceFamilies.Contains(parentPhysicsSurface)) {

		physicsSurfaceFamilies.Remove(parentPhysicsSurface);
	}
}


#endif