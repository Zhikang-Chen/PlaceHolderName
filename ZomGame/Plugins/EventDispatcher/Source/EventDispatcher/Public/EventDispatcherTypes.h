/**
* Copyright (C) 2017-2020 | Dry Eel Development
*
* Official EventDispatcher Documentation: https://eeldev.com/index.php/eventdispatcher-plugin/
*/

#pragma once

#include "CoreMinimal.h"
#include <Engine/DataAsset.h>
#include "EventDispatcherTypes.generated.h"

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
//		Structs
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //

UCLASS(BlueprintType)
class UEventIdentifier : public UDataAsset
{
	GENERATED_BODY()
public:
	// A Event Identifier description, this is only meant to be used internally as a description, not used in any code.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data")
	FText EventDescription;
};

USTRUCT(BlueprintType)
struct FStringFloat
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data")
	FString String;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data")
	float Float;
public:
	FStringFloat()
		: Float(0.0f)
	{
	}

	explicit FStringFloat(const FString& String, const float& Value)
		: String(String), Float(Value)
	{
	}
};

USTRUCT(BlueprintType)
struct FStringInt
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data")
	FString String;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data")
	int32 Value;
public:
	FStringInt()
		: Value(0)
	{
	}

	explicit FStringInt(const FString& String, const int32& Value)
		: String(String), Value(Value)
	{
	}
};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
//		Objects
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //

UCLASS(Blueprintable)
class EVENTDISPATCHER_API UPayloadObject : public UObject
{
	GENERATED_BODY()
public:
};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
//		Enums
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //

UENUM(BlueprintType)
enum class EEventType : uint8
{
	Invalid = 0,
	Simple,
	Payload,
	String,
	Int,
	Float,
	Bool,
	StringFloat,
	StringInt,
	Vector
};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
//		Delegates
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //

DECLARE_DYNAMIC_DELEGATE(FSimpleEventDelegate);
DECLARE_DYNAMIC_DELEGATE_OneParam(FPayloadEventDelegate, UObject*, Payload);
DECLARE_DYNAMIC_DELEGATE_OneParam(FStringEventDelegate, const FString&, String);
DECLARE_DYNAMIC_DELEGATE_OneParam(FIntEventDelegate, int32, Value);
DECLARE_DYNAMIC_DELEGATE_OneParam(FFloatEventDelegate, float, Value);
DECLARE_DYNAMIC_DELEGATE_OneParam(FBoolEventDelegate, bool, Value);
DECLARE_DYNAMIC_DELEGATE_OneParam(FStringFloatEventDelegate, const FStringFloat&, Data);
DECLARE_DYNAMIC_DELEGATE_OneParam(FStringIntEventDelegate, const FStringInt&, Data);
DECLARE_DYNAMIC_DELEGATE_OneParam(FVectorEventDelegate, const FVector&, Data);

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
//		FBaseEvent
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //

struct FBaseEvent
{
public:
	TWeakObjectPtr<UObject> m_ContextObject;
public:
	explicit FBaseEvent(UObject* ContextObject, UEventIdentifier* EventIdentifier, FString EventIdentifierName, EEventType Type)
		: m_ContextObject(ContextObject), m_EventIdentifier(EventIdentifier), m_EventIdentifierName(EventIdentifierName), m_Type(Type)
	{
	}

	virtual ~FBaseEvent()
	{
	}

public:
	UEventIdentifier* GetIdentifier() const { return m_EventIdentifier; }
	FString GetIdentifierName() const { return m_EventIdentifierName; }
	EEventType GetType() const { return m_Type; }
	bool IsValid() const { return m_Type != EEventType::Invalid; }
	void Invalidate() { m_Type = EEventType::Invalid; }

	FString GetEventName() const
	{
		return m_EventIdentifier ? m_EventIdentifier->GetName() : m_EventIdentifierName;
	}
	
	virtual void TriggerDelegates()
	{
	}

protected:
	UEventIdentifier* m_EventIdentifier;
	FString m_EventIdentifierName;
	TWeakObjectPtr<UObject> m_OptionalTarget;
	EEventType m_Type;
};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
//		FSimpleEvent
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //

struct FSimpleEvent : public FBaseEvent
{
public:
	explicit FSimpleEvent(UObject* ContextObject, UEventIdentifier* EventIdentifier, FString EventIdentifierName, const FSimpleEventDelegate& Delegate)
		: FBaseEvent(ContextObject, EventIdentifier, EventIdentifierName, EEventType::Simple), m_Delegate(Delegate)
	{
	}

