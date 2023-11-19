// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CPP_Grid.generated.h"

UCLASS()
class ZOMGAME_API ACPP_Grid : public AActor
{
	GENERATED_BODY()
public:

	UPROPERTY(EditDefaultsOnly)
	FIntVector GridSize;

	UPROPERTY(EditDefaultsOnly)
	FVector BoxSize;

	UPROPERTY(EditDefaultsOnly)
	bool DrawDebugGrid;

	UPROPERTY(VisibleAnywhere)
	TArray<FVector> Grid;

	UPROPERTY(VisibleAnywhere)
	FVector CenterOffSet;

	UPROPERTY(VisibleAnywhere)
	FVector MinBound;

	UPROPERTY(VisibleAnywhere)
	FVector MaxBound;


public:	
	// Sets default values for this actor's properties
	ACPP_Grid();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	const FIntVector ConvertWorldLocationToGridLocation(FVector worldLocation);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	const FVector ConvertGridLocationToWorldLocation(FIntVector gridLocation);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	const FVector GetGridWorldLocation(FIntVector gridLocation);
};
