// Copyright 2022 Alexander Floden, Alias Alex River. All Rights Reserved. 


#include "VertexPaintDetectionEdSubsystem.h"
#include "VertexPaintDetectionSettings.h"
#include "VertexPaintFunctionLibrary.h"
#include "RapidJsonFunctionLibrary.h"

#include "Engine/SkeletalMesh.h"
#include "Rendering/SkeletalMeshModel.h"
#include "Rendering/SkeletalMeshLODModel.h"
#include "Rendering/SkeletalMeshRenderData.h"

#include "Engine/StaticMesh.h"
#include "Components/StaticMeshComponent.h"
#include "StaticMeshResources.h"
#include "FileHelpers.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetRegistry/IAssetRegistry.h"

#include "VertexPaintMaterialDataAsset.h"
#include "VertexPaintOptimizationDataAsset.h"
#include "VertexPaintColorSnippetDataAsset.h"
#include "VertexPaintColorSnippetRefs.h"

#include "Components/Button.h"
#include "Components/ComboBoxString.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "Framework/Notifications/NotificationManager.h"

#include "GameplayTagsManager.h"
#include "GameplayTagsEditorModule.h"
#include "Templates/SharedPointer.h"
#include <fstream>
#include <string>
#include <vector>
#include "Misc/Paths.h"

#if ENGINE_MAJOR_VERSION == 5

#if ENGINE_MINOR_VERSION >= 2
#include "StaticMeshComponentLODInfo.h"
#endif

#endif

//--------------------------------------------------------

// Initialize

void UVertexPaintDetectionEdSubsystem::Initialize(FSubsystemCollectionBase& Collection) {

	UE_LOG(LogTemp, Log, TEXT("Runtime Vertex Color Paint and Detection Plugin Editor Subsystem Initialized"));
}


//--------------------------------------------------------

// Save Package Wrapper

bool UVertexPaintDetectionEdSubsystem::SavePackageWrapper(UObject* packageToSave) {

	if (!packageToSave) return false;
	if (!packageToSave->GetPackage()) return false;


	TArray<UPackage*> packages;
	packages.Add(packageToSave->GetPackage());

	return UEditorLoadingAndSavingUtils::SavePackages(packages, false);
}


//--------------------------------------------------------

// Add Editor Notification

void UVertexPaintDetectionEdSubsystem::AddEditorNotification(FString notification) {

	float notificationDuration_Local = 2;

	auto vertexPaintDetectionSettings = GetDefault<UVertexPaintDetectionSettings>();

	if (vertexPaintDetectionSettings) {

		// If set not to show notifications then just returns
		if (!vertexPaintDetectionSettings->vertexPaintAndDetectionPlugin_EditorWidgetNotificationEnabled)
			return;

		notificationDuration_Local = vertexPaintDetectionSettings->vertexPaintAndDetectionPlugin_EditorWidgetNotificationDuration;
	}


	FNotificationInfo Info(FText::FromString(notification));

	//How long before the widget automatically starts to fade out (in seconds)
	Info.ExpireDuration = notificationDuration_Local;

	// A larger, bolder font for super important warnings or something!
	// Info.bUseLargeFont = true;
	// Info.bFireAndForget
	// Info.bUseThrobber
	// Info.Hyperlink
	// Info.Image
	// Info.FadeInDuration = 1.0f;
	// Info.FadeOutDuration = 1.0f;
	// Info.WidthOverride = 500.0f;

	FSlateNotificationManager::Get().AddNotification(Info);
}


//--------------------------------------------------------

// Set Engine Specific Button Settings

void UVertexPaintDetectionEdSubsystem::SetEngineSpecificButtonSettings(TArray<UButton*> buttons, TArray<UComboBoxString*> comboBoxes) {

	if (buttons.Num() <= 0) return;


#if ENGINE_MAJOR_VERSION == 5

	// It turns out Buttons in UE5 has really dark tint by default, and outline of 1 which i have to turn off here

	for (auto buttonTemp : buttons) {

		FButtonStyle buttonStyleTemp;

#if ENGINE_MINOR_VERSION >= 2
		buttonStyleTemp = buttonTemp->GetStyle();
#else
		buttonStyleTemp = buttonTemp->WidgetStyle;
#endif


		buttonStyleTemp.Normal.TintColor = FSlateColor(FLinearColor::White);
		buttonStyleTemp.Normal.OutlineSettings.Width = 0;

		buttonStyleTemp.Hovered.TintColor = FSlateColor(FLinearColor::White);
		buttonStyleTemp.Hovered.OutlineSettings.Width = 0;

		buttonStyleTemp.Pressed.TintColor = FSlateColor(FLinearColor::White);
		buttonStyleTemp.Pressed.OutlineSettings.Width = 0;

		buttonStyleTemp.Disabled.TintColor = FSlateColor(FLinearColor::White);
		buttonStyleTemp.Disabled.OutlineSettings.Width = 0;
		buttonStyleTemp.Disabled.DrawAs = ESlateBrushDrawType::NoDrawType;


#if ENGINE_MINOR_VERSION >= 2
		buttonTemp->SetStyle(buttonStyleTemp);
#else
		buttonTemp->WidgetStyle = buttonStyleTemp;
#endif
	}

	for (auto comboBoxTemp : comboBoxes) {

		FComboBoxStyle comboBoxStyleTemp;

#if ENGINE_MINOR_VERSION >= 2
		comboBoxStyleTemp = comboBoxTemp->GetWidgetStyle();
#else
		comboBoxStyleTemp = comboBoxTemp->WidgetStyle;
#endif

		comboBoxStyleTemp.ComboButtonStyle.ButtonStyle.Normal.DrawAs = ESlateBrushDrawType::Box;
		comboBoxStyleTemp.ComboButtonStyle.ButtonStyle.Normal.OutlineSettings.Width = 0;

		comboBoxStyleTemp.ComboButtonStyle.ButtonStyle.Hovered.DrawAs = ESlateBrushDrawType::Box;
		comboBoxStyleTemp.ComboButtonStyle.ButtonStyle.Hovered.OutlineSettings.Width = 0;

		comboBoxStyleTemp.ComboButtonStyle.ButtonStyle.Pressed.DrawAs = ESlateBrushDrawType::Box;
		comboBoxStyleTemp.ComboButtonStyle.ButtonStyle.Pressed.OutlineSettings.Width = 0;

		comboBoxStyleTemp.ComboButtonStyle.ButtonStyle.Disabled.DrawAs = ESlateBrushDrawType::Box;
		comboBoxStyleTemp.ComboButtonStyle.ButtonStyle.Disabled.OutlineSettings.Width = 0;


#if ENGINE_MINOR_VERSION >= 2
		comboBoxTemp->SetWidgetStyle(comboBoxStyleTemp);
#else
		comboBoxTemp->WidgetStyle = comboBoxStyleTemp;
#endif
	}

#endif
}


//--------------------------------------------------------

// Add Skeletal Mesh And Amount Of LODs To Paint

