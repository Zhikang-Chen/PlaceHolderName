/**
* Copyright (C) 2017-2020 | Dry Eel Development
*
* Official EventDispatcher Documentation: https://eeldev.com/index.php/eventdispatcher-plugin/
*/

#include "EventDispatcherManager.h"
#include "EventDispatcherSettings.h"
#include "EventDispatcherModule.h"
#include "EventDispatcherLogging.h"
#include "EventDispatcherPrivatePCH.h"

static float PollingRate = 0.01f;
static float TickCounter = 0.0f;
static bool bInitialized = false;

void UEventManager::Initialize(FSubsystemCollectionBase& Collection)
{
	m_InQueue.SetNumZeroed(100);
	
	PollingRate = GetDefault<UEventDispatcherSettings>()->PollingRate;
	bInitialized = true;
}

void UEventManager::Deinitialize()
{
	m_InQueue.Empty();
	m_Events.Empty();

	bInitialized = false;
	TickCounter = 0.0f;
}

bool UEventManager::Tick(float deltaTime)
{
	if (bInitialized)
	{
		if (PollingRate == 0.0f)
		{
			HandleQueue();
		}
		else
		{
			TickCounter += deltaTime;

			if (TickCounter >= PollingRate)
			{
				HandleQueue();
				TickCounter = 0.0f;
			}
		}
	}

	return true;
}

void UEventManager::Internal_InvokeEvent(UEventIdentifier* EventIdentifier, FString EventIdentifierName, UObject* OptionalTarget, EEventType Type, void* Data)
{

	for (int32 i = 0; i < GetNumEvents(); i++)
	{
		const TSharedPtr<FBaseEvent> Event = m_Events[i];

		if (OptionalTarget)
		{
			if (Event->m_ContextObject != OptionalTarget)
				continue;
		}

		if (Event->GetType() == Type)
		{
			if ((EventIdentifier && (EventIdentifier == Event->GetIdentifier())) || (EventIdentifierName.IsEmpty() == false && (Event->GetIdentifierName() == EventIdentifierName)))
			{
				switch (Type)
				{
				case EEventType::Payload:
					AddQueueEvent(MakeShareable(new FPayloadEvent(*static_cast<FPayloadEvent*>(m_Events[i].Get()), Data)));
					break;
				case EEventType::Simple:
					AddQueueEvent(MakeShareable(new FSimpleEvent(*static_cast<FSimpleEvent*>(m_Events[i].Get()))));
					break;
				case EEventType::String:
					AddQueueEvent(MakeShareable(new FStringEvent(*static_cast<FStringEvent*>(m_Events[i].Get()), Data)));
					break;
				case EEventType::Int:
					AddQueueEvent(MakeShareable(new FIntEvent(*static_cast<FIntEvent*>(m_Events[i].Get()), Data)));
					break;
				case EEventType::Float:
					AddQueueEvent(MakeShareable(new FFloatEvent(*static_cast<FFloatEvent*>(m_Events[i].Get()), Data)));
					break;
				case EEventType::Bool:
					AddQueueEvent(MakeShareable(new FBoolEvent(*static_cast<FBoolEvent*>(m_Events[i].Get()), Data)));
					break;
				case EEventType::StringFloat:
					AddQueueEvent(MakeShareable(new FStringFloatEvent(*static_cast<FStringFloatEvent*>(m_Events[i].Get()), Data)));
					break;
				case EEventType::StringInt:
					AddQueueEvent(MakeShareable(new FStringIntEvent(*static_cast<FStringIntEvent*>(m_Events[i].Get()), Data)));
					break;
				case EEventType::Vector:
					AddQueueEvent(MakeShareable(new FVectorEvent(*static_cast<FVectorEvent*>(m_Events[i].Get()), Data)));
					break;
				case EEventType::Invalid: break;
				default: ;
				}
			}
		}
	}
}

void UEventManager::Internal_UnregisterEvents()
{
	FScopeLock QueueLock(&m_QueueLock);

	m_InQueue.Empty();

	for (int32 i = 0; i < GetNumEvents(); i++)
	{
		if (m_Events[i] != nullptr)
		{
			m_Events[i].Reset();
			m_Events[i] = nullptr;
		}
	}

	m_Events.Empty();
}

void UEventManager::Internal_UnregisterEvent(UEventIdentifier* EventIdentifier)
{
	FScopeLock QueueLock(&m_QueueLock);
	
	for (int32 i = GetNumEvents() - 1; i >= 0; i--)
	{
		if (m_Events[i]->GetIdentifier() == EventIdentifier)
		{
			m_Events[i].Reset();
			m_Events[i] = nullptr;

			m_Events.RemoveAt(i);

			LogVerbose("Event (%s) Unregistered", *EventIdentifier->GetName());
		}
	}
}

