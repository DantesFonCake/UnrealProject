#include "UStaticHeightMap.h"

UStaticHeightMap::UStaticHeightMap(int MapSize, FMyResourceProperties& Props):MapSize(MapSize),Props(Props)
{
	D.Init(0,GetFlatArraySize());
}

void UStaticHeightMap::Initialize(int NewMapSize, FMyResourceProperties NewProps)
{
	MapSize=NewMapSize;
	//UE_LOG(LogTemp,Warning,TEXT("Array size {%d}"),GetFlatArraySize());
	D.Init(0,GetFlatArraySize());
	this->Props=NewProps;
}


constexpr int UStaticHeightMap::InlineCoords(const int X, const int Y) const
{
	return Y * (MapSize+2) + X;
}

float& UStaticHeightMap::At(const int X, const int Y)
{
	//UE_LOG(LogTemp,Warning,TEXT("Accessing tile at {%d, %d}"),x,y);
	//return D[0];
	return D[InlineCoords(X, Y)];
}

const float& UStaticHeightMap::At(const int X, const int Y) const
{
	//UE_LOG(LogTemp,Warning,TEXT("Accessing const tile at {%d, %d}"),x,y);
	return D[InlineCoords(X, Y)];
}

void UStaticHeightMap::InitializeLevel(const float Level)
{
	const auto length = (MapSize + 2) * (MapSize + 2);
	D.Init(Level, length);
}

void UStaticHeightMap::SetResourceLevel(const int X, const int Y, const float NewLevel)
{
	At(X, Y) = NewLevel;
}

void UStaticHeightMap::AddResourceLevel(const int X, const int Y, const float AddLevel)
{
	At(X, Y) += AddLevel;
}

void UStaticHeightMap::SetAreaResourceLevel(const int X, const int Y, const int AreaWidth, const int AreaHeight,
										const float NewLevel)
{
	const auto xMax = X + AreaWidth;
	const auto yMax = Y + AreaHeight;
	for (auto j = Y; j < yMax; ++j)
	{
		for (int i = X; i < xMax; ++i)
		{
			At(i, j) = NewLevel;
		}
	}
}

void UStaticHeightMap::AddAreaResourceLevel(const int X, const int Y, const int AreaWidth, const int AreaHeight,
										const float AddLevel)
{
	const auto xMax = X + AreaWidth;
	const auto yMax = Y + AreaHeight;
	for (auto j = Y; j < yMax; ++j)
	{
		for (int i = X; i < xMax; ++i)
		{
			At(i, j) += AddLevel;
		}
	}
}