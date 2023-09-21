/**
* Copyright (C) 2017-2020 | Dry Eel Development
*
* Official EventDispatcher Documentation: https://eeldev.com/index.php/eventdispatcher-plugin/
*/

#include "EventDispatcherModule.h"
#include "EventDispatcherPrivatePCH.h"

#define LOCTEXT_NAMESPACE "FEventDispatcherModule"
DEFINE_LOG_CATEGORY(LogEventDispatcher);

void FEventDispatcherModule::StartupModule()
{
}

void FEventDispatcherModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FEventDispatcherModule, EventDispatcher)
