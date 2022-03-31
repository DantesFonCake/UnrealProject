// Fill out your copyright notice in the Description page of Project Settings.


#include "AResourceMap.h"

// Sets default values
AResourceMap::AResourceMap(): Width(128), Height(128)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Data.Init(0, Width * Height);
	DataSwap.Init(0, Width * Height);
}

// Called every frame
void AResourceMap::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Rebalance(DeltaTime);
	std::swap(Data, DataSwap);
}

constexpr int AResourceMap::InlineCoords(int x, int y) const
{
	return y * Width + x;
}

void AResourceMap::SetResourceLevel(int x, int y, float newLevel)
{
	At(x, y) = newLevel;
}

float& AResourceMap::At(int x, int y)
{
	//UE_LOG(LogTemp,Warning,TEXT("Accessing tile at {%d, %d}"),x,y);
	return Data[InlineCoords(x, y)];
}

const float& AResourceMap::At(int x, int y) const
{
	//UE_LOG(LogTemp,Warning,TEXT("Accessing const tile at {%d, %d}"),x,y);
	return Data[InlineCoords(x, y)];
}

void AResourceMap::AddResourceLevel(int x, int y, float addLevel)
{
	At(x, y) += addLevel;
}

void AResourceMap::BalanceSingleCell(int x, int y, float dT)
{
	const auto cLevel = At(x, y);
	const auto maxFlow = ResourceProperties.FlowSpeed * dT * cLevel;
	const auto xMin = x - 1 < 0 ? x : x - 1;
	const auto yMin = y - 1 < 0 ? y : y - 1;
	const auto xMax = x + 1 >= Width ? x : x + 1;
	const auto yMax = y + 1 >= Height ? y : y + 1;
	char distribution = (xMax - xMin) * (yMax - yMin) - 1;
	for (int yOffset = yMin; yOffset < yMax; ++yOffset)
	{
		for (int xOffset = xMin; xOffset < xMax; ++xOffset)
		{
			if (xOffset == x && yOffset == y) continue;
			const auto transfer = maxFlow / distribution;
			DataSwap[InlineCoords(xOffset, yOffset)] += transfer;
			DataSwap[InlineCoords(x, y)] -= transfer;
		}
	}
}

void AResourceMap::SetAreaResourceLevel(int x, int y, int width, int height, float newLevel)
{
	const auto xMax = x + width;
	const auto yMax = y + height;
	for (auto j = 0; j < yMax; ++j)
	{
		for (int i = x; i < xMax; ++i)
		{
			At(i, j) = newLevel;
		}
	}
}

void AResourceMap::AddAreaResourceLevel(int x, int y, int width, int height, float addLevel)
{
	const auto xMax = x + width;
	const auto yMax = y + height;
	for (auto j = 0; j < yMax; ++j)
	{
		for (int i = x; i < xMax; ++i)
		{
			At(i, j) += addLevel;
		}
	}
}

void AResourceMap::Rebalance(float dT)
{
	if (dT <= 0.016)
	{
		return;
	}
	for (auto y = 0; y < Height; ++y)
	{
		for (auto x = 0; x < Width; ++x)
		{
			BalanceSingleCell(x, y, dT);
		}
	}
}

constexpr bool AResourceMap::IsInRange(int x, int y) const
{
	return x >= 0 && x < Width && y >= 0 && y < Height;
}

void AResourceMap::InitializeLevel(float level)
{
	const auto length = Width * Height;
	Data.Init(level, length);
	DataSwap.Init(level, length);
}