void UEventManager::Internal_UnregisterEventName(FString EventIdentifier)
{
	FScopeLock QueueLock(&m_QueueLock);
	
	for (int32 i = GetNumEvents() - 1; i >= 0; i--)
	{
		if (m_Events[i]->GetIdentifierName() == EventIdentifier)
		{
			m_Events[i].Reset();
			m_Events[i] = nullptr;

			m_Events.RemoveAt(i);

			LogVerbose("Event (%s) Unregistered", *EventIdentifier);
		}
	}
}

void UEventManager::HandleQueue()
{
	FScopeLock QueueLock(&m_QueueLock);

	for (auto& Element : m_InQueue)
	{
		if (Element.IsValid())
		{
			if (Element->IsValid())
			{
				Element->TriggerDelegates();
				Element->Invalidate();
			}
			Element.Reset();
		}
	}
}

void UEventManager::RegisterSimpleEvent(UObject* WorldContextObject, UEventIdentifier* EventIdentifier, const FSimpleEventDelegate& Callback)
{
	ensureAlwaysMsgf(EventIdentifier, TEXT("No EventIdentifier was specified"));

	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		ensureAlwaysMsgf(World->GetGameInstance(), TEXT("No GameInstance found, failed to register the event"));

		if (World->GetGameInstance())
		{
			UEventManager* Manager = World->GetGameInstance()->GetSubsystem<UEventManager>();

			if (EventIdentifier)
			{
				const TSharedPtr<FSimpleEvent> SimpleEvent = MakeShareable(new FSimpleEvent(WorldContextObject, EventIdentifier, "",  Callback));
				Manager->AddEvent(SimpleEvent);
			}
			else
			{
				LogError("Invalid event identifier");
			}
		}
	}
}

void UEventManager::RegisterSimpleEventName(UObject* WorldContextObject, FString EventIdentifier, const FSimpleEventDelegate& Callback)
{
	ensureAlwaysMsgf(EventIdentifier.IsEmpty() == false, TEXT("No EventIdentifier was specified"));

	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		ensureAlwaysMsgf(World->GetGameInstance(), TEXT("No GameInstance found, failed to register the event"));

		if (World->GetGameInstance())
		{
			UEventManager* Manager = World->GetGameInstance()->GetSubsystem<UEventManager>();

			if (!EventIdentifier.IsEmpty())
			{
				const TSharedPtr<FSimpleEvent> SimpleEvent = MakeShareable(new FSimpleEvent(WorldContextObject, nullptr, EventIdentifier,  Callback));
				Manager->AddEvent(SimpleEvent);
			}
			else
			{
				LogError("Invalid event identifier");
			}
		}
	}
}

void UEventManager::RegisterPayloadEvent(UObject* WorldContextObject, UEventIdentifier* EventIdentifier, const FPayloadEventDelegate& Callback)
{
	ensureAlwaysMsgf(EventIdentifier, TEXT("No EventIdentifier was specified"));

	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		ensureAlwaysMsgf(World->GetGameInstance(), TEXT("No GameInstance found, failed to register the event"));

		if (World->GetGameInstance())
		{
			UEventManager* Manager = World->GetGameInstance()->GetSubsystem<UEventManager>();

			if (EventIdentifier)
			{
				const TSharedPtr<FPayloadEvent> PayloadEvent = MakeShareable(new FPayloadEvent(WorldContextObject, EventIdentifier, "", Callback));
				Manager->AddEvent(PayloadEvent);
			}
			else
			{
				LogError("Invalid event identifier");
			}
		}
	}
}

void UEventManager::RegisterPayloadEventName(UObject* WorldContextObject, FString EventIdentifier, const FPayloadEventDelegate& Callback)
{
	ensureAlwaysMsgf(EventIdentifier.IsEmpty() == false, TEXT("No EventIdentifier was specified"));

	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		ensureAlwaysMsgf(World->GetGameInstance(), TEXT("No GameInstance found, failed to register the event"));

		if (World->GetGameInstance())
		{
			UEventManager* Manager = World->GetGameInstance()->GetSubsystem<UEventManager>();

			if (!EventIdentifier.IsEmpty())
			{
				const TSharedPtr<FPayloadEvent> PayloadEvent = MakeShareable(new FPayloadEvent(WorldContextObject, nullptr, EventIdentifier, Callback));
				Manager->AddEvent(PayloadEvent);
			}
			else
			{
				LogError("Invalid event identifier");
			}
		}
	}
}

void UEventManager::RegisterStringEvent(UObject* WorldContextObject, UEventIdentifier* EventIdentifier, const FStringEventDelegate& Callback)
{
	ensureAlwaysMsgf(EventIdentifier, TEXT("No EventIdentifier was specified"));

	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		ensureAlwaysMsgf(World->GetGameInstance(), TEXT("No GameInstance found, failed to register the event"));

		if (World->GetGameInstance())
		{
			UEventManager* Manager = World->GetGameInstance()->GetSubsystem<UEventManager>();

			if (EventIdentifier)
			{
				const TSharedPtr<FStringEvent> StringEvent = MakeShareable(new FStringEvent(WorldContextObject, EventIdentifier, "", Callback));
				Manager->AddEvent(StringEvent);
			}
			else
			{
				LogError("Invalid event identifier");
			}
		}
	}
}

