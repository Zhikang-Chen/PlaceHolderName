// Copyright 2022 Alexander Floden, Alias Alex River. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/EngineSubsystem.h"
#include "VertexPaintDetectionEngSubsystem.generated.h"


/**
 *
 */
UCLASS()
class VERTEXPAINTDETECTIONPLUGIN_API UVertexPaintDetectionEngSubsystem : public UEngineSubsystem {


	GENERATED_BODY()

public:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
};
