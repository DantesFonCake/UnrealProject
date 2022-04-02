// Fill out your copyright notice in the Description page of Project Settings.


#include "UResourceMap.h"

void UResourceMap::Initialize(const int NewWidth, const int NewHeight, const FMyResourceProperties NewResourceProperties)
{
	Width=NewWidth;
	Height=NewHeight;
	ResourceProperties=NewResourceProperties;
	InitializeLevel(0);
	IsInitialized=true;
}

// Sets default values
UResourceMap::UResourceMap(): IsInitialized(false)
{
}

constexpr int UResourceMap::InlineCoords(const int X, const int Y) const
{
	return Y * Width + X;
}

void UResourceMap::SetResourceLevel(const int X, const int Y, const float NewLevel)
{
	At(X, Y) = NewLevel;
}

float& UResourceMap::At(const int X, const int Y)
{
	//UE_LOG(LogTemp,Warning,TEXT("Accessing tile at {%d, %d}"),x,y);
	return Data[InlineCoords(X, Y)];
}

const float& UResourceMap::At(const int X, const int Y) const
{
	//UE_LOG(LogTemp,Warning,TEXT("Accessing const tile at {%d, %d}"),x,y);
	return Data[InlineCoords(X, Y)];
}

const float& UResourceMap::DeltaAt(const int X, const int Y) const
{
	return Delta[InlineCoords(X, Y)];
}

float& UResourceMap::DeltaAt(const int X, const int Y)
{
	return Delta[InlineCoords(X, Y)];
}

void UResourceMap::AddResourceLevel(const int X, const int Y, const float AddLevel)
{
	At(X, Y) += AddLevel;
}

void UResourceMap::ClearDelta()
{
	int32 length = Delta.Num();
	Delta.Init(0, Height * Width);
	return;
	for (int i = 0; i < length; ++i)
	{
		Delta[i]*=0.9f;
	}
}

void UResourceMap::FillSingeCellDelta(const int X, const int Y, const float DeltaTime)
{
	const auto cLevel = At(X, Y);
	const auto maxFlow = ResourceProperties.FlowSpeed * DeltaTime * cLevel;
	const auto xMin = X - 1 >= 0 ? X - 1 : X;
	const auto yMin = Y - 1 >= 0 ? Y - 1 : Y;
	const auto xMax = X + 1 < Width ? X + 1 : X;
	const auto yMax = Y + 1 < Height ? Y + 1 : Y;
	const char distribution = (xMax - xMin + 1) * (yMax - yMin + 1) - 1;
	if (distribution == 0)
	{
		return;
	}
	const auto transfer = maxFlow / distribution;
	for (int yOffset = yMin; yOffset <= yMax; ++yOffset)
	{
		for (int xOffset = xMin; xOffset <= xMax; ++xOffset)
		{
			if (xOffset == X && yOffset == Y)
			{
				continue;
			}
			DeltaAt(xOffset, yOffset) += transfer;
			DeltaAt(X, Y) -= transfer;
		}
	}
}

void UResourceMap::SetAreaResourceLevel(const int X, const int Y, const int AreaWidth, const int AreaHeight, const float NewLevel)
{
	const auto xMax = X + AreaWidth;
	const auto yMax = Y + AreaHeight;
	for (auto j = 0; j < yMax; ++j)
	{
		for (int i = X; i < xMax; ++i)
		{
			At(i, j) = NewLevel;
		}
	}
}

void UResourceMap::AddAreaResourceLevel(const int X, const int Y, const int AreaWidth, const int AreaHeight, const float AddLevel)
{
	const auto xMax = X + AreaWidth;
	const auto yMax = Y + AreaHeight;
	for (auto j = 0; j < yMax; ++j)
	{
		for (int i = X; i < xMax; ++i)
		{
			At(i, j) += AddLevel;
		}
	}
}

void UResourceMap::Rebalance(const float DeltaTime)
{
	if (DeltaTime <= 0.016)
	{
		return;
	}
	for (auto y = 0; y < Height; ++y)
	{
		for (auto x = 0; x < Width; ++x)
		{
			FillSingeCellDelta(x, y, DeltaTime);
		}
	}
	for (auto y = 0; y < Height; ++y)
	{
		for (auto x = 0; x < Width; ++x)
		{
			At(x, y) += DeltaAt(x, y);
		}
	}
	ClearDelta();
}

constexpr bool UResourceMap::IsInRange(const int X, const int Y) const
{
	return X >= 0 && X < Width && Y >= 0 && Y < Height;
}

void UResourceMap::InitializeLevel(const float Level)
{
	const auto length = Width * Height;
	Data.Init(Level, length);
	Delta.Init(0, length);
}