void UEventManager::RegisterStringEventName(UObject* WorldContextObject, FString EventIdentifier, const FStringEventDelegate& Callback)
{
	ensureAlwaysMsgf(EventIdentifier.IsEmpty() == false, TEXT("No EventIdentifier was specified"));

	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		ensureAlwaysMsgf(World->GetGameInstance(), TEXT("No GameInstance found, failed to register the event"));

		if (World->GetGameInstance())
		{
			UEventManager* Manager = World->GetGameInstance()->GetSubsystem<UEventManager>();

			if (!EventIdentifier.IsEmpty())
			{
				const TSharedPtr<FStringEvent> StringEvent = MakeShareable(new FStringEvent(WorldContextObject, nullptr, EventIdentifier, Callback));
				Manager->AddEvent(StringEvent);
			}
			else
			{
				LogError("Invalid event identifier");
			}
		}
	}
}

void UEventManager::RegisterIntEvent(UObject* WorldContextObject, UEventIdentifier* EventIdentifier, const FIntEventDelegate& Callback)
{
	ensureAlwaysMsgf(EventIdentifier, TEXT("No EventIdentifier was specified"));

	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		ensureAlwaysMsgf(World->GetGameInstance(), TEXT("No GameInstance found, failed to register the event"));

		if (World->GetGameInstance())
		{
			UEventManager* Manager = World->GetGameInstance()->GetSubsystem<UEventManager>();

			if (EventIdentifier)
			{
				const TSharedPtr<FIntEvent> IntEvent = MakeShareable(new FIntEvent(WorldContextObject, EventIdentifier, "", Callback));
				Manager->AddEvent(IntEvent);
			}
			else
			{
				LogError("Invalid event identifier");
			}
		}
	}
}

void UEventManager::RegisterIntEventName(UObject* WorldContextObject, FString EventIdentifier, const FIntEventDelegate& Callback)
{
	ensureAlwaysMsgf(EventIdentifier.IsEmpty() == false, TEXT("No EventIdentifier was specified"));

	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		ensureAlwaysMsgf(World->GetGameInstance(), TEXT("No GameInstance found, failed to register the event"));

		if (World->GetGameInstance())
		{
			UEventManager* Manager = World->GetGameInstance()->GetSubsystem<UEventManager>();

			if (!EventIdentifier.IsEmpty())
			{
				const TSharedPtr<FIntEvent> IntEvent = MakeShareable(new FIntEvent(WorldContextObject, nullptr, EventIdentifier, Callback));
				Manager->AddEvent(IntEvent);
			}
			else
			{
				LogError("Invalid event identifier");
			}
		}
	}
}

void UEventManager::RegisterFloatEvent(UObject* WorldContextObject, UEventIdentifier* EventIdentifier, const FFloatEventDelegate& Callback)
{
	ensureAlwaysMsgf(EventIdentifier, TEXT("No EventIdentifier was specified"));

	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		ensureAlwaysMsgf(World->GetGameInstance(), TEXT("No GameInstance found, failed to register the event"));

		if (World->GetGameInstance())
		{
			UEventManager* Manager = World->GetGameInstance()->GetSubsystem<UEventManager>();

			if (EventIdentifier)
			{
				const TSharedPtr<FFloatEvent> FloatEvent = MakeShareable(new FFloatEvent(WorldContextObject, EventIdentifier, "", Callback));
				Manager->AddEvent(FloatEvent);
			}
			else
			{
				LogError("Invalid event identifier");
			}
		}
	}
}

void UEventManager::RegisterFloatEventName(UObject* WorldContextObject, FString EventIdentifier, const FFloatEventDelegate& Callback)
{
	ensureAlwaysMsgf(EventIdentifier.IsEmpty() == false, TEXT("No EventIdentifier was specified"));

	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		ensureAlwaysMsgf(World->GetGameInstance(), TEXT("No GameInstance found, failed to register the event"));

		if (World->GetGameInstance())
		{
			UEventManager* Manager = World->GetGameInstance()->GetSubsystem<UEventManager>();

			if (!EventIdentifier.IsEmpty())
			{
				const TSharedPtr<FFloatEvent> FloatEvent = MakeShareable(new FFloatEvent(WorldContextObject, nullptr, EventIdentifier, Callback));
				Manager->AddEvent(FloatEvent);
			}
			else
			{
				LogError("Invalid event identifier");
			}
		}
	}
}

void UEventManager::RegisterBoolEvent(UObject* WorldContextObject, UEventIdentifier* EventIdentifier, const FBoolEventDelegate& Callback)
{
	ensureAlwaysMsgf(EventIdentifier, TEXT("No EventIdentifier was specified"));

	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		ensureAlwaysMsgf(World->GetGameInstance(), TEXT("No GameInstance found, failed to register the event"));

		if (World->GetGameInstance())
		{
			UEventManager* Manager = World->GetGameInstance()->GetSubsystem<UEventManager>();

			if (EventIdentifier)
			{
				const TSharedPtr<FBoolEvent> BoolEvent = MakeShareable(new FBoolEvent(WorldContextObject, EventIdentifier, "", Callback));
				Manager->AddEvent(BoolEvent);
			}
			else
			{
				LogError("Invalid event identifier");
			}
		}
	}
}

