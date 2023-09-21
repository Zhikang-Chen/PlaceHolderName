/**
* Copyright (C) 2017-2020 | Dry Eel Development
*
* Official EventDispatcher Documentation: https://eeldev.com/index.php/eventdispatcher-plugin/
*/

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "EventDispatcherSettings.generated.h"

UCLASS(config = Engine, defaultconfig, meta = (DisplayName = "EventDispatcher Plugin"))
class EVENTDISPATCHER_API UEventDispatcherSettings : public UDeveloperSettings
{
	GENERATED_BODY()
public:
	UEventDispatcherSettings();
public:
	/**
	* All invokes are put in a queue and this setting determine how
	* frequently we check for new delegates to execute (in seconds)
	*/
	UPROPERTY(config, EditAnywhere, Category = "Settings")
	float PollingRate;

private:
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual FName GetCategoryName() const override;
#endif
};