void UVertexPaintDetectionEdSubsystem::AddSkeletalMeshAmountOfLODsToPaintByDefault(USkeletalMesh* skeletalMesh, int maxAmountOfLODsToPaint, bool& success) {

	success = false;

	if (!skeletalMesh || !IsValid(UVertexPaintFunctionLibrary::GetOptimizationDataAsset())) return;


	FPaintOnLODSettingsStruct paintOnLODSettingsStruct_Local;
	paintOnLODSettingsStruct_Local.maxAmountOfLODsToPaint = maxAmountOfLODsToPaint;

	success = true;

	if (UVertexPaintFunctionLibrary::GetOptimizationDataAsset()->skeletalMeshNumOfLODsToPaint.Contains(skeletalMesh))
		AddEditorNotification(FString::Printf(TEXT("Updated Skeletal Mesh %s LODs to Store and Paint!"), *skeletalMesh->GetName()));
	else
		AddEditorNotification(FString::Printf(TEXT("Added Skeletal Mesh %s LODs to Store and Paint!"), *skeletalMesh->GetName()));

	UVertexPaintFunctionLibrary::GetOptimizationDataAsset()->skeletalMeshNumOfLODsToPaint.Add(skeletalMesh, paintOnLODSettingsStruct_Local);


	SavePackageWrapper(UVertexPaintFunctionLibrary::GetOptimizationDataAsset());
}


//--------------------------------------------------------

// Remove Skeletal Mesh And Amount Of LODs To Paint

void UVertexPaintDetectionEdSubsystem::RemoveSkeletalMeshAmountOfLODsToPaintByDefault(USkeletalMesh* skeletalMesh, bool& success) {

	success = false;

	if (!IsValid(UVertexPaintFunctionLibrary::GetVertexPaintMaterialDataAsset())) {

		return;
	}

	if (UVertexPaintFunctionLibrary::GetOptimizationDataAsset()->skeletalMeshNumOfLODsToPaint.Contains(skeletalMesh)) {

		UVertexPaintFunctionLibrary::GetOptimizationDataAsset()->skeletalMeshNumOfLODsToPaint.Remove(skeletalMesh);

		success = true;

		SavePackageWrapper(UVertexPaintFunctionLibrary::GetOptimizationDataAsset());
	}
}


//--------------------------------------------------------

// Add Static Mesh And Amount Of LODs To Paint

void UVertexPaintDetectionEdSubsystem::AddStaticMeshAndAmountOfLODsToPaintByDefault(UStaticMesh* staticMesh, int maxAmountOfLODsToPaint, bool& success) {

	success = false;

	if (!IsValid(staticMesh) || !IsValid(UVertexPaintFunctionLibrary::GetOptimizationDataAsset())) return;


	FPaintOnLODSettingsStruct paintOnLODSettingsStruct_Local;

	paintOnLODSettingsStruct_Local.maxAmountOfLODsToPaint = maxAmountOfLODsToPaint;

	if (UVertexPaintFunctionLibrary::GetOptimizationDataAsset()->staticMeshNumOfLODsToPaint.Contains(staticMesh))
		AddEditorNotification(FString::Printf(TEXT("Updated Static Mesh %s LODs to Store and Paint!"), *staticMesh->GetName()));
	else
		AddEditorNotification(FString::Printf(TEXT("Added Static Mesh %s LODs to Store and Paint!"), *staticMesh->GetName()));


	UVertexPaintFunctionLibrary::GetOptimizationDataAsset()->staticMeshNumOfLODsToPaint.Add(staticMesh, paintOnLODSettingsStruct_Local);

	success = true;

	SavePackageWrapper(UVertexPaintFunctionLibrary::GetOptimizationDataAsset());

}


//--------------------------------------------------------

// Remove Static Mesh And Amount Of LODs To Paint

void UVertexPaintDetectionEdSubsystem::RemoveStaticMeshAndAmountOfLODsToPaintByDefault(UStaticMesh* staticMesh, bool& success) {

	success = false;

	if (!staticMesh) return;
	if (!IsValid(UVertexPaintFunctionLibrary::GetOptimizationDataAsset())) return;


	if (UVertexPaintFunctionLibrary::GetOptimizationDataAsset()->staticMeshNumOfLODsToPaint.Contains(staticMesh)) {

		UVertexPaintFunctionLibrary::GetOptimizationDataAsset()->staticMeshNumOfLODsToPaint.Remove(staticMesh);

		success = true;

		SavePackageWrapper(UVertexPaintFunctionLibrary::GetOptimizationDataAsset());
	}
}


//--------------------------------------------------------

// Add Skeletal Mesh Color Snippet