void UEventManager::RegisterBoolEventName(UObject* WorldContextObject, FString EventIdentifier, const FBoolEventDelegate& Callback)
{
	ensureAlwaysMsgf(EventIdentifier.IsEmpty() == false, TEXT("No EventIdentifier was specified"));

	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		ensureAlwaysMsgf(World->GetGameInstance(), TEXT("No GameInstance found, failed to register the event"));

		if (World->GetGameInstance())
		{
			UEventManager* Manager = World->GetGameInstance()->GetSubsystem<UEventManager>();

			if (!EventIdentifier.IsEmpty())
			{
				const TSharedPtr<FBoolEvent> BoolEvent = MakeShareable(new FBoolEvent(WorldContextObject, nullptr, EventIdentifier, Callback));
				Manager->AddEvent(BoolEvent);
			}
			else
			{
				LogError("Invalid event identifier");
			}
		}
	}
}

void UEventManager::RegisterStringFloatEvent(UObject* WorldContextObject, UEventIdentifier* EventIdentifier, const FStringFloatEventDelegate& Callback)
{
	ensureAlwaysMsgf(EventIdentifier, TEXT("No EventIdentifier was specified"));

	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		ensureAlwaysMsgf(World->GetGameInstance(), TEXT("No GameInstance found, failed to register the event"));

		if (World->GetGameInstance())
		{
			UEventManager* Manager = World->GetGameInstance()->GetSubsystem<UEventManager>();

			if (EventIdentifier)
			{
				const TSharedPtr<FStringFloatEvent> StringFloatEvent = MakeShareable(new FStringFloatEvent(WorldContextObject, EventIdentifier, "", Callback));
				Manager->AddEvent(StringFloatEvent);
			}
			else
			{
				LogError("Invalid event identifier");
			}
		}
	}
}

void UEventManager::RegisterStringFloatEventName(UObject* WorldContextObject, FString EventIdentifier, const FStringFloatEventDelegate& Callback)
{
	ensureAlwaysMsgf(EventIdentifier.IsEmpty() == false, TEXT("No EventIdentifier was specified"));

	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		ensureAlwaysMsgf(World->GetGameInstance(), TEXT("No GameInstance found, failed to register the event"));

		if (World->GetGameInstance())
		{
			UEventManager* Manager = World->GetGameInstance()->GetSubsystem<UEventManager>();

			if (!EventIdentifier.IsEmpty())
			{
				const TSharedPtr<FStringFloatEvent> StringFloatEvent = MakeShareable(new FStringFloatEvent(WorldContextObject, nullptr, EventIdentifier, Callback));
				Manager->AddEvent(StringFloatEvent);
			}
			else
			{
				LogError("Invalid event identifier");
			}
		}
	}
}

void UEventManager::RegisterStringIntEvent(UObject* WorldContextObject, UEventIdentifier* EventIdentifier, const FStringIntEventDelegate& Callback)
{
	ensureAlwaysMsgf(EventIdentifier, TEXT("No EventIdentifier was specified"));

	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		ensureAlwaysMsgf(World->GetGameInstance(), TEXT("No GameInstance found, failed to register the event"));

		if (World->GetGameInstance())
		{
			UEventManager* Manager = World->GetGameInstance()->GetSubsystem<UEventManager>();

			if (EventIdentifier)
			{
				const TSharedPtr<FStringIntEvent> StringIntEvent = MakeShareable(new FStringIntEvent(WorldContextObject, EventIdentifier, "", Callback));
				Manager->AddEvent(StringIntEvent);
			}
			else
			{
				LogError("Invalid event identifier");
			}
		}
	}
}

void UEventManager::RegisterStringIntEventName(UObject* WorldContextObject, FString EventIdentifier, const FStringIntEventDelegate& Callback)
{
	ensureAlwaysMsgf(EventIdentifier.IsEmpty() == false, TEXT("No EventIdentifier was specified"));

	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		ensureAlwaysMsgf(World->GetGameInstance(), TEXT("No GameInstance found, failed to register the event"));

		if (World->GetGameInstance())
		{
			UEventManager* Manager = World->GetGameInstance()->GetSubsystem<UEventManager>();

			if (!EventIdentifier.IsEmpty())
			{
				const TSharedPtr<FStringIntEvent> StringIntEvent = MakeShareable(new FStringIntEvent(WorldContextObject, nullptr, EventIdentifier, Callback));
				Manager->AddEvent(StringIntEvent);
			}
			else
			{
				LogError("Invalid event identifier");
			}
		}
	}
}

