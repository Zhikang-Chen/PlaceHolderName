// Copyright 2022 ZADROT LLC

#include "ISettingsModule.h"
#include "ISettingsSection.h"
#include "UConfig.h"

class EISModule : public IModuleInterface
{
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

void EISModule::StartupModule()
{
	ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");
	if (SettingsModule)
	{
		TSharedPtr<ISettingsSection> SettingsSection = 
			SettingsModule->RegisterSettings("Editor", "Plugins", "General", FText::FromString("Editor Interface Size"), FText::FromString("Adjust editor interface size"), GetMutableDefault<UConfig>());

		if (SettingsSection.IsValid()) 
		{ 
			GetMutableDefault<UConfig>()->UpdateScale();
		}
	}
}

void EISModule::ShutdownModule()
{
	ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");
	if (SettingsModule)
	{
		SettingsModule->UnregisterSettings("Editor", "Plugins", "General");
	}
}

IMPLEMENT_MODULE(EISModule, EIS)