void UVertexPaintDetectionEdSubsystem::AddMeshColorSnippet(UMeshComponent* meshComponent, FString colorSnippetID, UVertexPaintColorSnippetDataAsset* colorSnippetDataAsset, bool& success) {

	success = false;

	if (!colorSnippetDataAsset || !meshComponent) return;
	if (colorSnippetID.Len() <= 0) return;

	if (!UVertexPaintFunctionLibrary::GetColorSnippetReferenceDataAsset()) {

		UE_LOG(LogTemp, Warning, TEXT("Runtime Vertex Color Paint and Detection Plugin - Trying to Add Mesh Color Snippet but there is no Color Snippet Reference Data Asset Set in Project Settings. "));

		return;
	}


	if (auto staticMeshComponent_Local = Cast<UStaticMeshComponent>(meshComponent)) {

		if (!staticMeshComponent_Local->GetStaticMesh()) {

			UE_LOG(LogTemp, Warning, TEXT("Runtime Vertex Color Paint and Detection Plugin - Couldn't store Static Mesh Color Snippet because the selected Static Mesh Component a Static Mesh Set"));

			return;
		}

		if (staticMeshComponent_Local->LODData.Num() > 0) {

			if (staticMeshComponent_Local->LODData[0].PaintedVertices.Num() <= 0) {

				UE_LOG(LogTemp, Warning, TEXT("Runtime Vertex Color Paint and Detection Plugin - Couldn't store Static Mesh Color Snippet because the selected Static Mesh doesn't have any Painted Vertices at LOD0"));


				AddEditorNotification(FString::Printf(TEXT("Failed at Adding Color Snippet - Mesh has no Colors to Add!")));

				return;
			}
		}

		else {

			UE_LOG(LogTemp, Warning, TEXT("Runtime Vertex Color Paint and Detection Plugin - Couldn't store Static Mesh Color Snippet because the selected Static Mesh doesn't have any a LOD0"));

			return;
		}


		UObject* staticMesh_Local = nullptr;

#if ENGINE_MAJOR_VERSION == 4

		staticMesh_Local = staticMeshComponent_Local->GetStaticMesh();

#elif ENGINE_MAJOR_VERSION == 5

		staticMesh_Local = staticMeshComponent_Local->GetStaticMesh().Get();

#endif


		FVertexDetectColorSnippetDataStruct colorSnippetDataTemp;
		colorSnippetDataTemp.objectColorSnippetBelongsTo = staticMeshComponent_Local->GetStaticMesh();

		FVertexDetectMeshDataPerLODStruct colorSnippetPerLODTemp;
		colorSnippetPerLODTemp.lod = 0;
		colorSnippetPerLODTemp.meshVertexColorsPerLODArray = UVertexPaintFunctionLibrary::VertexPaintDetectionPlugin_GetStaticMeshVertexColorsAtLOD(staticMeshComponent_Local, 0);


		colorSnippetDataTemp.colorSnippetDataPerLOD.Add(colorSnippetPerLODTemp);


		colorSnippetDataAsset->snippetColorData.Add(colorSnippetID, colorSnippetDataTemp);
		SavePackageWrapper(colorSnippetDataAsset);


		FVertexDetectColorSnippetReferenceDataStruct colorSnippetReferenceDataTemp;

		if (UVertexPaintFunctionLibrary::GetColorSnippetReferenceDataAsset()->staticMeshesColorSnippets.Contains(staticMesh_Local))
			colorSnippetReferenceDataTemp = UVertexPaintFunctionLibrary::GetColorSnippetReferenceDataAsset()->staticMeshesColorSnippets.FindRef(staticMesh_Local);

		colorSnippetReferenceDataTemp.vertexColorSnippetsAndDataAssetsStoredOn.Add(colorSnippetID, colorSnippetDataAsset);
		UVertexPaintFunctionLibrary::GetColorSnippetReferenceDataAsset()->staticMeshesColorSnippets.Add(staticMesh_Local, colorSnippetReferenceDataTemp);


		SavePackageWrapper(UVertexPaintFunctionLibrary::GetColorSnippetReferenceDataAsset());

		success = true;

		AddEditorNotification(FString::Printf(TEXT("Successfully Added Static Mesh Color Snippet: %s!"), *colorSnippetID));
	}

	else if (auto skeletalMeshComponent_Local = Cast<USkeletalMeshComponent>(meshComponent)) {


		USkeletalMesh* skeletalMeshAsset_Local = nullptr;

#if ENGINE_MAJOR_VERSION == 4

		skeletalMeshAsset_Local = Cast<USkeletalMesh>(skeletalMeshComponent_Local->SkeletalMesh);

#elif ENGINE_MAJOR_VERSION == 5


#if ENGINE_MINOR_VERSION == 0

		skeletalMeshAsset_Local = Cast<USkeletalMesh>(skeletalMeshComponent_Local->SkeletalMesh.Get());

#else

		skeletalMeshAsset_Local = Cast<USkeletalMesh>(skeletalMeshComponent_Local->GetSkeletalMeshAsset());

#endif
#endif


		FVertexDetectColorSnippetDataStruct colorSnippetDataTemp;
		colorSnippetDataTemp.objectColorSnippetBelongsTo = skeletalMeshAsset_Local;


		FVertexDetectMeshDataPerLODStruct colorSnippetPerLODTemp;
		colorSnippetPerLODTemp.lod = 0;
		colorSnippetPerLODTemp.meshVertexColorsPerLODArray = UVertexPaintFunctionLibrary::UVertexPaintFunctionLibrary::VertexPaintDetectionPlugin_GetSkeletalMeshVertexColorsAtLOD(skeletalMeshComponent_Local, 0);


		colorSnippetDataTemp.colorSnippetDataPerLOD.Add(colorSnippetPerLODTemp);

		colorSnippetDataAsset->snippetColorData.Add(colorSnippetID, colorSnippetDataTemp);
		SavePackageWrapper(colorSnippetDataAsset);

		FVertexDetectColorSnippetReferenceDataStruct colorSnippetReferenceData_Local;

		if (UVertexPaintFunctionLibrary::GetColorSnippetReferenceDataAsset()->skeletalMeshesColorSnippets.Contains(skeletalMeshAsset_Local))
			colorSnippetReferenceData_Local = UVertexPaintFunctionLibrary::GetColorSnippetReferenceDataAsset()->skeletalMeshesColorSnippets.FindRef(skeletalMeshAsset_Local);

		colorSnippetReferenceData_Local.vertexColorSnippetsAndDataAssetsStoredOn.Add(colorSnippetID, colorSnippetDataAsset);
		UVertexPaintFunctionLibrary::GetColorSnippetReferenceDataAsset()->skeletalMeshesColorSnippets.Add(skeletalMeshAsset_Local, colorSnippetReferenceData_Local);

		SavePackageWrapper(UVertexPaintFunctionLibrary::GetColorSnippetReferenceDataAsset());

		success = true;

		AddEditorNotification(FString::Printf(TEXT("Successfully Added Skeletal Mesh Color Snippet: %s!"), *colorSnippetID));
	}


	// If wasn't success, i.e. didn't save in the function then we save here as a failsafe
	AddColorSnippetTag(colorSnippetID);

	RefreshAllAvailableCachedColorSnippetTagContainer();
}


//--------------------------------------------------------

// Move Mesh Color Snippet