void UEventManager::RegisterVectorEvent(UObject* WorldContextObject, UEventIdentifier* EventIdentifier, const FVectorEventDelegate& Callback)
{
	ensureAlwaysMsgf(EventIdentifier, TEXT("No EventIdentifier was specified"));

	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		ensureAlwaysMsgf(World->GetGameInstance(), TEXT("No GameInstance found, failed to register the event"));

		if (World->GetGameInstance())
		{
			UEventManager* Manager = World->GetGameInstance()->GetSubsystem<UEventManager>();

			if (EventIdentifier)
			{
				const TSharedPtr<FVectorEvent> VectorEvent = MakeShareable(new FVectorEvent(WorldContextObject, EventIdentifier, "", Callback));
				Manager->AddEvent(VectorEvent);
			}
			else
			{
				LogError("Invalid event identifier");
			}
		}
	}
}

void UEventManager::RegisterVectorEventName(UObject* WorldContextObject, FString EventIdentifier, const FVectorEventDelegate& Callback)
{
	ensureAlwaysMsgf(EventIdentifier.IsEmpty() == false, TEXT("No EventIdentifier was specified"));

	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		ensureAlwaysMsgf(World->GetGameInstance(), TEXT("No GameInstance found, failed to register the event"));

		if (World->GetGameInstance())
		{
			UEventManager* Manager = World->GetGameInstance()->GetSubsystem<UEventManager>();

			if (!EventIdentifier.IsEmpty())
			{
				const TSharedPtr<FVectorEvent> VectorEvent = MakeShareable(new FVectorEvent(WorldContextObject, nullptr, EventIdentifier, Callback));
				Manager->AddEvent(VectorEvent);
			}
			else
			{
				LogError("Invalid event identifier");
			}
		}
	}
}

void UEventManager::InvokeSimpleEvent(UObject* WorldContextObject, UObject* OptionalTarget, UEventIdentifier* EventIdentifier)
{
	ensureAlwaysMsgf(EventIdentifier, TEXT("No EventIdentifier was specified"));

	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		ensureAlwaysMsgf(World->GetGameInstance(), TEXT("No GameInstance found, failed to register the event"));

		if (World->GetGameInstance())
		{
			UEventManager* Manager = World->GetGameInstance()->GetSubsystem<UEventManager>();

			if (EventIdentifier)
			{
				Manager->Internal_InvokeEvent(EventIdentifier, FString(), OptionalTarget, EEventType::Simple, nullptr);
			}
		}
	}
}

void UEventManager::InvokeSimpleEventName(UObject* WorldContextObject, UObject* OptionalTarget, FString EventIdentifier)
{
	ensureAlwaysMsgf(!EventIdentifier.IsEmpty(), TEXT("No EventIdentifier was specified"));

	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		ensureAlwaysMsgf(World->GetGameInstance(), TEXT("No GameInstance found, failed to register the event"));

		if (World->GetGameInstance())
		{
			UEventManager* Manager = World->GetGameInstance()->GetSubsystem<UEventManager>();

			if (!EventIdentifier.IsEmpty())
			{
				Manager->Internal_InvokeEvent(nullptr, EventIdentifier, OptionalTarget, EEventType::Simple, nullptr);
			}
		}
	}
}

void UEventManager::InvokePayloadEvent(UObject* WorldContextObject, UObject* OptionalTarget, UEventIdentifier* EventIdentifier, UObject* Payload)
{
	ensureAlwaysMsgf(EventIdentifier, TEXT("No EventIdentifier was specified"));

	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		ensureAlwaysMsgf(World->GetGameInstance(), TEXT("No GameInstance found, failed to register the event"));

		if (World->GetGameInstance())
		{
			UEventManager* Manager = World->GetGameInstance()->GetSubsystem<UEventManager>();

			if (EventIdentifier)
			{
				Manager->Internal_InvokeEvent(EventIdentifier, FString(), OptionalTarget, EEventType::Payload, Payload);
			}
		}
	}
}

void UEventManager::InvokePayloadEventName(UObject* WorldContextObject, UObject* OptionalTarget, FString EventIdentifier, UObject* Payload)
{
	ensureAlwaysMsgf(EventIdentifier.IsEmpty() == false, TEXT("No EventIdentifier was specified"));

	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		ensureAlwaysMsgf(World->GetGameInstance(), TEXT("No GameInstance found, failed to register the event"));

		if (World->GetGameInstance())
		{
			UEventManager* Manager = World->GetGameInstance()->GetSubsystem<UEventManager>();

			if (!EventIdentifier.IsEmpty())
			{
				Manager->Internal_InvokeEvent(nullptr, EventIdentifier, OptionalTarget, EEventType::Payload, Payload);
			}
		}
	}
}

