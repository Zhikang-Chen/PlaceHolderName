/**
* Copyright (C) 2017-2020 | Dry Eel Development
*
* Official EventDispatcher Documentation: https://eeldev.com/index.php/eventdispatcher-plugin/
*/

#include "EventDispatcherSettings.h"
#include "EventDispatcherPrivatePCH.h"

FString EventDispatcherEngineIni = FPaths::ProjectConfigDir() / "DefaultEngine.ini";

UEventDispatcherSettings::UEventDispatcherSettings()
	: PollingRate(0.01f)
{
}

#if WITH_EDITOR
void UEventDispatcherSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	GConfig->Flush(false, EventDispatcherEngineIni);
	SaveConfig(CPF_Config, *EventDispatcherEngineIni);
}

FName UEventDispatcherSettings::GetCategoryName() const
{
	return NAME_Game;
}
#endif
