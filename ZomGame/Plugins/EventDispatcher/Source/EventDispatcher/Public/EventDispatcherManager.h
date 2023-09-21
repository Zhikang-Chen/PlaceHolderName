/**
* Copyright (C) 2017-2020 | Dry Eel Development
*
* Official EventDispatcher Documentation: https://eeldev.com/index.php/eventdispatcher-plugin/
*/

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "EventDispatcherTypes.h"
#include "Containers/Ticker.h"
#include "Misc/EngineVersionComparison.h"
#include "EventDispatcherManager.generated.h"

UCLASS()
class EVENTDISPATCHER_API UEventManager : public UGameInstanceSubsystem
#if UE_VERSION_OLDER_THAN(5,0,0)
, public FTickerObjectBase
#else
, public FTSTickerObjectBase
#endif
{
	GENERATED_BODY()
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual bool Tick(float deltaTime) override;
public:
	/**
	* Register a new event
	*
	* @param	EventIdentifier		Identifier for this event
	* @param	Callback			Callback to execute when this event gets called
	*/
	UFUNCTION(BlueprintCallable, Category = "Event Dispatcher", meta = (WorldContext = "WorldContextObject"))
	static void RegisterSimpleEvent(UObject* WorldContextObject, UEventIdentifier* EventIdentifier, const FSimpleEventDelegate& Callback);

	/**
	* Register a new event (Name)
	*
	* @param	EventIdentifier		Identifier for this event
	* @param	Callback			Callback to execute when this event gets called
	*/
	UFUNCTION(BlueprintCallable, Category = "Event Dispatcher", meta = (WorldContext = "WorldContextObject"))
	static void RegisterSimpleEventName(UObject* WorldContextObject, FString EventIdentifier, const FSimpleEventDelegate& Callback);

	/**
	* Register a new payload event
	*
	* @param	EventIdentifier		Identifier for this event
	* @param	Callback			Callback to execute when this event gets called
	*/
	UFUNCTION(BlueprintCallable, Category = "Event Dispatcher", meta = (WorldContext = "WorldContextObject"))
	static void RegisterPayloadEvent(UObject* WorldContextObject, UEventIdentifier* EventIdentifier, const FPayloadEventDelegate& Callback);

	/**
	* Register a new payload event
	*
	* @param	EventIdentifier		Identifier for this event
	* @param	Callback			Callback to execute when this event gets called
	*/
	UFUNCTION(BlueprintCallable, Category = "Event Dispatcher", meta = (WorldContext = "WorldContextObject"))
	static void RegisterPayloadEventName(UObject* WorldContextObject, FString EventIdentifier, const FPayloadEventDelegate& Callback);

	/**
	* Register a new string event
	*
	* @param	EventIdentifier		Identifier for this event
	* @param	Callback			Callback to execute when this event gets called
	*/
	UFUNCTION(BlueprintCallable, Category = "Event Dispatcher", meta = (WorldContext = "WorldContextObject"))
	static void RegisterStringEvent(UObject* WorldContextObject, UEventIdentifier* EventIdentifier, const FStringEventDelegate& Callback);

	/**
	* Register a new string event
	*
	* @param	EventIdentifier		Identifier for this event
	* @param	Callback			Callback to execute when this event gets called
	*/
	UFUNCTION(BlueprintCallable, Category = "Event Dispatcher", meta = (WorldContext = "WorldContextObject"))
	static void RegisterStringEventName(UObject* WorldContextObject, FString EventIdentifier, const FStringEventDelegate& Callback);

	/**
	* Register a new int event
	*
	* @param	EventIdentifier		Identifier for this event
	* @param	Callback			Callback to execute when this event gets called
	*/
	UFUNCTION(BlueprintCallable, Category = "Event Dispatcher", meta = (WorldContext = "WorldContextObject"))
	static void RegisterIntEvent(UObject* WorldContextObject, UEventIdentifier* EventIdentifier, const FIntEventDelegate& Callback);

	/**
	* Register a new int event
	*
	* @param	EventIdentifier		Identifier for this event
	* @param	Callback			Callback to execute when this event gets called
	*/
	UFUNCTION(BlueprintCallable, Category = "Event Dispatcher", meta = (WorldContext = "WorldContextObject"))
	static void RegisterIntEventName(UObject* WorldContextObject, FString EventIdentifier, const FIntEventDelegate& Callback);
	
	/**
	* Register a new float event
	*
	* @param	EventIdentifier		Identifier for this event
	* @param	Callback			Callback to execute when this event gets called
	*/
	UFUNCTION(BlueprintCallable, Category = "Event Dispatcher", meta = (WorldContext = "WorldContextObject"))
	static void RegisterFloatEvent(UObject* WorldContextObject, UEventIdentifier* EventIdentifier, const FFloatEventDelegate& Callback);

	/**
	* Register a new float event
	*
	* @param	EventIdentifier		Identifier for this event
	* @param	Callback			Callback to execute when this event gets called
	*/
	UFUNCTION(BlueprintCallable, Category = "Event Dispatcher", meta = (WorldContext = "WorldContextObject"))
	static void RegisterFloatEventName(UObject* WorldContextObject, FString EventIdentifier, const FFloatEventDelegate& Callback);

	/**
	* Register a new bool event
	*
	* @param	EventIdentifier		Identifier for this event
	* @param	Callback			Callback to execute when this event gets called
	*/
	UFUNCTION(BlueprintCallable, Category = "Event Dispatcher", meta = (WorldContext = "WorldContextObject"))
	static void RegisterBoolEvent(UObject* WorldContextObject, UEventIdentifier* EventIdentifier, const FBoolEventDelegate& Callback);

	/**
	* Register a new bool event
	*
	* @param	EventIdentifier		Identifier for this event
	* @param	Callback			Callback to execute when this event gets called
	*/
	UFUNCTION(BlueprintCallable, Category = "Event Dispatcher", meta = (WorldContext = "WorldContextObject"))
	static void RegisterBoolEventName(UObject* WorldContextObject, FString EventIdentifier, const FBoolEventDelegate& Callback);

	/**
	* Register a new StringFloat event
	*
	* @param	EventIdentifier		Identifier for this event
	* @param	Callback			Callback to execute when this event gets called
	*/
	UFUNCTION(BlueprintCallable, Category = "Event Dispatcher", meta = (WorldContext = "WorldContextObject"))
	static void RegisterStringFloatEvent(UObject* WorldContextObject, UEventIdentifier* EventIdentifier, const FStringFloatEventDelegate& Callback);

	/**
	* Register a new StringFloat event
	*
	* @param	EventIdentifier		Identifier for this event
	* @param	Callback			Callback to execute when this event gets called
	*/
	UFUNCTION(BlueprintCallable, Category = "Event Dispatcher", meta = (WorldContext = "WorldContextObject"))
	static void RegisterStringFloatEventName(UObject* WorldContextObject, FString EventIdentifier, const FStringFloatEventDelegate& Callback);
	
	/**
	* Register a new StringInt event
	*
	* @param	EventIdentifier		Identifier for this event
	* @param	Callback			Callback to execute when this event gets called
	*/
	UFUNCTION(BlueprintCallable, Category = "Event Dispatcher", meta = (WorldContext = "WorldContextObject"))
	static void RegisterStringIntEvent(UObject* WorldContextObject, UEventIdentifier* EventIdentifier, const FStringIntEventDelegate& Callback);

	/**
	* Register a new StringInt event
	*
	* @param	EventIdentifier		Identifier for this event
	* @param	Callback			Callback to execute when this event gets called
	*/
	UFUNCTION(BlueprintCallable, Category = "Event Dispatcher", meta = (WorldContext = "WorldContextObject"))
	static void RegisterStringIntEventName(UObject* WorldContextObject, FString EventIdentifier, const FStringIntEventDelegate& Callback);
	
	/**
	* Register a new Vector event
	*
	* @param	EventIdentifier		Identifier for this event
	* @param	Callback			Callback to execute when this event gets called
	*/
	UFUNCTION(BlueprintCallable, Category = "Event Dispatcher", meta = (WorldContext = "WorldContextObject"))
	static void RegisterVectorEvent(UObject* WorldContextObject, UEventIdentifier* EventIdentifier, const FVectorEventDelegate& Callback);

	/**
	* Register a new Vector event
	*
	* @param	EventIdentifier		Identifier for this event
	* @param	Callback			Callback to execute when this event gets called
	*/
	UFUNCTION(BlueprintCallable, Category = "Event Dispatcher", meta = (WorldContext = "WorldContextObject"))
	static void RegisterVectorEventName(UObject* WorldContextObject, FString EventIdentifier, const FVectorEventDelegate& Callback);
	
	/**
	* Invoke (call) the event
	* 
	* @param	EventIdentifier		Identifier for this event
	* @param	OptionalTarget		The target object to invoke this event on, this parameter is Optional and if left empty the event will be called on ALL objects that are listening for this event
	*/
	UFUNCTION(BlueprintCallable, Category = "Event Dispatcher", meta = (WorldContext = "WorldContextObject"))
	static void InvokeSimpleEvent(UObject* WorldContextObject, UObject* OptionalTarget, UEventIdentifier* EventIdentifier);
	
	/**
	* Invoke (call) the event
	* 
	* @param	EventIdentifier		Identifier for this event
	* @param	OptionalTarget		The target object to invoke this event on, this parameter is Optional and if left empty the event will be called on ALL objects that are listening for this event
	*/
	UFUNCTION(BlueprintCallable, Category = "Event Dispatcher", meta = (WorldContext = "WorldContextObject"))
	static void InvokeSimpleEventName(UObject* WorldContextObject, UObject* OptionalTarget, FString EventIdentifier);
	
	/**
	* Invoke (call) the event
	*
	* @param	EventIdentifier		Identifier for this event
	* @param	Payload				Object to send with the event
	* @param	OptionalTarget		The target object to invoke this event on, this parameter is Optional and if left empty the event will be called on ALL objects that are listening for this event
	*/
	UFUNCTION(BlueprintCallable, Category = "Event Dispatcher", meta = (WorldContext = "WorldContextObject"))
	static void InvokePayloadEvent(UObject* WorldContextObject, UObject* OptionalTarget, UEventIdentifier* EventIdentifier, UObject* Payload);

	/**
	* Invoke (call) the event
	*
	* @param	EventIdentifier		Identifier for this event
	* @param	Payload				Object to send with the event
	* @param	OptionalTarget		The target object to invoke this event on, this parameter is Optional and if left empty the event will be called on ALL objects that are listening for this event
	*/
	UFUNCTION(BlueprintCallable, Category = "Event Dispatcher", meta = (WorldContext = "WorldContextObject"))
	static void InvokePayloadEventName(UObject* WorldContextObject, UObject* OptionalTarget, FString EventIdentifier, UObject* Payload);
	
	/**
	* Invoke (call) the event
	*
	* @param	EventIdentifier		Identifier for this event
	* @param	OptionalTarget		The target object to invoke this event on, this parameter is Optional and if left empty the event will be called on ALL objects that are listening for this event
	* @param	String				String to send
	*/
	UFUNCTION(BlueprintCallable, Category = "Event Dispatcher", meta = (WorldContext = "WorldContextObject"))
	static void InvokeStringEvent(UObject* WorldContextObject, UObject* OptionalTarget, UEventIdentifier* EventIdentifier, FString String);

	/**
	* Invoke (call) the event
	*
	* @param	EventIdentifier		Identifier for this event
	* @param	OptionalTarget		The target object to invoke this event on, this parameter is Optional and if left empty the event will be called on ALL objects that are listening for this event
	* @param	String				String to send
	*/
	UFUNCTION(BlueprintCallable, Category = "Event Dispatcher", meta = (WorldContext = "WorldContextObject"))
	static void InvokeStringEventName(UObject* WorldContextObject, UObject* OptionalTarget, FString EventIdentifier, FString String);
	
	/**
	* Invoke (call) the event
	*
	* @param	EventIdentifier		Identifier for this event
	* @param	OptionalTarget		The target object to invoke this event on, this parameter is Optional and if left empty the event will be called on ALL objects that are listening for this event
	* @param	Value				Value to send
	*/
	UFUNCTION(BlueprintCallable, Category = "Event Dispatcher", meta = (WorldContext = "WorldContextObject"))
	static void InvokeIntEvent(UObject* WorldContextObject, UObject* OptionalTarget, UEventIdentifier* EventIdentifier, int32 Value);

	/**
	* Invoke (call) the event
	*
	* @param	EventIdentifier		Identifier for this event
	* @param	OptionalTarget		The target object to invoke this event on, this parameter is Optional and if left empty the event will be called on ALL objects that are listening for this event
	* @param	Value				Value to send
	*/
	UFUNCTION(BlueprintCallable, Category = "Event Dispatcher", meta = (WorldContext = "WorldContextObject"))
	static void InvokeIntEventName(UObject* WorldContextObject, UObject* OptionalTarget, FString EventIdentifier, int32 Value);
	
	/**
	* Invoke (call) the event
	*
	* @param	EventIdentifier		Identifier for this event
	* @param	OptionalTarget		The target object to invoke this event on, this parameter is Optional and if left empty the event will be called on ALL objects that are listening for this event
	* @param	Value				Value to send
	*/
	UFUNCTION(BlueprintCallable, Category = "Event Dispatcher", meta = (WorldContext = "WorldContextObject"))
	static void InvokeFloatEvent(UObject* WorldContextObject, UObject* OptionalTarget, UEventIdentifier* EventIdentifier, float Value);

	/**
	* Invoke (call) the event
	*
	* @param	EventIdentifier		Identifier for this event
	* @param	OptionalTarget		The target object to invoke this event on, this parameter is Optional and if left empty the event will be called on ALL objects that are listening for this event
	* @param	Value				Value to send
	*/
	UFUNCTION(BlueprintCallable, Category = "Event Dispatcher", meta = (WorldContext = "WorldContextObject"))
	static void InvokeFloatEventName(UObject* WorldContextObject, UObject* OptionalTarget, FString EventIdentifier, float Value);

	/**
	* Invoke (call) the event
	*
	* @param	EventIdentifier		Identifier for this event
	* @param	OptionalTarget		The target object to invoke this event on, this parameter is Optional and if left empty the event will be called on ALL objects that are listening for this event
	* @param	bValue				Value to send
	*/
	UFUNCTION(BlueprintCallable, Category = "Event Dispatcher", meta = (WorldContext = "WorldContextObject"))
	static void InvokeBoolEvent(UObject* WorldContextObject, UObject* OptionalTarget, UEventIdentifier* EventIdentifier, bool bValue);

	/**
	* Invoke (call) the event
	*
	* @param	EventIdentifier		Identifier for this event
	* @param	OptionalTarget		The target object to invoke this event on, this parameter is Optional and if left empty the event will be called on ALL objects that are listening for this event
	* @param	bValue				Value to send
	*/
	UFUNCTION(BlueprintCallable, Category = "Event Dispatcher", meta = (WorldContext = "WorldContextObject"))
	static void InvokeBoolEventName(UObject* WorldContextObject, UObject* OptionalTarget, FString EventIdentifier, bool bValue);

	/**
	* Invoke (call) the event
	*
	* @param	EventIdentifier		Identifier for this event
	* @param	OptionalTarget		The target object to invoke this event on, this parameter is Optional and if left empty the event will be called on ALL objects that are listening for this event
	* @param	Data				Data to send
	*/
	UFUNCTION(BlueprintCallable, Category = "Event Dispatcher", meta = (WorldContext = "WorldContextObject"))
	static void InvokeStringFloatEvent(UObject* WorldContextObject, UObject* OptionalTarget, UEventIdentifier* EventIdentifier, FStringFloat Data);

	/**
	* Invoke (call) the event
	*
	* @param	EventIdentifier		Identifier for this event
	* @param	OptionalTarget		The target object to invoke this event on, this parameter is Optional and if left empty the event will be called on ALL objects that are listening for this event
	* @param	Data				Data to send
	*/
	UFUNCTION(BlueprintCallable, Category = "Event Dispatcher", meta = (WorldContext = "WorldContextObject"))
	static void InvokeStringFloatEventName(UObject* WorldContextObject, UObject* OptionalTarget, FString EventIdentifier, FStringFloat Data);
	
	/**
	* Invoke (call) the event
	*
	* @param	EventIdentifier		Identifier for this event
	* @param	OptionalTarget		The target object to invoke this event on, this parameter is Optional and if left empty the event will be called on ALL objects that are listening for this event
	* @param	Data				Data to send
	*/
	UFUNCTION(BlueprintCallable, Category = "Event Dispatcher", meta = (WorldContext = "WorldContextObject"))
	static void InvokeStringIntEvent(UObject* WorldContextObject, UObject* OptionalTarget, UEventIdentifier* EventIdentifier, FStringInt Data);

	/**
	* Invoke (call) the event
	*
	* @param	EventIdentifier		Identifier for this event
	* @param	OptionalTarget		The target object to invoke this event on, this parameter is Optional and if left empty the event will be called on ALL objects that are listening for this event
	* @param	Data				Data to send
	*/
	UFUNCTION(BlueprintCallable, Category = "Event Dispatcher", meta = (WorldContext = "WorldContextObject"))
	static void InvokeStringIntEventName(UObject* WorldContextObject, UObject* OptionalTarget, FString EventIdentifier, FStringInt Data);
	
	/**
	* Invoke (call) the event
	*
	* @param	EventIdentifier		Identifier for this event
	* @param	OptionalTarget		The target object to invoke this event on, this parameter is Optional and if left empty the event will be called on ALL objects that are listening for this event
	* @param	Data				Data to send
	*/
	UFUNCTION(BlueprintCallable, Category = "Event Dispatcher", meta = (WorldContext = "WorldContextObject"))
	static void InvokeVectorEvent(UObject* WorldContextObject, UObject* OptionalTarget, UEventIdentifier* EventIdentifier, FVector Data);

	/**
	* Invoke (call) the event
	*
	* @param	EventIdentifier		Identifier for this event
	* @param	OptionalTarget		The target object to invoke this event on, this parameter is Optional and if left empty the event will be called on ALL objects that are listening for this event
	* @param	Data				Data to send
	*/
	UFUNCTION(BlueprintCallable, Category = "Event Dispatcher", meta = (WorldContext = "WorldContextObject"))
	static void InvokeVectorEventName(UObject* WorldContextObject, UObject* OptionalTarget, FString EventIdentifier, FVector Data);
	
	/**
	* Unregister an event
	*
	* @param	EventIdentifier		Identifier for this event
	*/
	UFUNCTION(BlueprintCallable, Category = "Event Dispatcher", meta = (WorldContext = "WorldContextObject"))
	static void UnregisterEvent(UObject* WorldContextObject, UEventIdentifier* EventIdentifier);

	/**
	* Unregister an event
	*
	* @param	EventIdentifier		Identifier for this event
	*/
	UFUNCTION(BlueprintCallable, Category = "Event Dispatcher", meta = (WorldContext = "WorldContextObject"))
	static void UnregisterEventName(UObject* WorldContextObject, FString EventIdentifier);

	/**
	* Unregister all events
	*/
	UFUNCTION(BlueprintCallable, Category = "Event Dispatcher", meta = (WorldContext = "WorldContextObject"))
	static void UnregisterEvents(UObject* WorldContextObject);

	/**
	* Print all active delegates and objects
	*/
	UFUNCTION(BlueprintCallable, Category = "Event Dispatcher", meta = (WorldContext = "WorldContextObject"))
	static void DebugEventManager(UObject* WorldContextObject);
protected:
	void HandleQueue();
	void Internal_InvokeEvent(UEventIdentifier* EventIdentifier, FString EventIdentifierName, UObject* OptionalTarget, EEventType Type, void* Data);
	void Internal_UnregisterEvents();
	void Internal_UnregisterEvent(UEventIdentifier* EventIdentifier);
	void Internal_UnregisterEventName(FString EventIdentifier);
	void AddQueueEvent(TSharedPtr<FBaseEvent> Event);
	void AddEvent(TSharedPtr<FBaseEvent> Event);
	int32 GetNumEvents() const;
protected:
	TArray<TSharedPtr<FBaseEvent>> m_Events;
	TArray<TSharedPtr<FBaseEvent>, TFixedAllocator<100>> m_InQueue;

	mutable FCriticalSection m_EventsLock;
	mutable FCriticalSection m_QueueLock;
};