void UEventManager::InvokeStringEvent(UObject* WorldContextObject, UObject* OptionalTarget, UEventIdentifier* EventIdentifier, FString String)
{
	ensureAlwaysMsgf(EventIdentifier, TEXT("No EventIdentifier was specified"));

	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		ensureAlwaysMsgf(World->GetGameInstance(), TEXT("No GameInstance found, failed to register the event"));

		if (World->GetGameInstance())
		{
			UEventManager* Manager = World->GetGameInstance()->GetSubsystem<UEventManager>();

			if (EventIdentifier)
			{
				Manager->Internal_InvokeEvent(EventIdentifier, FString(), OptionalTarget, EEventType::String, &String);
			}
		}
	}
}

void UEventManager::InvokeStringEventName(UObject* WorldContextObject, UObject* OptionalTarget, FString EventIdentifier, FString String)
{
	ensureAlwaysMsgf(EventIdentifier.IsEmpty() == false, TEXT("No EventIdentifier was specified"));

	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		ensureAlwaysMsgf(World->GetGameInstance(), TEXT("No GameInstance found, failed to register the event"));

		if (World->GetGameInstance())
		{
			UEventManager* Manager = World->GetGameInstance()->GetSubsystem<UEventManager>();

			if (!EventIdentifier.IsEmpty())
			{
				Manager->Internal_InvokeEvent(nullptr, EventIdentifier, OptionalTarget, EEventType::String, &String);
			}
		}
	}
}

void UEventManager::InvokeIntEvent(UObject* WorldContextObject, UObject* OptionalTarget, UEventIdentifier* EventIdentifier, int32 Value)
{
	ensureAlwaysMsgf(EventIdentifier, TEXT("No EventIdentifier was specified"));

	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		ensureAlwaysMsgf(World->GetGameInstance(), TEXT("No GameInstance found, failed to register the event"));

		if (World->GetGameInstance())
		{
			UEventManager* Manager = World->GetGameInstance()->GetSubsystem<UEventManager>();

			if (EventIdentifier)
			{
				Manager->Internal_InvokeEvent(EventIdentifier, FString(), OptionalTarget, EEventType::Int, &Value);
			}
		}
	}
}

void UEventManager::InvokeIntEventName(UObject* WorldContextObject, UObject* OptionalTarget, FString EventIdentifier, int32 Value)
{
	ensureAlwaysMsgf(EventIdentifier.IsEmpty() == false, TEXT("No EventIdentifier was specified"));

	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		ensureAlwaysMsgf(World->GetGameInstance(), TEXT("No GameInstance found, failed to register the event"));

		if (World->GetGameInstance())
		{
			UEventManager* Manager = World->GetGameInstance()->GetSubsystem<UEventManager>();

			if (!EventIdentifier.IsEmpty())
			{
				Manager->Internal_InvokeEvent(nullptr, EventIdentifier, OptionalTarget, EEventType::Int, &Value);
			}
		}
	}
}

void UEventManager::InvokeFloatEvent(UObject* WorldContextObject, UObject* OptionalTarget, UEventIdentifier* EventIdentifier, float value)
{
	ensureAlwaysMsgf(EventIdentifier, TEXT("No EventIdentifier was specified"));

	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		ensureAlwaysMsgf(World->GetGameInstance(), TEXT("No GameInstance found, failed to register the event"));

		if (World->GetGameInstance())
		{
			UEventManager* Manager = World->GetGameInstance()->GetSubsystem<UEventManager>();

			if (EventIdentifier)
			{
				Manager->Internal_InvokeEvent(EventIdentifier, FString(), OptionalTarget, EEventType::Float, &value);
			}
		}
	}
}

void UEventManager::InvokeFloatEventName(UObject* WorldContextObject, UObject* OptionalTarget, FString EventIdentifier, float Value)
{
	ensureAlwaysMsgf(EventIdentifier.IsEmpty() == false, TEXT("No EventIdentifier was specified"));

	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		ensureAlwaysMsgf(World->GetGameInstance(), TEXT("No GameInstance found, failed to register the event"));

		if (World->GetGameInstance())
		{
			UEventManager* Manager = World->GetGameInstance()->GetSubsystem<UEventManager>();

			if (!EventIdentifier.IsEmpty())
			{
				Manager->Internal_InvokeEvent(nullptr, EventIdentifier, OptionalTarget, EEventType::Float, &Value);
			}
		}
	}
}

void UEventManager::InvokeBoolEvent(UObject* WorldContextObject, UObject* OptionalTarget, UEventIdentifier* EventIdentifier, bool value)
{
	ensureAlwaysMsgf(EventIdentifier, TEXT("No EventIdentifier was specified"));

	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		ensureAlwaysMsgf(World->GetGameInstance(), TEXT("No GameInstance found, failed to register the event"));

		if (World->GetGameInstance())
		{
			UEventManager* Manager = World->GetGameInstance()->GetSubsystem<UEventManager>();

			if (EventIdentifier)
			{
				Manager->Internal_InvokeEvent(EventIdentifier, FString(), OptionalTarget, EEventType::Bool, &value);
			}
		}
	}
}

