#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TowerStat.generated.h"

USTRUCT(BluePrintType)
struct FTowerStats
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float Health = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float AttackSpeed = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float Damage = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float TargetingRange = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float DeployTime = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float Pierce = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float Aggro = 0;
};