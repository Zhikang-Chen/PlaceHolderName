// Copyright 2023 Alexander Floden, Alias Alex River. All Rights Reserved. 

#include "CustomStructSerializationExample.h"


// Called when the game starts or when spawned
void ACustomStructSerializationExample::BeginPlay() {

	Super::BeginPlay();
	

	// Serializing

	myCustomStruct.Name = "TestString";
	myCustomStruct.Age = 10;

	if (URapidJsonFunctionLibrary::SerializeCustomStruct(myCustomStruct, myCustomStructSerialized)) {

		UE_LOG(LogTemp, Warning, TEXT("Successfull Serialization of myCustomStruct: %s"), *myCustomStructSerialized);
	}

	else {

		UE_LOG(LogTemp, Warning, TEXT("Serialization of myCustomStruct Failed"));
	}




	// De-Serializing

	FMyCustomStructSerializationExample deserializedMyCustomStruct;

	if (URapidJsonFunctionLibrary::DeserializeCustomStruct(myCustomStructSerialized, deserializedMyCustomStruct)) {

		UE_LOG(LogTemp, Warning, TEXT("Deserialization of myCustomStructSerialized: %s - age: %i "), *deserializedMyCustomStruct.Name, deserializedMyCustomStruct.Age);
	}
	else {

		UE_LOG(LogTemp, Warning, TEXT("De-Serialization of myCustomStructSerialized Failed"));
	}
}