void UEventManager::InvokeBoolEventName(UObject* WorldContextObject, UObject* OptionalTarget, FString EventIdentifier, bool bValue)
{
	ensureAlwaysMsgf(EventIdentifier.IsEmpty() == false, TEXT("No EventIdentifier was specified"));

	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		ensureAlwaysMsgf(World->GetGameInstance(), TEXT("No GameInstance found, failed to register the event"));

		if (World->GetGameInstance())
		{
			UEventManager* Manager = World->GetGameInstance()->GetSubsystem<UEventManager>();

			if (!EventIdentifier.IsEmpty())
			{
				Manager->Internal_InvokeEvent(nullptr, EventIdentifier, OptionalTarget, EEventType::Bool, &bValue);
			}
		}
	}
}

void UEventManager::InvokeStringFloatEvent(UObject* WorldContextObject, UObject* OptionalTarget, UEventIdentifier* EventIdentifier, FStringFloat Data)
{
	ensureAlwaysMsgf(EventIdentifier, TEXT("No EventIdentifier was specified"));

	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		ensureAlwaysMsgf(World->GetGameInstance(), TEXT("No GameInstance found, failed to register the event"));

		if (World->GetGameInstance())
		{
			UEventManager* Manager = World->GetGameInstance()->GetSubsystem<UEventManager>();

			if (EventIdentifier)
			{
				Manager->Internal_InvokeEvent(EventIdentifier, FString(), OptionalTarget, EEventType::StringFloat, &Data);
			}
		}
	}
}

void UEventManager::InvokeStringFloatEventName(UObject* WorldContextObject, UObject* OptionalTarget, FString EventIdentifier, FStringFloat Data)
{
	ensureAlwaysMsgf(EventIdentifier.IsEmpty() == false, TEXT("No EventIdentifier was specified"));

	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		ensureAlwaysMsgf(World->GetGameInstance(), TEXT("No GameInstance found, failed to register the event"));

		if (World->GetGameInstance())
		{
			UEventManager* Manager = World->GetGameInstance()->GetSubsystem<UEventManager>();

			if (!EventIdentifier.IsEmpty())
			{
				Manager->Internal_InvokeEvent(nullptr, EventIdentifier, OptionalTarget, EEventType::StringFloat, &Data);
			}
		}
	}
}

void UEventManager::InvokeStringIntEvent(UObject* WorldContextObject, UObject* OptionalTarget, UEventIdentifier* EventIdentifier, FStringInt Data)
{
	ensureAlwaysMsgf(EventIdentifier, TEXT("No EventIdentifier was specified"));

	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		ensureAlwaysMsgf(World->GetGameInstance(), TEXT("No GameInstance found, failed to register the event"));

		if (World->GetGameInstance())
		{
			UEventManager* Manager = World->GetGameInstance()->GetSubsystem<UEventManager>();

			if (EventIdentifier)
			{
				Manager->Internal_InvokeEvent(EventIdentifier, FString(), OptionalTarget, EEventType::StringInt, &Data);
			}
		}
	}
}

void UEventManager::InvokeStringIntEventName(UObject* WorldContextObject, UObject* OptionalTarget, FString EventIdentifier, FStringInt Data)
{
	ensureAlwaysMsgf(EventIdentifier.IsEmpty() == false, TEXT("No EventIdentifier was specified"));

	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		ensureAlwaysMsgf(World->GetGameInstance(), TEXT("No GameInstance found, failed to register the event"));

		if (World->GetGameInstance())
		{
			UEventManager* Manager = World->GetGameInstance()->GetSubsystem<UEventManager>();

			if (!EventIdentifier.IsEmpty())
			{
				Manager->Internal_InvokeEvent(nullptr, EventIdentifier, OptionalTarget, EEventType::StringInt, &Data);
			}
		}
	}
}

void UEventManager::InvokeVectorEvent(UObject* WorldContextObject, UObject* OptionalTarget, UEventIdentifier* EventIdentifier, FVector Data)
{
	ensureAlwaysMsgf(EventIdentifier, TEXT("No EventIdentifier was specified"));

	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		ensureAlwaysMsgf(World->GetGameInstance(), TEXT("No GameInstance found, failed to register the event"));

		if (World->GetGameInstance())
		{
			UEventManager* Manager = World->GetGameInstance()->GetSubsystem<UEventManager>();

			if (EventIdentifier)
			{
				Manager->Internal_InvokeEvent(EventIdentifier, FString(), OptionalTarget, EEventType::Vector, &Data);
			}
		}
	}
}

void UEventManager::InvokeVectorEventName(UObject* WorldContextObject, UObject* OptionalTarget, FString EventIdentifier, FVector Data)
{
	ensureAlwaysMsgf(EventIdentifier.IsEmpty() == false, TEXT("No EventIdentifier was specified"));

	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		ensureAlwaysMsgf(World->GetGameInstance(), TEXT("No GameInstance found, failed to register the event"));

		if (World->GetGameInstance())
		{
			UEventManager* Manager = World->GetGameInstance()->GetSubsystem<UEventManager>();

			if (!EventIdentifier.IsEmpty())
			{
				Manager->Internal_InvokeEvent(nullptr, EventIdentifier, OptionalTarget, EEventType::Vector, &Data);
			}
		}
	}
}

