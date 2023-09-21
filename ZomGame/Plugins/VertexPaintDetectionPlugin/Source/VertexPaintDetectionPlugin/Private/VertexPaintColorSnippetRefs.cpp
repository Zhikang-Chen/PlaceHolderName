// Copyright 2022 Alexander Floden, Alias Alex River. All Rights Reserved. 


#include "VertexPaintColorSnippetRefs.h"


//-------------------------------------------------------

// Get Object From Snippet ID

TSoftObjectPtr<UObject> UVertexPaintColorSnippetRefs::GetObjectFromSnippetID(FString snippetID) {

    if (snippetID.Len() <= 0) return TSoftObjectPtr<UObject>();


    for (auto& objectTemp : staticMeshesColorSnippets) {

        TArray<FString> colorSnippetsTemp;

        objectTemp.Value.vertexColorSnippetsAndDataAssetsStoredOn.GetKeys(colorSnippetsTemp);

        if (colorSnippetsTemp.Contains(snippetID))
            return objectTemp.Key;
    }

    for (auto& objectTemp : skeletalMeshesColorSnippets) {

        TArray<FString> colorSnippetsTemp;

        objectTemp.Value.vertexColorSnippetsAndDataAssetsStoredOn.GetKeys(colorSnippetsTemp);

        if (colorSnippetsTemp.Contains(snippetID))
            return objectTemp.Key;
    }

    return TSoftObjectPtr<UObject>();
}


//-------------------------------------------------------

// Get All Color Snippets And Data Asset For Object

TMap<FString, TSoftObjectPtr<UVertexPaintColorSnippetDataAsset>> UVertexPaintColorSnippetRefs::GetAllColorSnippetsAndDataAssetForObject(UObject* object) {

    if (!IsValid(object)) return TMap<FString, TSoftObjectPtr<UVertexPaintColorSnippetDataAsset>>();


    if (staticMeshesColorSnippets.Contains(object)) {

        return staticMeshesColorSnippets.FindRef(object).vertexColorSnippetsAndDataAssetsStoredOn;
    }

    else if (skeletalMeshesColorSnippets.Contains(object)) {

        return skeletalMeshesColorSnippets.FindRef(object).vertexColorSnippetsAndDataAssetsStoredOn;
    }

    return TMap<FString, TSoftObjectPtr<UVertexPaintColorSnippetDataAsset>>();
}


//-------------------------------------------------------

// Remove Snippet Object

void UVertexPaintColorSnippetRefs::RemoveSnippetObject(UObject* object) {

    if (staticMeshesColorSnippets.Contains(object)) {

        staticMeshesColorSnippets.Remove(object);
    }

    if (skeletalMeshesColorSnippets.Contains(object)) {

        skeletalMeshesColorSnippets.Remove(object);
    }
}


//-------------------------------------------------------

// Remove Color Snippet

void UVertexPaintColorSnippetRefs::RemoveColorSnippet(FString snippetID) {

    if (snippetID.Len() <= 0) return;


    auto mesh = GetObjectFromSnippetID(snippetID);

    if (Cast<USkeletalMesh>(mesh.Get())) {

        if (skeletalMeshesColorSnippets.Contains(mesh.Get())) {

            // If there are more then only removes the specifc snippet reference
            if (skeletalMeshesColorSnippets.FindRef(mesh.Get()).vertexColorSnippetsAndDataAssetsStoredOn.Num() > 1) {

                auto snippetWithDataAssetsTemp = skeletalMeshesColorSnippets.FindRef(mesh.Get());

                snippetWithDataAssetsTemp.vertexColorSnippetsAndDataAssetsStoredOn.Remove(snippetID);

                skeletalMeshesColorSnippets.Add(mesh.Get(), snippetWithDataAssetsTemp);
            }
            else {

                // If this is the last one then removes the entire TMap for this object. 
                skeletalMeshesColorSnippets.Remove(mesh.Get());
            }
        }
    }

    else if (Cast<UStaticMesh>(mesh.Get())) {

        if (staticMeshesColorSnippets.Contains(mesh.Get())) {

            // If there are more then only removes the specifc snippet reference
            if (staticMeshesColorSnippets.FindRef(mesh.Get()).vertexColorSnippetsAndDataAssetsStoredOn.Num() > 1) {

                auto snippetWithDataAssetsTemp = staticMeshesColorSnippets.FindRef(mesh.Get());

                snippetWithDataAssetsTemp.vertexColorSnippetsAndDataAssetsStoredOn.Remove(snippetID);

                staticMeshesColorSnippets.Add(mesh.Get(), snippetWithDataAssetsTemp);
            }
            else {

                // If this is the last one then removes the entire TMap for this object. 
                staticMeshesColorSnippets.Remove(mesh.Get());
            }
        }
    }
}


//-------------------------------------------------------

// Contains Color Snippet

bool UVertexPaintColorSnippetRefs::ContainsColorSnippet(FString snippetID) {

    if (snippetID.Len() <= 0) return false;


    auto mesh = GetObjectFromSnippetID(snippetID);

    if (Cast<USkeletalMesh>(mesh.Get())) {

        if (skeletalMeshesColorSnippets.Contains(mesh.Get())) {

            return true;
        }
    }

    else if (Cast<UStaticMesh>(mesh.Get())) {

        if (staticMeshesColorSnippets.Contains(mesh.Get())) {

            return true;
        }
    }

    return false;
}
