// Copyright 2022 Alexander Floden, Alias Alex River. All Rights Reserved. 


#if ENGINE_MAJOR_VERSION == 4


#include "VertexPaint_UEToolbarButton.h"
#include "VertexPaint_UEToolbarButtonStyle.h"
#include "VertexPaint_UEToolbarButtonCommands.h"
#include "Misc/MessageDialog.h"
#include "WidgetBlueprint.h"
#include "EditorUtilityWidgetBlueprint.h"
#include "EditorUtilitySubsystem.h"
#include "ToolMenus.h"
#include "AssetRegistry/AssetRegistryModule.h"


static const FName VertexPaint_UEToolbarButtonTabName("VertexPaint_UEToolbarButton");

#define LOCTEXT_NAMESPACE "FVertexPaint_UEToolbarButtonModule"

void FVertexPaint_UEToolbarButtonModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	UE_LOG(LogTemp, Log, TEXT("Runtime Vertex Color Paint and Detection Plugin - UE4 Editor Toolbar Button Module Startup!"));

	FVertexPaint_UEToolbarButtonStyle::Initialize();
	FVertexPaint_UEToolbarButtonStyle::ReloadTextures();

	FVertexPaint_UEToolbarButtonCommands::Register();

	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FVertexPaint_UEToolbarButtonCommands::Get().PluginAction,
		FExecuteAction::CreateRaw(this, &FVertexPaint_UEToolbarButtonModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FVertexPaint_UEToolbarButtonModule::RegisterMenus));
}

void FVertexPaint_UEToolbarButtonModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UE_LOG(LogTemp, Log, TEXT("Runtime Vertex Color Paint and Detection Plugin - UE4 Editor Toolbar Button Module Shutdown! "));

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FVertexPaint_UEToolbarButtonStyle::Shutdown();

	FVertexPaint_UEToolbarButtonCommands::Unregister();
}

void FVertexPaint_UEToolbarButtonModule::PluginButtonClicked()
{
	// Put your "OnButtonClicked" stuff here

	/*
	FText DialogText = FText::Format(
							LOCTEXT("PluginButtonDialogText", "Add code to {0} in {1} to override this button's actions"),
							FText::FromString(TEXT("FVertexPaint_UEToolbarButtonModule::PluginButtonClicked()")),
							FText::FromString(TEXT("VertexPaint_UEToolbarButton.cpp"))
					   );
	FMessageDialog::Open(EAppMsgType::Ok, DialogText);
	*/

	UE_LOG(LogTemp, Log, TEXT("Runtime Vertex Color Paint and Detection Plugin - Pressed Editor Utility Button"));


	auto editorSubsystem = GEditor->GetEditorSubsystem<UEditorUtilitySubsystem>();

	// Creates the Editor Utility Widget Blueprint when pressing the Button
	if (editorSubsystem->DoesTabExist(editorWidgetID)) {

		editorSubsystem->CloseTabByID(editorWidgetID);
	}

	else {

		FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
		TArray<FAssetData> AssetData;

		auto editorWidgetAsset = AssetRegistryModule.Get().GetAssetByObjectPath("/VertexPaintDetectionPlugin/EditorUtility/EWBP_VertexPaint_PluginSettings.EWBP_VertexPaint_PluginSettings", true).GetAsset();


		if (editorWidgetAsset) {

			auto widgetBlueprint = Cast<UWidgetBlueprint>(editorWidgetAsset);

			if (widgetBlueprint) {

				if (auto editorWidgetBlueprint = Cast<UEditorUtilityWidgetBlueprint>(editorWidgetAsset))
					editorSubsystem->SpawnAndRegisterTabAndGetID(editorWidgetBlueprint, editorWidgetID);
			}
		}
	}

}

void FVertexPaint_UEToolbarButtonModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FVertexPaint_UEToolbarButtonCommands::Get().PluginAction, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("Modes");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FVertexPaint_UEToolbarButtonCommands::Get().PluginAction));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FVertexPaint_UEToolbarButtonModule, VertexPaint_UEToolbarButton)



#elif ENGINE_MAJOR_VERSION == 5


#include "VertexPaint_UEToolbarButton.h"
#include "VertexPaint_UEToolbarButtonStyle.h"
#include "VertexPaint_UEToolbarButtonCommands.h"
#include "Misc/MessageDialog.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "WidgetBlueprint.h"
#include "EditorUtilityWidget.h"
#include "EditorUtilityWidgetBlueprint.h"
#include "EditorUtilitySubsystem.h"

#include "ToolMenus.h"

static const FName VertexPaint_UEToolbarButtonTabName("VertexPaint_UEToolbarButton");