	explicit FSimpleEvent(const FSimpleEvent& Other)
		: FBaseEvent(Other), m_Delegate(Other.m_Delegate)
	{
	}

	virtual ~FSimpleEvent()
	{
		m_Delegate.Unbind();
		Invalidate();
	}

	virtual void TriggerDelegates() override
	{
		if (IsValid())
		{
			m_Delegate.ExecuteIfBound();
		}
	}
protected:
	
	FSimpleEventDelegate m_Delegate;
};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
//		FPayloadEvent
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //

struct FPayloadEvent : public FBaseEvent
{
public:
	explicit FPayloadEvent(UObject* ContextObject, UEventIdentifier* EventIdentifier, FString EventIdentifierName, const FPayloadEventDelegate& Delegate)
		: FBaseEvent(ContextObject, EventIdentifier, EventIdentifierName, EEventType::Payload), m_Delegate(Delegate)
	{
	}

	explicit FPayloadEvent(const FPayloadEvent& Other, void* Data)
		: FBaseEvent(Other), Payload(static_cast<UObject*>(Data)), m_Delegate(Other.m_Delegate)
	{
	}

	virtual ~FPayloadEvent()
	{
		m_Delegate.Unbind();
		Invalidate();
	}

	virtual void TriggerDelegates() override { if (IsValid()) m_Delegate.ExecuteIfBound(Payload.Get()); }
	UObject* GetPayload() const { return Payload.Get(); }
protected:
	TWeakObjectPtr<UObject> Payload;
	FPayloadEventDelegate m_Delegate;
};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
//		FStringEvent
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //

struct FStringEvent : public FBaseEvent
{
public:
	explicit FStringEvent(UObject* ContextObject, UEventIdentifier* EventIdentifier, FString EventIdentifierName, const FStringEventDelegate& Delegate)
		: FBaseEvent(ContextObject, EventIdentifier, EventIdentifierName, EEventType::String), m_Delegate(Delegate)
	{
	}

	explicit FStringEvent(const FStringEvent& Other, void* Data)
		: FBaseEvent(Other), String(*static_cast<FString*>(Data)), m_Delegate(Other.m_Delegate)
	{
	}

	virtual ~FStringEvent()
	{
		m_Delegate.Unbind();
		Invalidate();
	}

	virtual void TriggerDelegates() override { if (IsValid()) m_Delegate.ExecuteIfBound(String); }
	FString GetString() const { return String; }
protected:
	FString String;
	FStringEventDelegate m_Delegate;
};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
//		FIntEvent
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //

struct FIntEvent : public FBaseEvent
{
public:
	explicit FIntEvent(UObject* ContextObject, UEventIdentifier* EventIdentifier, FString EventIdentifierName, const FIntEventDelegate& Delegate)
		: FBaseEvent(ContextObject, EventIdentifier, EventIdentifierName, EEventType::Int), m_Delegate(Delegate)
	{
	}

	explicit FIntEvent(const FIntEvent& Other, void* Data)
		: FBaseEvent(Other), m_Value(*static_cast<int32*>(Data)), m_Delegate(Other.m_Delegate)
	{
	}

	virtual ~FIntEvent()
	{
		m_Delegate.Unbind();
		Invalidate();
	}

	virtual void TriggerDelegates() override { if (IsValid()) m_Delegate.ExecuteIfBound(m_Value); }
	int32 GetValue() const { return m_Value; }
protected:
	int32 m_Value;
	FIntEventDelegate m_Delegate;
};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
//		FFloatEvent
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //

struct FFloatEvent : public FBaseEvent
{
public:
	explicit FFloatEvent(UObject* ContextObject, UEventIdentifier* EventIdentifier, FString EventIdentifierName, const FFloatEventDelegate& Delegate)
		: FBaseEvent(ContextObject, EventIdentifier, EventIdentifierName, EEventType::Float), m_Delegate(Delegate)
	{
	}

	explicit FFloatEvent(const FFloatEvent& Other, void* Data)
		: FBaseEvent(Other), m_Value(*static_cast<float*>(Data)), m_Delegate(Other.m_Delegate)
	{
	}

	virtual ~FFloatEvent()
	{
		m_Delegate.Unbind();
		Invalidate();
	}

	virtual void TriggerDelegates() override { if (IsValid()) m_Delegate.ExecuteIfBound(m_Value); }
	float GetValue() const { return m_Value; }
protected:
	float m_Value;
	FFloatEventDelegate m_Delegate;
};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
//		FBoolEvent
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //

