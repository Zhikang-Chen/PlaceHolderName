// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_Grid.h"

// Sets default values
ACPP_Grid::ACPP_Grid()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	DrawDebugGrid = false;
	GridSize = FIntVector::NoneValue;
	BoxSize = FVector::One();
}

// Called when the game starts or when spawned
void ACPP_Grid::BeginPlay()
{
	Super::BeginPlay();
	RootComponent->SetMobility(EComponentMobility::Static);
	//GridSize.X * BoxSize.X;
	auto offSet = GetActorLocation();
	CenterOffSet = FVector(((float)(BoxSize.X * GridSize.X) / 2.0f), ((float)(BoxSize.Y * GridSize.Y) / 2.0f), ((float)(BoxSize.Z * GridSize.Z) / 2.0f));


	float xMinBound = offSet.X - (BoxSize.X / 2.f) - CenterOffSet.X;
	float yMinBound = offSet.Y - (BoxSize.Y / 2.f) - CenterOffSet.Y;
	float zMinBound = offSet.Z - (BoxSize.Z / 2.f) - CenterOffSet.Z;
	//(GridSize.Y * BoxSize.Y) +
	MinBound = FVector(xMinBound, yMinBound, zMinBound);

	float xMaxBound = offSet.X + (GridSize.X * BoxSize.X) - CenterOffSet.X;
	float yMaxBound = offSet.Y + (GridSize.Y * BoxSize.Y) - CenterOffSet.Y;
	float zMaxBound = offSet.Z + (GridSize.Z * BoxSize.Z) - CenterOffSet.Z;
	MaxBound = FVector(xMaxBound, yMaxBound, zMaxBound);

	for (int x = 0; x < GridSize.X; ++x)
	{
		auto xAxis = offSet.X + (BoxSize.X * x) - CenterOffSet.X;
		for (int y = 0; y < GridSize.Y; ++y)
		{
			auto yAxis = offSet.Y + (BoxSize.Y * y) - CenterOffSet.Y;

			for (int z = 0; z < GridSize.Z; ++z)
			{
				auto zAxis = offSet.Z + (BoxSize.Z * z) - CenterOffSet.Z;
				Grid.Add(FVector(xAxis, yAxis, zAxis));
			}
		}
	}

	if (DrawDebugGrid)
	{
		DrawDebugPoint(GetWorld(), offSet, 20.f, FColor::Cyan, true);
		for (int i = 0; i < Grid.Num(); ++i)
		{
			DrawDebugBox(GetWorld(), Grid[i], BoxSize / 2.f, FColor::Red, true);
			DrawDebugPoint(GetWorld(), Grid[i], 10.f, FColor::Emerald, true);
		}

		DrawDebugPoint(GetWorld(), FVector(xMinBound, offSet.Y, offSet.Z), 30.f, FColor::Black, true);
		DrawDebugPoint(GetWorld(), FVector(offSet.X, yMinBound, offSet.Z), 30.f, FColor::Black, true);

		DrawDebugPoint(GetWorld(), FVector(xMaxBound, offSet.Y, offSet.Z), 30.f, FColor::Black, true);
		DrawDebugPoint(GetWorld(), FVector(offSet.X, yMaxBound, offSet.Z), 30.f, FColor::Black, true);
	}
}

// Called every frame
void ACPP_Grid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Making this a vector 3 instead of vector so if i want to make it a 3d grid i can
const FIntVector ACPP_Grid::ConvertWorldLocationToGridLocation(FVector worldLocation)
{
	//worldLocation - GetActorLocation();
	//FMath::RoundToInt32()
	auto world = worldLocation;


	if (world.X > MaxBound.X || world.X < MinBound.X ||
		world.Y > MaxBound.Y || world.Y < MinBound.Y ||
		world.Z > MaxBound.Z || world.Z < MinBound.Z)
	{
		float clampedX = FMath::Clamp(world.X, MinBound.X, MaxBound.X);
		float clampedY = FMath::Clamp(world.Y, MinBound.Y, MaxBound.Y);
		float clampedZ = FMath::Clamp(world.Z, MinBound.Z, MaxBound.Z);;
		world = FVector(clampedX, clampedY, clampedZ);
	}


	auto relativeLocation = world - GetActorLocation();
	int32 x = FMath::RoundToInt32(relativeLocation.X / BoxSize.X);
	int32 y = FMath::RoundToInt32(relativeLocation.Y / BoxSize.Y);
	int32 z = FMath::RoundToInt32(relativeLocation.Z / BoxSize.Z);
	return FIntVector(x, y, z);
}

const FVector ACPP_Grid::ConvertGridLocationToWorldLocation(FIntVector gridLocation)
{
	auto offSet = GetActorLocation();
	float x = ((float)gridLocation.X) * BoxSize.X;
	float y = ((float)gridLocation.Y) * BoxSize.Y;
	float z = ((float)gridLocation.Z) * BoxSize.Z;

	auto returnVector = offSet + FVector(x, y, z);
	return FVector(returnVector);
}

const FVector ACPP_Grid::GetGridWorldLocation(FIntVector gridLocation)
{
	int32 x = FMath::Clamp(gridLocation.X, 0, GridSize.X);
	int32 y = FMath::Clamp(gridLocation.Y, 0, GridSize.Y);
	int32 z = FMath::Clamp(gridLocation.Z, 0, GridSize.Z);

	if(z <= 0)
		return Grid[x * y];

	return Grid[x * y * z];
}