#define LOCTEXT_NAMESPACE "FVertexPaint_UEToolbarButtonModule"

void FVertexPaint_UEToolbarButtonModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	UE_LOG(LogTemp, Log, TEXT("Runtime Vertex Color Paint and Detection Plugin - UE5 Editor Toolbar Button Module Startup!"));

	FVertexPaint_UEToolbarButtonStyle::Initialize();
	FVertexPaint_UEToolbarButtonStyle::ReloadTextures();

	FVertexPaint_UEToolbarButtonCommands::Register();

	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FVertexPaint_UEToolbarButtonCommands::Get().PluginAction,
		FExecuteAction::CreateRaw(this, &FVertexPaint_UEToolbarButtonModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FVertexPaint_UEToolbarButtonModule::RegisterMenus));
}

void FVertexPaint_UEToolbarButtonModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UE_LOG(LogTemp, Log, TEXT("Runtime Vertex Color Paint and Detection Plugin - UE5 Editor Toolbar Button Module Shutdown! "));

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FVertexPaint_UEToolbarButtonStyle::Shutdown();

	FVertexPaint_UEToolbarButtonCommands::Unregister();
}

void FVertexPaint_UEToolbarButtonModule::PluginButtonClicked()
{
	UE_LOG(LogTemp, Log, TEXT("Runtime Vertex Color Paint and Detection Plugin - Pressed Editor Utility Button"));

	/*
	// Put your "OnButtonClicked" stuff here
	FText DialogText = FText::Format(
							LOCTEXT("PluginButtonDialogText", "Add code to {0} in {1} to override this button's actions"),
							FText::FromString(TEXT("FVertexPaint_UEToolbarButtonModule::PluginButtonClicked()")),
							FText::FromString(TEXT("VertexPaint_UEToolbarButton.cpp"))
					   );
	FMessageDialog::Open(EAppMsgType::Ok, DialogText);

	UE_LOG(LogTemp, Warning, TEXT("Vertex Paint and Detection Plugin - Pressed Editor Utility Button"));
	*/

	auto editorSubsystem = GEditor->GetEditorSubsystem<UEditorUtilitySubsystem>();

	// Creates the Editor Utility Widget Blueprint when pressing the Button
	if (editorSubsystem->DoesTabExist(editorWidgetID)) {

		editorSubsystem->CloseTabByID(editorWidgetID);
	}

	else {

		FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
		TArray<FAssetData> AssetData;

		// auto editorWidgetAsset = AssetRegistryModule.Get().GetAssetByObjectPath("/VertexPaintDetectionPlugin/EditorUtility/EWBP_VertexPaint_PluginSettings.EWBP_VertexPaint_PluginSettings", true).GetAsset();


#if ENGINE_MAJOR_VERSION == 4

		auto editorWidgetAsset = AssetRegistryModule.Get().GetAssetByObjectPath("/VertexPaintDetectionPlugin/EditorUtility/EWBP_VertexPaint_PluginSettings.EWBP_VertexPaint_PluginSettings", true).GetAsset();

#elif ENGINE_MAJOR_VERSION == 5


#if ENGINE_MINOR_VERSION == 0

		auto editorWidgetAsset = AssetRegistryModule.Get().GetAssetByObjectPath("/VertexPaintDetectionPlugin/EditorUtility/EWBP_VertexPaint_PluginSettings.EWBP_VertexPaint_PluginSettings", true).GetAsset();

#else

		FSoftObjectPath objectPathTemp;
		objectPathTemp.SetPath("/VertexPaintDetectionPlugin/EditorUtility/EWBP_VertexPaint_PluginSettings.EWBP_VertexPaint_PluginSettings");

		auto editorWidgetAsset = AssetRegistryModule.Get().GetAssetByObjectPath(objectPathTemp, true).GetAsset();

#endif
#endif

		if (editorWidgetAsset) {

			auto widgetBlueprint = Cast<UWidgetBlueprint>(editorWidgetAsset);

			if (widgetBlueprint) {

				if (auto editorWidgetBlueprint = Cast<UEditorUtilityWidgetBlueprint>(editorWidgetAsset))
					editorSubsystem->SpawnAndRegisterTabAndGetID(editorWidgetBlueprint, editorWidgetID);
			}
		}
	}
}

void FVertexPaint_UEToolbarButtonModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FVertexPaint_UEToolbarButtonCommands::Get().PluginAction, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("PluginTools");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FVertexPaint_UEToolbarButtonCommands::Get().PluginAction));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FVertexPaint_UEToolbarButtonModule, VertexPaint_UEToolbarButton)


#endif