void UVertexPaintDetectionEdSubsystem::MoveMeshColorSnippet(FString colorSnippetID, UVertexPaintColorSnippetDataAsset* colorSnippetDataAssetToMoveFrom, UVertexPaintColorSnippetDataAsset* colorSnippetDataAssetToMoveTo, bool& success) {

	success = false;

	if (!colorSnippetDataAssetToMoveFrom || !colorSnippetDataAssetToMoveTo || colorSnippetID.Len() <= 0) return;


	if (colorSnippetDataAssetToMoveTo->snippetColorData.Contains(colorSnippetID)) {

		UE_LOG(LogTemp, Warning, TEXT("Runtime Vertex Color Paint and Detection Plugin - Trying to Move Mesh Color Snippet to a Data Asset that already has it. "));

		return;
	}

	if (!UVertexPaintFunctionLibrary::GetColorSnippetReferenceDataAsset()) {

		UE_LOG(LogTemp, Warning, TEXT("Runtime Vertex Color Paint and Detection Plugin - Trying to Move Mesh Color Snippet but there is no Color Snippet Reference Data Asset Set in Project Settings. "));

		return;
	}

	if (!UVertexPaintFunctionLibrary::GetColorSnippetReferenceDataAsset()->ContainsColorSnippet(colorSnippetID)) {

		UE_LOG(LogTemp, Warning, TEXT("Runtime Vertex Color Paint and Detection Plugin - Trying to Move Mesh Color Snippet but the Color Snippet isn't registered in the Reference Data Asset. "));

		return;
	}


	if (colorSnippetDataAssetToMoveFrom->snippetColorData.Contains(colorSnippetID)) {

		UObject* snippetMeshToMove = UVertexPaintFunctionLibrary::GetColorSnippetReferenceDataAsset()->GetObjectFromSnippetID(colorSnippetID).Get();
		FVertexDetectColorSnippetDataStruct colorSnippetStruct_Local = colorSnippetDataAssetToMoveFrom->snippetColorData.FindRef(colorSnippetID);


		colorSnippetDataAssetToMoveFrom->snippetColorData.Remove(colorSnippetID);
		UVertexPaintFunctionLibrary::GetColorSnippetReferenceDataAsset()->RemoveColorSnippet(colorSnippetID);


		colorSnippetDataAssetToMoveTo->snippetColorData.Add(colorSnippetID, colorSnippetStruct_Local);

		FVertexDetectColorSnippetReferenceDataStruct colorSnippetReferenceData_Local;

		if (Cast<USkeletalMesh>(snippetMeshToMove)) {


			if (UVertexPaintFunctionLibrary::GetColorSnippetReferenceDataAsset()->skeletalMeshesColorSnippets.Contains(snippetMeshToMove))
				colorSnippetReferenceData_Local = UVertexPaintFunctionLibrary::GetColorSnippetReferenceDataAsset()->skeletalMeshesColorSnippets.FindRef(snippetMeshToMove);

			colorSnippetReferenceData_Local.vertexColorSnippetsAndDataAssetsStoredOn.Add(colorSnippetID, colorSnippetDataAssetToMoveTo);
			UVertexPaintFunctionLibrary::GetColorSnippetReferenceDataAsset()->skeletalMeshesColorSnippets.Add(snippetMeshToMove, colorSnippetReferenceData_Local);

		}

		else if (Cast<UStaticMesh>(snippetMeshToMove)) {

			if (UVertexPaintFunctionLibrary::GetColorSnippetReferenceDataAsset()->staticMeshesColorSnippets.Contains(snippetMeshToMove))
				colorSnippetReferenceData_Local = UVertexPaintFunctionLibrary::GetColorSnippetReferenceDataAsset()->staticMeshesColorSnippets.FindRef(snippetMeshToMove);

			colorSnippetReferenceData_Local.vertexColorSnippetsAndDataAssetsStoredOn.Add(colorSnippetID, colorSnippetDataAssetToMoveTo);
			UVertexPaintFunctionLibrary::GetColorSnippetReferenceDataAsset()->staticMeshesColorSnippets.Add(snippetMeshToMove, colorSnippetReferenceData_Local);
		}


		SavePackageWrapper(colorSnippetDataAssetToMoveFrom);
		SavePackageWrapper(colorSnippetDataAssetToMoveTo);
		SavePackageWrapper(UVertexPaintFunctionLibrary::GetColorSnippetReferenceDataAsset());

		AddEditorNotification(FString::Printf(TEXT("Successfully Moved Color Snippet: %s!"), *colorSnippetID));

		success = true;
	}

	else {

		UE_LOG(LogTemp, Warning, TEXT("Runtime Vertex Color Paint and Detection Plugin - Trying to Move Mesh Color Snippet but the Color Snippet isn't registered in the Color Snippet Data Asset. "));

		return;
	}
}


//--------------------------------------------------------

// Add Color Snippet Tag

bool UVertexPaintDetectionEdSubsystem::AddColorSnippetTag(FString tag) {

	if (!UVertexPaintFunctionLibrary::GetColorSnippetReferenceDataAsset() || DoesColorSnippetTagExist(tag)) return false;


	if (auto gameplayTagEditorModule = &IGameplayTagsEditorModule::Get()) {

		if (gameplayTagEditorModule) {

			if (auto gameplayTagManager = &UGameplayTagsManager::Get()) {

				gameplayTagEditorModule->AddNewGameplayTagToINI(tag, colorSnippetDevComments, "");

				return true;
			}
		}
	}

	return false;
}


//--------------------------------------------------------

// Does Color Snippet Tag Exist

bool UVertexPaintDetectionEdSubsystem::DoesColorSnippetTagExist(FString tag) {


	if (auto gameplayTagManager = &UGameplayTagsManager::Get()) {

		FName colorSnippetTagName(*tag);
		TSharedPtr<FGameplayTagNode> colorSnippetTagNodePtr = gameplayTagManager->FindTagNode(colorSnippetTagName);

		return colorSnippetTagNodePtr.IsValid();
	}

	return false;
}


//--------------------------------------------------------

// Get All Color Snippet Tags As String

