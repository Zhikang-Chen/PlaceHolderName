// Copyright 2023 Alexander Floden, Alias Alex River. All Rights Reserved. 

#include "RapidJsonFunctionLibrary.h"
#include "../ThirdParty/rapidjson/document.h"
#include "../ThirdParty/rapidjson/writer.h"
#include "../ThirdParty/rapidjson/stringbuffer.h"
#include "RapidJsonSerializer.h"
#include "RapidJsonDeserializer.h"

// For Custom Structs
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"


//------- CUSTOM STRUCTS --------//

bool URapidJsonFunctionLibrary::SerializeCustomStruct(const FCustomStructSerialization& CustomStruct, FString& OutJsonString)
{
    TSharedPtr<FJsonObject> JsonObject = CustomStruct.ToJson();
    if (!JsonObject.IsValid())
    {
        return false;
    }

    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutJsonString);
    FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

    return true;
}

bool URapidJsonFunctionLibrary::DeserializeCustomStruct(const FString& InJsonString, FCustomStructSerialization& OutCustomStruct)
{
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(InJsonString);

    TSharedPtr<FJsonObject> JsonObject;
    if (!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid())
    {
        return false;
    }

    return OutCustomStruct.FromJson(JsonObject);
}


//------- SERIALIZATION --------//

FString URapidJsonFunctionLibrary::SerializeInt_Wrapper(int32 IntValue)
{
    rapidjson::Document document;
    rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
    rapidjson::Value root;
    SerializeInt(root, IntValue, allocator);
    document.Swap(root);

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    document.Accept(writer);
    return FString(buffer.GetString());
}

FString URapidJsonFunctionLibrary::SerializeUInt8_Wrapper(uint8 UInt8Value)
{
    rapidjson::Document document;
    rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
    rapidjson::Value root;
    SerializeUInt8(root, UInt8Value, allocator);
    document.Swap(root);

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    document.Accept(writer);
    return FString(buffer.GetString());
}

FString URapidJsonFunctionLibrary::SerializeInt64_Wrapper(int64 Int64Value)
{
    rapidjson::Document document;
    rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
    rapidjson::Value root;
    SerializeInt64(root, Int64Value, allocator);
    document.Swap(root);

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    document.Accept(writer);
    return FString(buffer.GetString());
}

FString URapidJsonFunctionLibrary::SerializeFString_Wrapper(FString StringValue)
{
    rapidjson::Document document;
    rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
    rapidjson::Value root;
    SerializeFString(root, StringValue, allocator);
    document.Swap(root);

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    document.Accept(writer);
    return FString(buffer.GetString());
}

FString URapidJsonFunctionLibrary::SerializeFColor_Wrapper(FColor ColorValue)
{
    rapidjson::Document document;
    rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
    rapidjson::Value root;
    SerializeFColor(root, ColorValue, allocator);
    document.Swap(root);

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    document.Accept(writer);
    return FString(buffer.GetString());
}

FString URapidJsonFunctionLibrary::SerializeBool_Wrapper(bool BoolValue)
{
    rapidjson::Document document;
    rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
    rapidjson::Value root;
    SerializeBool(root, BoolValue, allocator);
    document.Swap(root);

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    document.Accept(writer);
    return FString(buffer.GetString());
}

FString URapidJsonFunctionLibrary::SerializeFName_Wrapper(FName NameValue)
{
    rapidjson::Document document;
    rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
    rapidjson::Value root;
    SerializeFName(root, NameValue, allocator);
    document.Swap(root);

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    document.Accept(writer);
    return FString(buffer.GetString());
}

FString URapidJsonFunctionLibrary::SerializeFText_Wrapper(FText TextValue)
{
    rapidjson::Document document;
    rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
    rapidjson::Value root;
    SerializeFText(root, TextValue, allocator);
    document.Swap(root);

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    document.Accept(writer);
    return FString(buffer.GetString());
}

FString URapidJsonFunctionLibrary::SerializeFloat_Wrapper(float FloatValue)
{
    rapidjson::Document document;
    rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
    rapidjson::Value root;
    SerializeFloat(root, FloatValue, allocator);
    document.Swap(root);

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    document.Accept(writer);
    return FString(buffer.GetString());
}

FString URapidJsonFunctionLibrary::SerializeFVector_Wrapper(FVector VectorValue)
{
    rapidjson::Document document;
    rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
    rapidjson::Value root;
    SerializeFVector(root, VectorValue, allocator);
    document.Swap(root);

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    document.Accept(writer);
    return FString(buffer.GetString());
}