void UEventManager::DebugEventManager(UObject* WorldContextObject)
{

	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		ensureAlwaysMsgf(World->GetGameInstance(), TEXT("No GameInstance found, failed to register the event"));

		if (World->GetGameInstance())
		{
			UEventManager* Manager = World->GetGameInstance()->GetSubsystem<UEventManager>();

			UE_LOG(LogTemp, Warning, TEXT("///////////////////////// DEBUG INFO /////////////////////////"));
			UE_LOG(LogTemp, Warning, TEXT("Registered m_Events: (%i)"), Manager->m_Events.Num());

			for (int32 i = 0; i < Manager->m_Events.Num(); i++)
			{
				if (Manager->m_Events[i].IsValid())
				{
					UE_LOG(LogTemp, Warning, TEXT("Name: %s | Type: %s"), *Manager->m_Events[i]->GetIdentifier()->GetName(), *GetDispatcherTypeAsString(Manager->m_Events[i]->GetType()));
				}
			}

			UE_LOG(LogTemp, Warning, TEXT("--------- QUEUE INFO ---------"));
			UE_LOG(LogTemp, Warning, TEXT("Number queued events: (%i)"), Manager->m_InQueue.Num());

			for (int32 i = 0; i < Manager->m_InQueue.Num(); i++)
			{
				if (Manager->m_InQueue[i].IsValid())
				{
					UE_LOG(LogTemp, Warning, TEXT("Name: %s | Type: %s"), *Manager->m_InQueue[i]->GetIdentifier()->GetName(), *GetDispatcherTypeAsString(Manager->m_InQueue[i]->GetType()));
				} 
			}

			UE_LOG(LogTemp, Warning, TEXT("///////////////////////// DEBUG INFO /////////////////////////"));
		}
	}
}

void UEventManager::UnregisterEvent(UObject* WorldContextObject, UEventIdentifier* EventIdentifier)
{
	ensureAlwaysMsgf(EventIdentifier, TEXT("No EventIdentifier was specified"));

	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		ensureAlwaysMsgf(World->GetGameInstance(), TEXT("No GameInstance found, failed to register the event"));

		if (World->GetGameInstance())
		{
			UEventManager* Manager = World->GetGameInstance()->GetSubsystem<UEventManager>();
			check(Manager);

			if (EventIdentifier)
			{
				Manager->Internal_UnregisterEvent(EventIdentifier);
			}
			else
			{
				LogError("Invalid event identifier");
			}
		}
	}
}

void UEventManager::UnregisterEventName(UObject* WorldContextObject, FString EventIdentifier)
{
	ensureAlwaysMsgf(EventIdentifier.IsEmpty() == false, TEXT("No EventIdentifier was specified"));

	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		ensureAlwaysMsgf(World->GetGameInstance(), TEXT("No GameInstance found, failed to register the event"));

		if (World->GetGameInstance())
		{
			UEventManager* Manager = World->GetGameInstance()->GetSubsystem<UEventManager>();
			check(Manager);

			if (!EventIdentifier.IsEmpty())
			{
				Manager->Internal_UnregisterEventName(EventIdentifier);
			}
			else
			{
				LogError("Invalid event identifier");
			}
		}
	}
}

void UEventManager::UnregisterEvents(UObject* WorldContextObject)
{

	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		ensureAlwaysMsgf(World->GetGameInstance(), TEXT("No GameInstance found, failed to register the event"));

		if (World->GetGameInstance())
		{
			UEventManager* Manager = World->GetGameInstance()->GetSubsystem<UEventManager>();

			Manager->Internal_UnregisterEvents();
		}
	}
}

void UEventManager::AddQueueEvent(TSharedPtr<FBaseEvent> Event)
{

	bool bEventAdded = false;

	if (bInitialized)
	{
		if (Event)
		{
			FScopeLock QueueLock(&m_QueueLock);

			for (auto& Element : m_InQueue)
			{
				if (!Element.IsValid() || Element == nullptr)
				{
					Element = Event;
					bEventAdded = true;
					
					break;
				}
			}
		}
	}
	else
	{
		LogError("EventManager has not been initialized");
	}

	if (!bEventAdded)
	{
		LogError("Event Queue was full, event was not added. Consider increasing the queue size.");
	}
}

void UEventManager::AddEvent(TSharedPtr<FBaseEvent> Event)
{
	check(Event);

	if (bInitialized)
	{
		if (Event)
		{
			FScopeLock EventsLock(&m_EventsLock);

			m_Events.Add(Event);

			LogVerbose("Event Type: %s Registered. Identifier: %s", *GetDispatcherTypeAsString(Event->GetType()), *Event->GetEventName());
		}
	}
	else
	{
		LogError("EventManager has not been initialized");
	}
}

int32 UEventManager::GetNumEvents() const
{
	FScopeLock EventsLock(&m_EventsLock);
	return m_Events.Num();
}
