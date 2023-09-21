// Copyright 2022 Alexander Floden, Alias Alex River. All Rights Reserved. 


#include "VertexPaintDetectionSettings.h"

//-------------------------------------------------------

// Construct

UVertexPaintDetectionSettings::UVertexPaintDetectionSettings() {

	UE_LOG(LogTemp, Log, TEXT("Vertex Paint Detection Settings Construct"));

#if WITH_EDITOR

	// Binds so we get a callback to whenever a settings has been changed
	SettingsChangedDelegate.AddUObject(this, &UVertexPaintDetectionSettings::VertexPaintDetectionSettingsChanged);

#endif
}

#if WITH_EDITOR

//-------------------------------------------------------

// Vertex Paint Detection Settings Changed

void UVertexPaintDetectionSettings::VertexPaintDetectionSettingsChanged(UObject* Settings, FPropertyChangedEvent& PropertyChangedEvent) {


	UE_LOG(LogTemp, Log, TEXT("Vertex Paint Detection Settings Changed"));
}

#endif