FString URapidJsonFunctionLibrary::SerializeFRotator_Wrapper(FRotator RotatorValue)
{
    rapidjson::Document document;
    rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
    rapidjson::Value root;
    SerializeFRotator(root, RotatorValue, allocator);
    document.Swap(root);

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    document.Accept(writer);
    return FString(buffer.GetString());
}

FString URapidJsonFunctionLibrary::SerializeFTransform_Wrapper(FTransform TransformValue)
{
    rapidjson::Document document;
    rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
    rapidjson::Value root;
    SerializeFTransform(root, TransformValue, allocator);
    document.Swap(root);

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    document.Accept(writer);
    return FString(buffer.GetString());
}


//------- SERIALIZATION ARRAYS --------//

FString URapidJsonFunctionLibrary::SerializeTArrayFColor_Wrapper(const TArray<FColor>& Colors)
{
    rapidjson::Document document;
    rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
    rapidjson::Value root(rapidjson::kArrayType);
    SerializeTArrayFColor(root, Colors, allocator);
    document.Swap(root);

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    document.Accept(writer);
    return FString(buffer.GetString());
}

FString URapidJsonFunctionLibrary::SerializeTArrayInt_Wrapper(const TArray<int32>& InInts)
{
    rapidjson::Document document;
    rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
    rapidjson::Value root(rapidjson::kArrayType);
    SerializeTArrayInt(root, InInts, allocator);
    document.Swap(root);

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    document.Accept(writer);
    return FString(buffer.GetString());
}

FString URapidJsonFunctionLibrary::SerializeTArrayUInt8_Wrapper(const TArray<uint8>& InUInt8s)
{
    rapidjson::Document document;
    rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
    rapidjson::Value root(rapidjson::kArrayType);
    SerializeTArrayUInt8(root, InUInt8s, allocator);
    document.Swap(root);

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    document.Accept(writer);
    return FString(buffer.GetString());
}

FString URapidJsonFunctionLibrary::SerializeTArrayInt64_Wrapper(const TArray<int64>& InInt64s)
{
    rapidjson::Document document;
    rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
    rapidjson::Value root(rapidjson::kArrayType);
    SerializeTArrayInt64(root, InInt64s, allocator);
    document.Swap(root);

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    document.Accept(writer);
    return FString(buffer.GetString());
}

FString URapidJsonFunctionLibrary::SerializeTArrayFString_Wrapper(const TArray<FString>& InStrings)
{
    rapidjson::Document document;
    rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
    rapidjson::Value root(rapidjson::kArrayType);
    SerializeTArrayFString(root, InStrings, allocator);
    document.Swap(root);

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    document.Accept(writer);
    return FString(buffer.GetString());
}

FString URapidJsonFunctionLibrary::SerializeTArrayBool_Wrapper(const TArray<bool>& InBools)
{
    rapidjson::Document document;
    rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
    rapidjson::Value root(rapidjson::kArrayType);
    SerializeTArrayBool(root, InBools, allocator);
    document.Swap(root);

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    document.Accept(writer);
    return FString(buffer.GetString());
}

FString URapidJsonFunctionLibrary::SerializeTArrayFName_Wrapper(const TArray<FName>& InNames)
{
    rapidjson::Document document;
    rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
    rapidjson::Value root(rapidjson::kArrayType);
    SerializeTArrayFName(root, InNames, allocator);
    document.Swap(root);

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    document.Accept(writer);
    return FString(buffer.GetString());
}

FString URapidJsonFunctionLibrary::SerializeTArrayFText_Wrapper(const TArray<FText>& InTexts)
{
    rapidjson::Document document;
    rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
    rapidjson::Value root(rapidjson::kArrayType);
    SerializeTArrayFText(root, InTexts, allocator);
    document.Swap(root);

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    document.Accept(writer);
    return FString(buffer.GetString());
}

FString URapidJsonFunctionLibrary::SerializeTArrayFloat_Wrapper(const TArray<float>& inFloats)
{
    rapidjson::Document document;
    rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
    rapidjson::Value root(rapidjson::kArrayType);
    SerializeTArrayFloat(root, inFloats, allocator);
    document.Swap(root);

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    document.Accept(writer);
    return FString(buffer.GetString());
}

