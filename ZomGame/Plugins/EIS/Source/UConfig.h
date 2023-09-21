// Copyright 2022 ZADROT LLC

#pragma once
#include "UConfig.generated.h"

UCLASS(Config = EditorSettings)
class UConfig : public UObject
{
	GENERATED_UCLASS_BODY()
	void UpdateScale();
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
	UPROPERTY(EditAnywhere, Config, Category = "Settings", meta = (DisplayName = "Scale", ClampMin = 0.5, ClampMax = 4.0))
	float Scale;
};