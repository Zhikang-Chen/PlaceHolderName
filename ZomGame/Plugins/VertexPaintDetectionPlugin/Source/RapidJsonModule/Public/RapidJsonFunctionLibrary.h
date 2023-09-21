// Copyright 2023 Alexander Floden, Alias Alex River. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Dom/JsonObject.h"
#include "Templates/SharedPointer.h"
#include "RapidJsonFunctionLibrary.generated.h"


USTRUCT(BlueprintType)
struct FCustomStructSerialization
{
    GENERATED_BODY()

        virtual ~FCustomStructSerialization() = default;

    virtual TSharedPtr<FJsonObject> ToJson() const {

        TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
        return JsonObject;
    }

    virtual bool FromJson(const TSharedPtr<FJsonObject>& JsonObject) {
        return true;
    }
};


UCLASS()
class RAPIDJSONMODULE_API URapidJsonFunctionLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:

    //------- CUSTOM STRUCTS --------//

    static bool SerializeCustomStruct(const FCustomStructSerialization& CustomStruct, FString& OutJsonString);

    static bool DeserializeCustomStruct(const FString& InJsonString, FCustomStructSerialization& OutCustomStruct);


    // Appended with _Wrapper to differ them from the functions in Serializer / DeSerializer

    //------- SERIALIZE --------//

    UFUNCTION(BlueprintCallable, Category = "RapidJson|Serialize", meta = (DisplayName = "Serialize Int"))
        static FString SerializeInt_Wrapper(int32 IntValue);

    UFUNCTION(BlueprintCallable, Category = "RapidJson|Serialize", meta = (DisplayName = "Serialize Byte"))
        static FString SerializeUInt8_Wrapper(uint8 UInt8Value);

    UFUNCTION(BlueprintCallable, Category = "RapidJson|Serialize", meta = (DisplayName = "Serialize Int64"))
        static FString SerializeInt64_Wrapper(int64 Int64Value);

    UFUNCTION(BlueprintCallable, Category = "RapidJson|Serialize", meta = (DisplayName = "Serialize String"))
        static FString SerializeFString_Wrapper(FString StringValue);

    UFUNCTION(BlueprintCallable, Category = "RapidJson|Serialize", meta = (DisplayName = "Serialize Color"))
        static FString SerializeFColor_Wrapper(FColor ColorValue);

    UFUNCTION(BlueprintCallable, Category = "RapidJson|Serialize", meta = (DisplayName = "Serialize Bool"))
        static FString SerializeBool_Wrapper(bool BoolValue);

    UFUNCTION(BlueprintCallable, Category = "RapidJson|Serialize", meta = (DisplayName = "Serialize Name"))
        static FString SerializeFName_Wrapper(FName NameValue);

    UFUNCTION(BlueprintCallable, Category = "RapidJson|Serialize", meta = (DisplayName = "Serialize Text"))
        static FString SerializeFText_Wrapper(FText TextValue);

    UFUNCTION(BlueprintCallable, Category = "RapidJson|Serialize", meta = (DisplayName = "Serialize Float"))
        static FString SerializeFloat_Wrapper(float FloatValue);

    UFUNCTION(BlueprintCallable, Category = "RapidJson|Serialize", meta = (DisplayName = "Serialize Vector"))
        static FString SerializeFVector_Wrapper(FVector VectorValue);

    UFUNCTION(BlueprintCallable, Category = "RapidJson|Serialize", meta = (DisplayName = "Serialize Rotator"))
        static FString SerializeFRotator_Wrapper(FRotator RotatorValue);

    UFUNCTION(BlueprintCallable, Category = "RapidJson|Serialize", meta = (DisplayName = "Serialize Transform"))
        static FString SerializeFTransform_Wrapper(FTransform TransformValue);


    //------- De-SERIALIZE --------//

    UFUNCTION(BlueprintCallable, Category = "RapidJson|De-Serialize", meta = (DisplayName = "De-Serialize Int"))
        static int32 DeserializeInt_Wrapper(const FString& JsonString);

    UFUNCTION(BlueprintCallable, Category = "RapidJson|De-Serialize", meta = (DisplayName = "De-Serialize Byte"))
        static uint8 DeserializeUInt8_Wrapper(const FString& JsonString);

    UFUNCTION(BlueprintCallable, Category = "RapidJson|De-Serialize", meta = (DisplayName = "De-Serialize Int64"))
        static int64 DeserializeInt64_Wrapper(const FString& JsonString);

    UFUNCTION(BlueprintCallable, Category = "RapidJson|De-Serialize", meta = (DisplayName = "De-Serialize String"))
        static FString DeserializeFString_Wrapper(const FString& JsonString);

    UFUNCTION(BlueprintCallable, Category = "RapidJson|De-Serialize", meta = (DisplayName = "De-Serialize Color"))
        static FColor DeserializeFColor_Wrapper(const FString& JsonString);

    UFUNCTION(BlueprintCallable, Category = "RapidJson|De-Serialize", meta = (DisplayName = "De-Serialize Bool"))
        static bool DeserializeBool_Wrapper(const FString& JsonString);

    UFUNCTION(BlueprintCallable, Category = "RapidJson|De-Serialize", meta = (DisplayName = "De-Serialize Name"))
        static FName DeserializeFName_Wrapper(const FString& JsonString);

    UFUNCTION(BlueprintCallable, Category = "RapidJson|De-Serialize", meta = (DisplayName = "De-Serialize Text"))
        static FText DeserializeFText_Wrapper(const FString& JsonString);

    UFUNCTION(BlueprintCallable, Category = "RapidJson|De-Serialize", meta = (DisplayName = "De-Serialize Float"))
        static float DeserializeFloat_Wrapper(const FString& JsonString);

    UFUNCTION(BlueprintCallable, Category = "RapidJson|De-Serialize", meta = (DisplayName = "De-Serialize Vector"))
        static FVector DeserializeFVector_Wrapper(const FString& JsonString);

    UFUNCTION(BlueprintCallable, Category = "RapidJson|De-Serialize", meta = (DisplayName = "De-Serialize Rotator"))
        static FRotator DeserializeFRotator_Wrapper(const FString& JsonString);

    UFUNCTION(BlueprintCallable, Category = "RapidJson|De-Serialize", meta = (DisplayName = "De-Serialize Transform"))
        static FTransform DeserializeFTransform_Wrapper(const FString& JsonString);


    //------- SERIALIZE ARRAYS --------//

    UFUNCTION(BlueprintCallable, Category = "RapidJson|Serialize Array", meta = (DisplayName = "Serialize Int Array"))
        static FString SerializeTArrayInt_Wrapper(const TArray<int32>& InInts);

    UFUNCTION(BlueprintCallable, Category = "RapidJson|Serialize Array", meta = (DisplayName = "Serialize Byte Array"))
        static FString SerializeTArrayUInt8_Wrapper(const TArray<uint8>& InUInt8s);

    UFUNCTION(BlueprintCallable, Category = "RapidJson|Serialize Array", meta = (DisplayName = "Serialize Int64 Array"))
        static FString SerializeTArrayInt64_Wrapper(const TArray<int64>& InInt64s);

    UFUNCTION(BlueprintCallable, Category = "RapidJson|Serialize Array", meta = (DisplayName = "Serialize String Array"))
        static FString SerializeTArrayFString_Wrapper(const TArray<FString>& InStrings);

    UFUNCTION(BlueprintCallable, Category = "RapidJson|Serialize Array", meta = (DisplayName = "Serialize Color Array"))
        static FString SerializeTArrayFColor_Wrapper(const TArray<FColor>& Colors);

    UFUNCTION(BlueprintCallable, Category = "RapidJson|Serialize Array", meta = (DisplayName = "Serialize Bool Array"))
        static FString SerializeTArrayBool_Wrapper(const TArray<bool>& InBools);

    UFUNCTION(BlueprintCallable, Category = "RapidJson|Serialize Array", meta = (DisplayName = "Serialize Name Array"))
        static FString SerializeTArrayFName_Wrapper(const TArray<FName>& InNames);

    UFUNCTION(BlueprintCallable, Category = "RapidJson|Serialize Array", meta = (DisplayName = "Serialize Text Array"))
        static FString SerializeTArrayFText_Wrapper(const TArray<FText>& InTexts);

    UFUNCTION(BlueprintCallable, Category = "RapidJson|Serialize Array", meta = (DisplayName = "Serialize Float Array"))
        static FString SerializeTArrayFloat_Wrapper(const TArray<float>& inFloats);

    UFUNCTION(BlueprintCallable, Category = "RapidJson|Serialize Array", meta = (DisplayName = "Serialize Vector Array"))
        static FString SerializeTArrayFVector_Wrapper(const TArray<FVector>& inFVector);

    UFUNCTION(BlueprintCallable, Category = "RapidJson|Serialize Array", meta = (DisplayName = "Serialize Rotator Array"))
        static FString SerializeTArrayFRotator_Wrapper(const TArray<FRotator>& inRotator);

    UFUNCTION(BlueprintCallable, Category = "RapidJson|Serialize Array", meta = (DisplayName = "Serialize Transform Array"))
        static FString SerializeTArrayFTransform_Wrapper(const TArray<FTransform>& inTransform);



    //------- DE-SERIALIZE ARRAYS --------//

    UFUNCTION(BlueprintCallable, Category = "RapidJson|De-Serialize Array", meta = (DisplayName = "De-Serialize Int Array"))
        static TArray<int32> DeserializeTArrayInt_Wrapper(const FString& JsonString);

    UFUNCTION(BlueprintCallable, Category = "RapidJson|De-Serialize Array", meta = (DisplayName = "De-Serialize Byte Array"))
        static TArray<uint8> DeserializeTArrayUInt8_Wrapper(const FString& JsonString);

    UFUNCTION(BlueprintCallable, Category = "RapidJson|De-Serialize Array", meta = (DisplayName = "De-Serialize Int64 Array"))
        static TArray<int64> DeserializeTArrayInt64_Wrapper(const FString& JsonString);

    UFUNCTION(BlueprintCallable, Category = "RapidJson|De-Serialize Array", meta = (DisplayName = "De-Serialize String Array"))
        static TArray<FString> DeserializeTArrayFString_Wrapper(const FString& JsonString);

    UFUNCTION(BlueprintCallable, Category = "RapidJson|De-Serialize Array", meta = (DisplayName = "De-Serialize Color Array"))
        static TArray<FColor> DeserializeTArrayFColor_Wrapper(const FString& JsonString);

    UFUNCTION(BlueprintCallable, Category = "RapidJson|De-Serialize Array", meta = (DisplayName = "De-Serialize Bool Array"))
        static TArray<bool> DeserializeTArrayBool_Wrapper(const FString& JsonString);

    UFUNCTION(BlueprintCallable, Category = "RapidJson|De-Serialize Array", meta = (DisplayName = "De-Serialize Name Array"))
        static TArray<FName> DeserializeTArrayFName_Wrapper(const FString& JsonString);

    UFUNCTION(BlueprintCallable, Category = "RapidJson|De-Serialize Array", meta = (DisplayName = "De-Serialize Text Array"))
        static TArray<FText> DeserializeTArrayFText_Wrapper(const FString& JsonString);

    UFUNCTION(BlueprintCallable, Category = "RapidJson|De-Serialize Array", meta = (DisplayName = "De-Serialize Float Array"))
        static TArray<float> DeserializeTArrayFloat_Wrapper(const FString& JsonString);

    UFUNCTION(BlueprintCallable, Category = "RapidJson|De-Serialize Array", meta = (DisplayName = "De-Serialize Vector Array"))
        static TArray<FVector> DeserializeTArrayFVector_Wrapper(const FString& JsonString);

    UFUNCTION(BlueprintCallable, Category = "RapidJson|De-Serialize Array", meta = (DisplayName = "De-Serialize Rotator Array"))
        static TArray<FRotator> DeserializeTArrayFRotator_Wrapper(const FString& JsonString);

    UFUNCTION(BlueprintCallable, Category = "RapidJson|De-Serialize Array", meta = (DisplayName = "De-Serialize Transform Array"))
        static TArray<FTransform> DeserializeTArrayFTransform_Wrapper(const FString& JsonString);

};