struct FBoolEvent : public FBaseEvent
{
public:
	explicit FBoolEvent(UObject* ContextObject, UEventIdentifier* EventIdentifier, FString EventIdentifierName, const FBoolEventDelegate& Delegate)
		: FBaseEvent(ContextObject, EventIdentifier, EventIdentifierName, EEventType::Bool), m_Delegate(Delegate)
	{
	}

	explicit FBoolEvent(const FBoolEvent& Other, void* Data)
		: FBaseEvent(Other), m_Value(*static_cast<bool*>(Data)), m_Delegate(Other.m_Delegate)
	{
	}

	virtual ~FBoolEvent()
	{
		m_Delegate.Unbind();
		Invalidate();
	}

	virtual void TriggerDelegates() override { if (IsValid()) m_Delegate.ExecuteIfBound(m_Value); }
	float GetValue() const { return m_Value; }
protected:
	bool m_Value;
	FBoolEventDelegate m_Delegate;
};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
//		FStringFloatEvent
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //

struct FStringFloatEvent : public FBaseEvent
{
public:
	explicit FStringFloatEvent(UObject* ContextObject, UEventIdentifier* EventIdentifier, FString EventIdentifierName, const FStringFloatEventDelegate& Delegate)
		: FBaseEvent(ContextObject, EventIdentifier, EventIdentifierName, EEventType::StringFloat), m_Delegate(Delegate)
	{
	}

	explicit FStringFloatEvent(const FStringFloatEvent& Other, void* Data)
		: FBaseEvent(Other), m_Data(*static_cast<FStringFloat*>(Data)), m_Delegate(Other.m_Delegate)
	{
	}

	virtual ~FStringFloatEvent()
	{
		m_Delegate.Unbind();
		Invalidate();
	}

	virtual void TriggerDelegates() override { if (IsValid()) m_Delegate.ExecuteIfBound(m_Data); }
	FStringFloat& GetValue() { return m_Data; }
protected:
	FStringFloat m_Data;
	FStringFloatEventDelegate m_Delegate;
};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
//		FStringIntEvent
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
struct FStringIntEvent : public FBaseEvent
{
public:
	explicit FStringIntEvent(UObject* ContextObject, UEventIdentifier* EventIdentifier, FString EventIdentifierName, const FStringIntEventDelegate& Delegate)
		: FBaseEvent(ContextObject, EventIdentifier, EventIdentifierName, EEventType::StringInt), m_Delegate(Delegate)
	{
	}

	explicit FStringIntEvent(const FStringIntEvent& Other, void* Data)
		: FBaseEvent(Other), m_Data(*static_cast<FStringInt*>(Data)), m_Delegate(Other.m_Delegate)
	{
	}

	virtual ~FStringIntEvent()
	{
		m_Delegate.Unbind();
		Invalidate();
	}

	virtual void TriggerDelegates() override { if (IsValid()) m_Delegate.ExecuteIfBound(m_Data); }
	FStringInt& GetValue() { return m_Data; }
protected:
	FStringInt m_Data;
	FStringIntEventDelegate m_Delegate;
};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
//		FVectorEvent
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
struct FVectorEvent : public FBaseEvent
{
public:
	explicit FVectorEvent(UObject* ContextObject, UEventIdentifier* EventIdentifier, FString EventIdentifierName, const FVectorEventDelegate& Delegate)
		: FBaseEvent(ContextObject, EventIdentifier, EventIdentifierName, EEventType::Vector), m_Delegate(Delegate)
	{
	}

	explicit FVectorEvent(const FVectorEvent& Other, void* Data)
		: FBaseEvent(Other), m_Data(*static_cast<FVector*>(Data)), m_Delegate(Other.m_Delegate)
	{
	}

	virtual ~FVectorEvent()
	{
		m_Delegate.Unbind();
		Invalidate();
	}

	virtual void TriggerDelegates() override { if (IsValid()) m_Delegate.ExecuteIfBound(m_Data); }
	FVector& GetValue() { return m_Data; }
protected:
	FVector m_Data;
	FVectorEventDelegate m_Delegate;
};

static FORCEINLINE FString GetDispatcherTypeAsString(EEventType Type)
{
	switch (Type)
	{
	case EEventType::Simple:
		return "Simple";
	case EEventType::Payload:
		return "Payload";
	case EEventType::String:
		return "String";
	case EEventType::Bool:
		return "Bool";
	case EEventType::Float:
		return "Float";
	case EEventType::Int:
		return "Int";
	case EEventType::StringFloat:
		return "StringFloat";
	case EEventType::StringInt:
		return "StringInt";
	case EEventType::Vector:
		return "Vector";
	}

	return "";
}