FString URapidJsonFunctionLibrary::SerializeTArrayFVector_Wrapper(const TArray<FVector>& inFVector)
{
    rapidjson::Document document;
    rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
    rapidjson::Value root(rapidjson::kArrayType);
    SerializeTArrayFVector(root, inFVector, allocator);
    document.Swap(root);

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    document.Accept(writer);
    return FString(buffer.GetString());
}

FString URapidJsonFunctionLibrary::SerializeTArrayFRotator_Wrapper(const TArray<FRotator>& inRotator)
{
    rapidjson::Document document;
    rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
    rapidjson::Value root(rapidjson::kArrayType);
    SerializeTArrayFRotator(root, inRotator, allocator);
    document.Swap(root);

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    document.Accept(writer);
    return FString(buffer.GetString());
}

FString URapidJsonFunctionLibrary::SerializeTArrayFTransform_Wrapper(const TArray<FTransform>& inTransform)
{
    rapidjson::Document document;
    rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
    rapidjson::Value root(rapidjson::kArrayType);
    SerializeTArrayFTransform(root, inTransform, allocator);
    document.Swap(root);

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    document.Accept(writer);
    return FString(buffer.GetString());
}


//------- DE-SERIALIZATION --------//

int32 URapidJsonFunctionLibrary::DeserializeInt_Wrapper(const FString& JsonString)
{
    rapidjson::Document deserializedDocument;
    deserializedDocument.Parse(TCHAR_TO_UTF8(*JsonString));
    return DeserializeInt(deserializedDocument);
}

uint8 URapidJsonFunctionLibrary::DeserializeUInt8_Wrapper(const FString& JsonString)
{
    rapidjson::Document deserializedDocument;
    deserializedDocument.Parse(TCHAR_TO_UTF8(*JsonString));
    return DeserializeUInt8(deserializedDocument);
}

int64 URapidJsonFunctionLibrary::DeserializeInt64_Wrapper(const FString& JsonString)
{
    rapidjson::Document deserializedDocument;
    deserializedDocument.Parse(TCHAR_TO_UTF8(*JsonString));
    return DeserializeInt64(deserializedDocument);
}

FString URapidJsonFunctionLibrary::DeserializeFString_Wrapper(const FString& JsonString)
{
    rapidjson::Document deserializedDocument;
    deserializedDocument.Parse(TCHAR_TO_UTF8(*JsonString));
    return DeserializeFString(deserializedDocument);
}

FColor URapidJsonFunctionLibrary::DeserializeFColor_Wrapper(const FString& JsonString)
{
    rapidjson::Document deserializedDocument;
    deserializedDocument.Parse(TCHAR_TO_UTF8(*JsonString));
    return DeserializeFColor(deserializedDocument);
}

bool URapidJsonFunctionLibrary::DeserializeBool_Wrapper(const FString& JsonString)
{
    rapidjson::Document deserializedDocument;
    deserializedDocument.Parse(TCHAR_TO_UTF8(*JsonString));
    return DeserializeBool(deserializedDocument);
}

FName URapidJsonFunctionLibrary::DeserializeFName_Wrapper(const FString& JsonString)
{
    rapidjson::Document deserializedDocument;
    deserializedDocument.Parse(TCHAR_TO_UTF8(*JsonString));
    return DeserializeFName(deserializedDocument);
}

FText URapidJsonFunctionLibrary::DeserializeFText_Wrapper(const FString& JsonString)
{
    rapidjson::Document deserializedDocument;
    deserializedDocument.Parse(TCHAR_TO_UTF8(*JsonString));
    return DeserializeFText(deserializedDocument);
}

float URapidJsonFunctionLibrary::DeserializeFloat_Wrapper(const FString& JsonString)
{
    rapidjson::Document deserializedDocument;
    deserializedDocument.Parse(TCHAR_TO_UTF8(*JsonString));
    return DeserializeFloat(deserializedDocument);
}

FVector URapidJsonFunctionLibrary::DeserializeFVector_Wrapper(const FString& JsonString)
{
    rapidjson::Document deserializedDocument;
    deserializedDocument.Parse(TCHAR_TO_UTF8(*JsonString));
    return DeserializeFVector(deserializedDocument);
}

FRotator URapidJsonFunctionLibrary::DeserializeFRotator_Wrapper(const FString& JsonString)
{
    rapidjson::Document deserializedDocument;
    deserializedDocument.Parse(TCHAR_TO_UTF8(*JsonString));
    return DeserializeFRotator(deserializedDocument);
}