TArray<FString> UVertexPaintDetectionEdSubsystem::GetAllColorSnippetTagsDirectlyFromIni() {


	/* NOTE We DON'T want to use this to get all color snippets since this gets updated when manually added and removed etc., we want to be able to pull them from the .ini directly, so in case the user changes them manually etc. we can clean them up.
	if (!UVertexPaintFunctionLibrary::GetColorSnippetReferenceDataAsset()) return TArray<FString>();


	TArray<FGameplayTag> allAvailableGameplayTags_Local;
	UVertexPaintFunctionLibrary::GetColorSnippetReferenceDataAsset()->allAvailableColorSnippets.GetGameplayTagArray(allAvailableGameplayTags_Local);


	TArray<FString> allAvailableGameplayTagsAsStrings_Local;

	for (auto tag : allAvailableGameplayTags_Local)
		allAvailableGameplayTagsAsStrings_Local.Add(tag.GetTagName().ToString());

	return allAvailableGameplayTagsAsStrings_Local;
	*/


	TArray<FString> tagsAsString;

	// Filters for the tag that has our dev comment
	std::string devCommentToFilterFor = std::string(TCHAR_TO_UTF8(*colorSnippetDevComments));
	std::string pathToProjectRoot = std::string(TCHAR_TO_UTF8(*FPaths::ProjectDir()));

	// Open the DefaultGameplayTags.ini file for reading
	std::ifstream inFile(pathToProjectRoot + "/Config/DefaultGameplayTags.ini");

	// Check if the file was opened successfully
	if (!inFile.is_open()) {
		// Handle error
		return tagsAsString;
	}

	// Read the contents of the file into a string
	std::string fileContents((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());

	// Close the input file
	inFile.close();


	// Find all tags in the file contents that have the specified dev comment
	std::vector<std::string> tags;
	size_t pos = 0;

	while ((pos = fileContents.find("+GameplayTagList=", pos)) != std::string::npos) {

		// Find the end of the line containing this tag
		size_t lineEnd = fileContents.find('\n', pos);

		if (lineEnd == std::string::npos) {
			lineEnd = fileContents.length();
		}

		// Extract this tag's dev comment
		size_t devCommentStart = fileContents.find("DevComment=\"", pos);
		if (devCommentStart == std::string::npos || devCommentStart >= lineEnd) {
			// This tag doesn't have a dev comment, so skip it
			pos = lineEnd + 1;
			continue;
		}

		devCommentStart += strlen("DevComment=\"");
		size_t devCommentEnd = fileContents.find('\"', devCommentStart);
		if (devCommentEnd == std::string::npos || devCommentEnd >= lineEnd) {

			// This tag's dev comment is malformed, so skip it
			pos = lineEnd + 1;
			continue;
		}

		std::string thisDevComment = fileContents.substr(devCommentStart, devCommentEnd - devCommentStart);

		// Check if this tag has the specified dev comment
		if (devCommentToFilterFor.empty() || thisDevComment == devCommentToFilterFor) {

			// Extract this tag's name
			size_t tagStart = fileContents.find("Tag=\"", pos);
			if (tagStart == std::string::npos || tagStart >= lineEnd) {

				// This tag is malformed, so skip it
				pos = lineEnd + 1;
				continue;
			}

			tagStart += strlen("Tag=\"");
			size_t tagEnd = fileContents.find('\"', tagStart);

			if (tagEnd == std::string::npos || tagEnd >= lineEnd) {

				// This tag is malformed, so skip it
				pos = lineEnd + 1;
				continue;
			}

			std::string thisTag = fileContents.substr(tagStart, tagEnd - tagStart);

			// Add this tag to the list of tags with the specified dev comment
			tags.push_back(thisTag);
		}

		// Move to the next line in the file contents
		pos = lineEnd + 1;
	}

	// Convert the vector of tags to an array of strings
	std::vector<const char*> cStrings(tags.size());

	for (size_t i = 0; i < tags.size(); ++i) {

		cStrings[i] = tags[i].c_str();
	}

	// Converts to FString and adds to array
	for (const char* cString : cStrings) {

		tagsAsString.Add(FString(cString));
	}

	return tagsAsString;

}


//--------------------------------------------------------

// Remove Mesh Color Snippet

void UVertexPaintDetectionEdSubsystem::RemoveMeshColorSnippet(FString colorSnippetID, UVertexPaintColorSnippetDataAsset* colorSnippetDataAsset) {

	if (!colorSnippetDataAsset) return;
	if (colorSnippetID.Len() <= 0) return;


	if (!UVertexPaintFunctionLibrary::GetColorSnippetReferenceDataAsset()) {

		UE_LOG(LogTemp, Warning, TEXT("Runtime Vertex Color Paint and Detection Plugin - Trying to Remove Mesh Color Snippet but there is no Color Snippet Reference Mesh Data Asset Set in Project Settings. "));

		return;
	}


	// Verify references
	FAssetIdentifier TagId = FAssetIdentifier(FGameplayTag::StaticStruct(), (FName)colorSnippetID);
	TArray<FAssetIdentifier> Referencers;

	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	AssetRegistryModule.Get().GetReferencers(TagId, Referencers, UE::AssetRegistry::EDependencyCategory::SearchableName);

	if (Referencers.Num() > 0) {

		AddEditorNotification(FString::Printf(TEXT("Cannot Delete Color Snippet: %s  -  It's Gameplay Tag is stilled Referenced somewhere!"), *colorSnippetID));

		return;
	}


	if (colorSnippetDataAsset->snippetColorData.Contains(colorSnippetID)) {

		colorSnippetDataAsset->snippetColorData.Remove(colorSnippetID);
		SavePackageWrapper(colorSnippetDataAsset);
	}


	UVertexPaintFunctionLibrary::GetColorSnippetReferenceDataAsset()->RemoveColorSnippet(colorSnippetID);
	SavePackageWrapper(UVertexPaintFunctionLibrary::GetColorSnippetReferenceDataAsset());

	RemoveColorSnippetTag(colorSnippetID);

	RefreshAllAvailableCachedColorSnippetTagContainer();
}


//--------------------------------------------------------

// Remove Color Snippet Tag

bool UVertexPaintDetectionEdSubsystem::RemoveColorSnippetTag(FString tag) {

	if (!UVertexPaintFunctionLibrary::GetColorSnippetReferenceDataAsset()) return false;


	if (auto gameplayTagManager = &UGameplayTagsManager::Get()) {

		FName colorSnippetIDTagName(*tag);
		TSharedPtr<FGameplayTagNode> colorSnippetTagNodePtr = gameplayTagManager->FindTagNode(colorSnippetIDTagName);

		if (colorSnippetTagNodePtr.IsValid()) {

			if (auto gameplayTagEditorModule = &IGameplayTagsEditorModule::Get()) {

				gameplayTagEditorModule->DeleteTagFromINI(colorSnippetTagNodePtr);

				return true;
			}
		}
	}

	return false;
}


//--------------------------------------------------------

// Rename Color Snippet Tag

void UVertexPaintDetectionEdSubsystem::RenameColorSnippetTag(FString oldTag, FString newTag) {


	RemoveColorSnippetTag(oldTag);
	AddColorSnippetTag(newTag);

	/* Couldn't use the RenameTagInINI since it ask you to Restart the Editor
	if (auto gameplayTagManager = &UGameplayTagsManager::Get()) {

		FName prevColorSnippetTagName(*oldTag);
		TSharedPtr<FGameplayTagNode> colorSnippetTagNodePtr = gameplayTagManager->FindTagNode(prevColorSnippetTagName);

		if (colorSnippetTagNodePtr.IsValid()) {

			if (auto gameplayTagEditorModule = &IGameplayTagsEditorModule::Get())
				gameplayTagEditorModule->RenameTagInINI(oldTag, newTag);
		}
	}
	*/
}


//--------------------------------------------------------

// Clear Cached Color Snippet Tag Container

void UVertexPaintDetectionEdSubsystem::ClearAllAvailableColorSnippetCacheTagContainer() {

	if (!UVertexPaintFunctionLibrary::GetColorSnippetReferenceDataAsset()) return;


	if (auto gameplayTagManager = &UGameplayTagsManager::Get()) {

		// First clears them
		TArray<FGameplayTag> currentGameplayTags;
		UVertexPaintFunctionLibrary::GetColorSnippetReferenceDataAsset()->allAvailableColorSnippets.GetGameplayTagArray(currentGameplayTags);

		if (currentGameplayTags.Num() > 0) {

			FGameplayTagContainer currentGameplayTagContainer = UVertexPaintFunctionLibrary::GetColorSnippetReferenceDataAsset()->allAvailableColorSnippets;
			UVertexPaintFunctionLibrary::GetColorSnippetReferenceDataAsset()->allAvailableColorSnippets.RemoveTags(currentGameplayTagContainer);
		}
	}

	SavePackageWrapper(UVertexPaintFunctionLibrary::GetColorSnippetReferenceDataAsset());
}


//--------------------------------------------------------

// Refresh Cached Color Snippet Tag Container

void UVertexPaintDetectionEdSubsystem::RefreshAllAvailableCachedColorSnippetTagContainer() {

	if (!UVertexPaintFunctionLibrary::GetColorSnippetReferenceDataAsset()) return;


	if (auto gameplayTagManager = &UGameplayTagsManager::Get()) {


		TArray<FVertexDetectColorSnippetReferenceDataStruct> totalColorSnippetReferences;
		TArray<FVertexDetectColorSnippetReferenceDataStruct> staticMeshColorSnippetReference;
		TArray<FVertexDetectColorSnippetReferenceDataStruct> skeletalMeshColorSnippetReference;
		UVertexPaintFunctionLibrary::GetColorSnippetReferenceDataAsset()->staticMeshesColorSnippets.GenerateValueArray(staticMeshColorSnippetReference);
		UVertexPaintFunctionLibrary::GetColorSnippetReferenceDataAsset()->skeletalMeshesColorSnippets.GenerateValueArray(skeletalMeshColorSnippetReference);


		totalColorSnippetReferences.Append(staticMeshColorSnippetReference);
		totalColorSnippetReferences.Append(skeletalMeshColorSnippetReference);

		int amountOfSnippetsTemp = 0;

		for (auto snippetRef : totalColorSnippetReferences)
			amountOfSnippetsTemp += snippetRef.vertexColorSnippetsAndDataAssetsStoredOn.Num();


		bool changeOnDataAssetTemp = false;

		// If different amount then gonna clear later and refresh
		if (amountOfSnippetsTemp != UVertexPaintFunctionLibrary::GetColorSnippetReferenceDataAsset()->allAvailableColorSnippets.Num())
			changeOnDataAssetTemp = true;


		TArray<FGameplayTag> tagsToAdd;

		// Then fills it up with whatever is registered to the Static and Skeletal Mesh snippets
		for (auto snippetRef : totalColorSnippetReferences) {

			TArray<FString> colorSnippetStrings;
			snippetRef.vertexColorSnippetsAndDataAssetsStoredOn.GetKeys(colorSnippetStrings);

			for (auto snippetString : colorSnippetStrings) {

				FName colorSnippetIDTagName(*snippetString);
				FGameplayTag requestedTag = gameplayTagManager->RequestGameplayTag(colorSnippetIDTagName);

				if (requestedTag.IsValid()) {

					tagsToAdd.Add(requestedTag);

					// If doesn't have a tag, then we set so we clear the data asset later and fill it will all of them. So we only save it if we actually do a change
					if (!UVertexPaintFunctionLibrary::GetColorSnippetReferenceDataAsset()->allAvailableColorSnippets.HasTag(requestedTag)) {

						changeOnDataAssetTemp = true;
					}
				}
			}
		}

		// If discovered that all available tags aren't the same amount, or there was tags missing in the all available color snippets, then we clear and then add the correct ones. Has this check so we don't save the color snippet reference data asset unless we actually need to. 
		if (changeOnDataAssetTemp) {


			UE_LOG(LogTemp, Warning, TEXT("Runtime Vertex Color Paint and Detection Plugin - Missmatch with amount of Tags Registered on Color Snippet Reference Data Asset, Re-stores them! "));

			ClearAllAvailableColorSnippetCacheTagContainer();

			for (auto tag : tagsToAdd)
				UVertexPaintFunctionLibrary::GetColorSnippetReferenceDataAsset()->allAvailableColorSnippets.AddTag(tag);

			SavePackageWrapper(UVertexPaintFunctionLibrary::GetColorSnippetReferenceDataAsset());
		}
	}
}


//--------------------------------------------------------

// Update Mesh Color Snippet ID

void UVertexPaintDetectionEdSubsystem::UpdateMeshColorSnippetID(FString prevColorSnippetID, FString newColorSnippetID, UVertexPaintColorSnippetDataAsset* colorSnippetDataAsset, bool& success) {

	success = false;

	if (!colorSnippetDataAsset) return;
	if (prevColorSnippetID.Len() <= 0 || newColorSnippetID.Len() <= 0) return;

	if (!UVertexPaintFunctionLibrary::GetColorSnippetReferenceDataAsset()) {

		UE_LOG(LogTemp, Warning, TEXT("Runtime Vertex Color Paint and Detection Plugin - Trying to Update Mesh Color Snippet but there is no Color Snippet Reference Data Asset Set in Project Settings. "));
	}


	if (!UVertexPaintFunctionLibrary::GetColorSnippetReferenceDataAsset()->ContainsColorSnippet(prevColorSnippetID)) {

		UE_LOG(LogTemp, Warning, TEXT("Runtime Vertex Color Paint and Detection Plugin - Trying to Update Color Snippet ID but the Color Snippet isn't registered in the Reference Data Asset. "));

		return;
	}


	// Updates the Color Snippet Data Asset First
	if (colorSnippetDataAsset->snippetColorData.Contains(prevColorSnippetID)) {

		auto colorSnippetStruct_Local = colorSnippetDataAsset->snippetColorData.FindRef(prevColorSnippetID);
		colorSnippetDataAsset->snippetColorData.Remove(prevColorSnippetID);
		colorSnippetDataAsset->snippetColorData.Add(newColorSnippetID, colorSnippetStruct_Local);

		SavePackageWrapper(colorSnippetDataAsset);
	}


	// Then the Color Snippet Reference Data Asset
	auto mesh = UVertexPaintFunctionLibrary::GetColorSnippetReferenceDataAsset()->GetObjectFromSnippetID(prevColorSnippetID);


	FVertexDetectColorSnippetReferenceDataStruct colorSnippetReference_Local;

	if (UVertexPaintFunctionLibrary::GetColorSnippetReferenceDataAsset()->skeletalMeshesColorSnippets.Contains(mesh.Get())) {

		colorSnippetReference_Local = UVertexPaintFunctionLibrary::GetColorSnippetReferenceDataAsset()->skeletalMeshesColorSnippets.FindRef(mesh.Get());
	}

	else if (UVertexPaintFunctionLibrary::GetColorSnippetReferenceDataAsset()->staticMeshesColorSnippets.Contains(mesh.Get())) {

		colorSnippetReference_Local = UVertexPaintFunctionLibrary::GetColorSnippetReferenceDataAsset()->staticMeshesColorSnippets.FindRef(mesh.Get());
	}


	if (colorSnippetReference_Local.vertexColorSnippetsAndDataAssetsStoredOn.Num() > 0) {

		if (colorSnippetReference_Local.vertexColorSnippetsAndDataAssetsStoredOn.Contains(prevColorSnippetID))
			colorSnippetReference_Local.vertexColorSnippetsAndDataAssetsStoredOn.Remove(prevColorSnippetID);

		colorSnippetReference_Local.vertexColorSnippetsAndDataAssetsStoredOn.Add(newColorSnippetID, colorSnippetDataAsset);


		if (Cast<USkeletalMesh>(mesh.Get())) {

			UVertexPaintFunctionLibrary::GetColorSnippetReferenceDataAsset()->skeletalMeshesColorSnippets.Add(mesh.Get(), colorSnippetReference_Local);
		}

		else if (Cast<UStaticMesh>(mesh.Get())) {

			UVertexPaintFunctionLibrary::GetColorSnippetReferenceDataAsset()->staticMeshesColorSnippets.Add(mesh.Get(), colorSnippetReference_Local);
		}
	}

	success = true;

	AddEditorNotification(FString::Printf(TEXT("Successfully Updated Color Snippet to: %s!"), *newColorSnippetID));

	RenameColorSnippetTag(prevColorSnippetID, newColorSnippetID);

	SavePackageWrapper(UVertexPaintFunctionLibrary::GetColorSnippetReferenceDataAsset());

	RefreshAllAvailableCachedColorSnippetTagContainer();
}


//--------------------------------------------------------

// Refresh Stored Color Snippet

void UVertexPaintDetectionEdSubsystem::RefreshStoredColorSnippetFromSerializedStringToTArray(UVertexPaintColorSnippetDataAsset* colorSnippetDataAsset, FString colorSnippet) {

	if (!colorSnippetDataAsset || colorSnippet.Len() <= 0) return;


	if (!UVertexPaintFunctionLibrary::GetColorSnippetReferenceDataAsset()) {

		UE_LOG(LogTemp, Warning, TEXT("Runtime Vertex Color Paint and Detection Plugin - Trying to ReStoreColorSnippetAsJSonString but there is no Color Snippet Reference Data Asset Set in Project Settings. "));

		return;
	}


	for (auto& snippetColorData_Local : colorSnippetDataAsset->snippetColorData) {


		if (colorSnippet == snippetColorData_Local.Key) {

			// If there's an old color snippet as serialized string, then we refresh it
			if (snippetColorData_Local.Value.colorSnippetPerLODAsRapidJSon.IsValidIndex(0)) {


				FVertexDetectColorSnippetDataStruct colorSnippetDataTemp;
				colorSnippetDataTemp.objectColorSnippetBelongsTo = snippetColorData_Local.Value.objectColorSnippetBelongsTo;

				FVertexDetectMeshDataPerLODStruct colorSnippetPerLODTemp;
				colorSnippetPerLODTemp.lod = 0;


				if (snippetColorData_Local.Value.colorSnippetPerLODAsRapidJSon.Num() > 0) {

					colorSnippetPerLODTemp.meshVertexColorsPerLODArray = URapidJsonFunctionLibrary::DeserializeTArrayFColor_Wrapper(snippetColorData_Local.Value.colorSnippetPerLODAsRapidJSon[0].colorsAtLODAsJSonString);

					snippetColorData_Local.Value.colorSnippetPerLODAsRapidJSon.Empty();

					colorSnippetDataTemp.colorSnippetDataPerLOD.Add(colorSnippetPerLODTemp);
					colorSnippetDataAsset->snippetColorData.Add(colorSnippet, colorSnippetDataTemp);

					UE_LOG(LogTemp, Log, TEXT("Runtime Vertex Color Paint and Detection Plugin - Refreshed Color Snippet as JSong String!"));

					SavePackageWrapper(colorSnippetDataAsset);

					return;
				}
			}
		}
	}
}


//--------------------------------------------------------

// Apply Vertex Color Data To Mesh

bool UVertexPaintDetectionEdSubsystem::ApplyVertexColorToMeshAtLOD0(UMeshComponent* mesh, TArray<FColor> vertexColorsAtLOD0) {

	if (!IsValid(mesh) || vertexColorsAtLOD0.Num() <= 0) return false;


	if (auto staticMeshComp = Cast<UStaticMeshComponent>(mesh)) {

		bool appliedColorsToAnyLOD_Local = false;
		staticMeshComp->SetLODDataCount(1, staticMeshComp->GetStaticMesh()->GetNumLODs());
		staticMeshComp->Modify();

		if (staticMeshComp->LODData.IsValidIndex(0)) {

			if (staticMeshComp->LODData[0].OverrideVertexColors) {

				staticMeshComp->LODData[0].ReleaseOverrideVertexColorsAndBlock(); // UMeshPaintingToolset::SetInstanceColorDataForLOD ran this and it seem to fix an issue if updating a color snippet, then previewing it again where you couldn't reset it afterwards. 

			}

			staticMeshComp->LODData[0].OverrideVertexColors = new FColorVertexBuffer;
			staticMeshComp->LODData[0].OverrideVertexColors->InitFromColorArray(vertexColorsAtLOD0);

			BeginInitResource(staticMeshComp->LODData[0].OverrideVertexColors);

			staticMeshComp->MarkRenderStateDirty();
			return true;
		}
	}

	else if (auto skeletalMeshComp = Cast<USkeletalMeshComponent>(mesh)) {


		USkeletalMesh* skelMesh = nullptr;

#if ENGINE_MAJOR_VERSION == 4

		skelMesh = skeletalMeshComp->SkeletalMesh;

#elif ENGINE_MAJOR_VERSION == 5

#if ENGINE_MINOR_VERSION == 0

		skelMesh = skeletalMeshComp->SkeletalMesh.Get();

#else

		skelMesh = skeletalMeshComp->GetSkeletalMeshAsset();

#endif
#endif


		if (skeletalMeshComp->LODInfo.IsValidIndex(0)) {

			// Dirty the mesh
			skelMesh->SetFlags(RF_Transactional);
			skelMesh->Modify();
			skelMesh->SetHasVertexColors(true);
			skelMesh->SetVertexColorGuid(FGuid::NewGuid());

			// Release the static mesh's resources.
			skelMesh->ReleaseResources();

			// Flush the resource release commands to the rendering thread to ensure that the build doesn't occur while a resource is still
			// allocated, and potentially accessing the USkeletalMesh.
			skelMesh->ReleaseResourcesFence.Wait();

			// const int32 NumLODs = Mesh->GetLODNum();
			const int32 NumLODs = 1;
			if (NumLODs > 0)
			{
				TUniquePtr<FSkinnedMeshComponentRecreateRenderStateContext> RecreateRenderStateContext = MakeUnique<FSkinnedMeshComponentRecreateRenderStateContext>(skelMesh);

				for (int32 LODIndex = 0; LODIndex < NumLODs; ++LODIndex)
				{
					FSkeletalMeshLODRenderData& LODData = skelMesh->GetResourceForRendering()->LODRenderData[LODIndex];
					FPositionVertexBuffer& PositionVertexBuffer = LODData.StaticVertexBuffers.PositionVertexBuffer;


					// Check if the provided vertex colors match the mesh's vertex count
					int32 VertexCount = PositionVertexBuffer.GetNumVertices();
					if (vertexColorsAtLOD0.Num() != VertexCount)
					{
						UE_LOG(LogTemp, Error, TEXT("Runtime Vertex Color Paint and Detection Plugin - The number of provided vertex colors does not match the mesh's vertex count."));
						continue;
					}

					// Make sure the existing color buffer is initialized
					if (!LODData.StaticVertexBuffers.ColorVertexBuffer.IsInitialized())
					{
						LODData.StaticVertexBuffers.ColorVertexBuffer.Init(VertexCount);
					}

					for (int32 j = 0; j < VertexCount; j++)
					{
						if (vertexColorsAtLOD0.IsValidIndex(j))
						{
							LODData.StaticVertexBuffers.ColorVertexBuffer.VertexColor(j) = vertexColorsAtLOD0[j];
						}
					}

					ENQUEUE_RENDER_COMMAND(RefreshColorBuffer)(
						[&LODData](FRHICommandListImmediate& RHICmdList)
						{
							BeginInitResource(&LODData.StaticVertexBuffers.ColorVertexBuffer);
						});
				}

				skelMesh->InitResources();
			}

			return true;
		}
	}

	return false;
}


//--------------------------------------------------------

// Set Custom Settings Optimizations Data Asset To Use

void UVertexPaintDetectionEdSubsystem::SetCustomSettingsOptimizationsDataAssetToUse(UObject* dataAsset) {

	// if (!IsValid(dataAsset)) return; // No valid check here as we should be able to set it to nothing if we want to

	UVertexPaintDetectionSettings* developerSettings_Local = GetMutableDefault<UVertexPaintDetectionSettings>();
	UVertexPaintOptimizationDataAsset* optimizationsDataAsset_Local = Cast<UVertexPaintOptimizationDataAsset>(dataAsset);

	if (developerSettings_Local) {

		if (optimizationsDataAsset_Local)
			developerSettings_Local->vertexPaintAndDetectionPlugin_OptimizationDataAssetToUse = optimizationsDataAsset_Local;
		else
			developerSettings_Local->vertexPaintAndDetectionPlugin_OptimizationDataAssetToUse = nullptr;

		developerSettings_Local->SaveConfig();


#if ENGINE_MAJOR_VERSION == 4

		developerSettings_Local->UpdateDefaultConfigFile();

#elif ENGINE_MAJOR_VERSION == 5

		developerSettings_Local->TryUpdateDefaultConfigFile();
#endif
	}
}


//----------------------------------------------------------------------------------------------------------------

// Set Custom Settings Vertex Paint Material To Use

void UVertexPaintDetectionEdSubsystem::SetCustomSettingsVertexPaintMaterialToUse(UObject* dataAsset) {

	// if (!IsValid(dataAsset)) return;  // No valid check here as we should be able to set it to nothing if we want to

	UVertexPaintDetectionSettings* developerSettings_Local = GetMutableDefault<UVertexPaintDetectionSettings>();
	UVertexPaintMaterialDataAsset* paintOnMaterialDataAsset_Local = Cast<UVertexPaintMaterialDataAsset>(dataAsset);


	if (developerSettings_Local) {

		if (paintOnMaterialDataAsset_Local)
			developerSettings_Local->vertexPaintAndDetectionPlugin_MaterialsDataAssetToUse = paintOnMaterialDataAsset_Local;
		else
			developerSettings_Local->vertexPaintAndDetectionPlugin_MaterialsDataAssetToUse = nullptr;

		developerSettings_Local->SaveConfig();


#if ENGINE_MAJOR_VERSION == 4

		developerSettings_Local->UpdateDefaultConfigFile();

#elif ENGINE_MAJOR_VERSION == 5

		developerSettings_Local->TryUpdateDefaultConfigFile();
#endif
	}
}


//----------------------------------------------------------------------------------------------------------------

// Set Custom Settings Vertex Paint Color Snippet Reference Data Asset To Use

void UVertexPaintDetectionEdSubsystem::SetCustomSettingsVertexPaintColorSnippetReferenceDataAssetToUse(UObject* dataAsset) {

	// if (!IsValid(dataAsset)) return; // No valid check here as we should be able to set it to nothing if we want to

	UVertexPaintDetectionSettings* developerSettings_Local = GetMutableDefault<UVertexPaintDetectionSettings>();
	UVertexPaintColorSnippetRefs* skeletalMeshDataAsset_Local = Cast<UVertexPaintColorSnippetRefs>(dataAsset);


	if (developerSettings_Local) {

		if (skeletalMeshDataAsset_Local)
			developerSettings_Local->vertexPaintAndDetectionPlugin_ColorSnippetReferencesDataAssetToUse = skeletalMeshDataAsset_Local;
		else
			developerSettings_Local->vertexPaintAndDetectionPlugin_ColorSnippetReferencesDataAssetToUse = nullptr;

		developerSettings_Local->SaveConfig();


#if ENGINE_MAJOR_VERSION == 4

		developerSettings_Local->UpdateDefaultConfigFile();

#elif ENGINE_MAJOR_VERSION == 5

		developerSettings_Local->TryUpdateDefaultConfigFile();
#endif
	}
}


//--------------------------------------------------------

// Load Essential Data Assets - Used by for instance Editor Widget to make sure they're loaded when it's open, to avoid race condition if you close the editor with the widget open and the next time you open the editor

void UVertexPaintDetectionEdSubsystem::LoadEssentialDataAssets() {

	UVertexPaintFunctionLibrary::GetColorSnippetReferenceDataAsset();
	UVertexPaintFunctionLibrary::GetOptimizationDataAsset();
	UVertexPaintFunctionLibrary::GetVertexPaintMaterialDataAsset();
}


//-------------------------------------------------------

// Wrappers

int UVertexPaintDetectionEdSubsystem::GetStaticMeshVerticesAmount_Wrapper(UStaticMesh* mesh, int lod) {

	if (!IsValid(mesh)) return 0;

	if (!mesh->GetRenderData()->LODResources.IsValidIndex(lod)) return 0;


	return mesh->GetRenderData()->LODResources[lod].GetNumVertices();
}

int UVertexPaintDetectionEdSubsystem::GetSkeletalMeshVerticesAmount_Wrapper(USkeletalMesh* mesh, int lod) {

	if (!IsValid(mesh)) return 0;

	if (!mesh->GetImportedModel()->LODModels.IsValidIndex(lod)) return 0;


	return mesh->GetImportedModel()->LODModels[lod].NumVertices;
}

int UVertexPaintDetectionEdSubsystem::GetStaticMeshLODCount_Wrapper(UStaticMesh* mesh) {

	if (!mesh) return -1;

	return mesh->GetNumSourceModels();
}

int UVertexPaintDetectionEdSubsystem::GetSkeletalMeshLODCount_Wrapper(USkeletalMesh* skeletalMesh) {

	if (!skeletalMesh) return -1;

	return skeletalMesh->GetLODNum();
}

int UVertexPaintDetectionEdSubsystem::GetSkeletalMeshComponentVertexCount_Wrapper(USkeletalMeshComponent* skeletalMeshComponent, int lod) {

	if (!skeletalMeshComponent) return 0;

	if (!skeletalMeshComponent->GetSkeletalMeshRenderData()->LODRenderData.IsValidIndex(lod)) return 0;

	return skeletalMeshComponent->GetSkeletalMeshRenderData()->LODRenderData[lod].GetNumVertices();
}

int UVertexPaintDetectionEdSubsystem::GetStaticMeshComponentVertexCount_Wrapper(UStaticMeshComponent* staticMeshComponent, int lod) {

	if (!staticMeshComponent) return 0;


	if (staticMeshComponent->GetStaticMesh()->GetRenderData()->LODResources.IsValidIndex(lod))
		return staticMeshComponent->GetStaticMesh()->GetRenderData()->LODResources[lod].GetNumVertices();

	return 0;
}

UWorld* UVertexPaintDetectionEdSubsystem::GetPersistentLevelsWorld_Wrapper(const UObject* worldContext) {

	if (!IsValid(worldContext)) return nullptr;

	return worldContext->GetWorld();
}


//-------------------------------------------------------

// Deinitialize

void UVertexPaintDetectionEdSubsystem::Deinitialize() {

	UE_LOG(LogTemp, Log, TEXT("Runtime Vertex Color Paint and Detection Plugin Editor Subsystem De-Initialized"));
}
