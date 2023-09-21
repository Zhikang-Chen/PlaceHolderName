/**
* Copyright (C) 2017-2020 | Dry Eel Development
*
* Official EventDispatcher Documentation: https://eeldev.com/index.php/eventdispatcher-plugin/
*/

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(LogEventDispatcher, Log, All);

class FEventDispatcherModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