FTransform URapidJsonFunctionLibrary::DeserializeFTransform_Wrapper(const FString& JsonString)
{
    rapidjson::Document deserializedDocument;
    deserializedDocument.Parse(TCHAR_TO_UTF8(*JsonString));
    return DeserializeFTransform(deserializedDocument);
}


//------- DE-SERIALIZATION ARRAYS --------//

TArray<FColor> URapidJsonFunctionLibrary::DeserializeTArrayFColor_Wrapper(const FString& JsonString)
{
    rapidjson::Document deserializedDocument;
    deserializedDocument.Parse(TCHAR_TO_UTF8(*JsonString));
    return DeserializeTArrayFColor(deserializedDocument);
}

TArray<int32> URapidJsonFunctionLibrary::DeserializeTArrayInt_Wrapper(const FString& JsonString)
{
    rapidjson::Document deserializedDocument;
    deserializedDocument.Parse(TCHAR_TO_UTF8(*JsonString));
    return DeserializeTArrayInt(deserializedDocument);
}

TArray<uint8> URapidJsonFunctionLibrary::DeserializeTArrayUInt8_Wrapper(const FString& JsonString)
{
    rapidjson::Document deserializedDocument;
    deserializedDocument.Parse(TCHAR_TO_UTF8(*JsonString));
    return DeserializeTArrayUInt8(deserializedDocument);
}

TArray<int64> URapidJsonFunctionLibrary::DeserializeTArrayInt64_Wrapper(const FString& JsonString)
{
    rapidjson::Document deserializedDocument;
    deserializedDocument.Parse(TCHAR_TO_UTF8(*JsonString));
    return DeserializeTArrayInt64(deserializedDocument);
}

TArray<FString> URapidJsonFunctionLibrary::DeserializeTArrayFString_Wrapper(const FString& JsonString)
{
    rapidjson::Document deserializedDocument;
    deserializedDocument.Parse(TCHAR_TO_UTF8(*JsonString));
    return DeserializeTArrayFString(deserializedDocument);
}

TArray<bool> URapidJsonFunctionLibrary::DeserializeTArrayBool_Wrapper(const FString& JsonString)
{
    rapidjson::Document deserializedDocument;
    deserializedDocument.Parse(TCHAR_TO_UTF8(*JsonString));
    return DeserializeTArrayBool(deserializedDocument);
}

TArray<FName> URapidJsonFunctionLibrary::DeserializeTArrayFName_Wrapper(const FString& JsonString)
{
    rapidjson::Document deserializedDocument;
    deserializedDocument.Parse(TCHAR_TO_UTF8(*JsonString));
    return DeserializeTArrayFName(deserializedDocument);
}

TArray<FText> URapidJsonFunctionLibrary::DeserializeTArrayFText_Wrapper(const FString& JsonString)
{
    rapidjson::Document deserializedDocument;
    deserializedDocument.Parse(TCHAR_TO_UTF8(*JsonString));
    return DeserializeTArrayFText(deserializedDocument);
}

TArray<float> URapidJsonFunctionLibrary::DeserializeTArrayFloat_Wrapper(const FString& JsonString)
{
    rapidjson::Document deserializedDocument;
    deserializedDocument.Parse(TCHAR_TO_UTF8(*JsonString));
    return DeserializeTArrayFloat(deserializedDocument);
}

TArray<FVector> URapidJsonFunctionLibrary::DeserializeTArrayFVector_Wrapper(const FString& JsonString)
{
    rapidjson::Document deserializedDocument;
    deserializedDocument.Parse(TCHAR_TO_UTF8(*JsonString));
    return DeserializeTArrayFVector(deserializedDocument);
}

TArray<FRotator> URapidJsonFunctionLibrary::DeserializeTArrayFRotator_Wrapper(const FString& JsonString)
{
    rapidjson::Document deserializedDocument;
    deserializedDocument.Parse(TCHAR_TO_UTF8(*JsonString));
    return DeserializeTArrayFRotator(deserializedDocument);
}

TArray<FTransform> URapidJsonFunctionLibrary::DeserializeTArrayFTransform_Wrapper(const FString& JsonString)
{
    rapidjson::Document deserializedDocument;
    deserializedDocument.Parse(TCHAR_TO_UTF8(*JsonString));
    return DeserializeTArrayFTransform(deserializedDocument);
}
