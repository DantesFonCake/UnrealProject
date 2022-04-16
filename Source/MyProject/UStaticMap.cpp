#include "UStaticMap.h"

UStaticMap::UStaticMap(const int MapSize, const FMyResourceProperties& Props):MapSize(MapSize),Props(Props)
{
	Amount.Init(0,GetFlatArraySize());
}

void UStaticMap::Initialize(const int NewMapSize, const FMyResourceProperties NewProps)
{
	MapSize=NewMapSize;
	//UE_LOG(LogTemp,Warning,TEXT("Array size {%d}"),GetFlatArraySize());
	Amount.Init(0,GetFlatArraySize());
	this->Props=NewProps;
}

float& UStaticMap::At(const int X, const int Y)
{
	//UE_LOG(LogTemp,Warning,TEXT("Accessing tile at {%d, %d}"),x,y);
	//return D[0];
	return Amount[InlineCoords(X, Y)];
}

const TArray<float>& UStaticMap::GetData()
{
	return Amount;
}

const float& UStaticMap::At(const int X, const int Y) const
{
	//UE_LOG(LogTemp,Warning,TEXT("Accessing const tile at {%d, %d}"),x,y);
	return Amount[InlineCoords(X, Y)];
}

void UStaticMap::InitializeLevel(const float Level)
{
	const auto length = (MapSize + 2) * (MapSize + 2);
	Amount.Init(Level, length);
}

void UStaticMap::SetLevel(const int X, const int Y, const float NewLevel)
{
	At(X, Y) = NewLevel;
}

void UStaticMap::AddLevel(const int X, const int Y, const float AddLevel)
{
	At(X, Y) += AddLevel;
}

void UStaticMap::SetAreaLevel(const int X, const int Y, const int AreaWidth, const int AreaHeight,
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

void UStaticMap::AddAreaLevel(const int X, const int Y, const int AreaWidth, const int AreaHeight,
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