#include "UDiffuseEqualisationHeightMap.h"

#define FOR_EACH_CELL for ( x=1 ; x<=MapSize ; x++ ) { for ( y=1 ; y<=MapSize ; y++ ) {
#define END_FOR }}
#define GAUSS_CLAMP(s,x1,y1) if(s>=0) { if(s<=Future[InlineCoords(x,y)]){ totalNet-=GroundLevel[InlineCoords(x,y)]-GroundLevel[InlineCoords(x1,y1)];} c+=a;}
#define GAUSS_CLAMP_D(s,x1,y1,d) if(s>=0) { if(s<=Future[InlineCoords(x,y)]){ totalNet-=(GroundLevel[InlineCoords(x,y)]-GroundLevel[InlineCoords(x1,y1)])*d;} c+=a*d;}

inline void UDiffuseEqualisationHeightMap::SetBound(const int BoundType, TArray<float>& Field) const
{
	for (int i = 1; i <= MapSize; i++)
	{
		Field[InlineCoords(0, i)] = BoundType == 1 ? -Field[InlineCoords(1, i)] : Field[InlineCoords(1, i)];
		Field[InlineCoords(MapSize + 1, i)] = BoundType == 1
			                                      ? -Field[InlineCoords(MapSize, i)]
			                                      : Field[InlineCoords(MapSize, i)];
		Field[InlineCoords(i, 0)] = BoundType == 2 ? -Field[InlineCoords(i, 1)] : Field[InlineCoords(i, 1)];
		Field[InlineCoords(i, MapSize + 1)] = BoundType == 2
			                                      ? -Field[InlineCoords(i, MapSize)]
			                                      : Field[InlineCoords(i, MapSize)];
	}
	Field[InlineCoords(0, 0)] = 0.5f * (Field[InlineCoords(1, 0)] + Field[InlineCoords(0, 1)]);
	Field[InlineCoords(0, MapSize + 1)] = 0.5f * (Field[InlineCoords(1, MapSize + 1)] + Field[
		InlineCoords(0, MapSize)]);
	Field[InlineCoords(MapSize + 1, 0)] = 0.5f * (Field[InlineCoords(MapSize, 0)] + Field[InlineCoords(
		MapSize + 1, 1)]);
	Field[InlineCoords(MapSize + 1, MapSize + 1)] = 0.5f * (Field[InlineCoords(MapSize, MapSize + 1)] + Field[
		InlineCoords(MapSize + 1, MapSize)]);
}

void UDiffuseEqualisationHeightMap::Rebalance(const float DeltaTime, const TArray<float>& GroundLevel,
                                              const TArray<float>& DiffuseMap)
{
	if (DiffuseMap.Num() == 0 || DiffuseMap.Num() != GetFlatArraySize())
	{
		Diffuse(DeltaTime, Amount, Future, GroundLevel);
	}
	else
	{
		Diffuse(DeltaTime, Amount, Future, GroundLevel, DiffuseMap);
	}
	std::memcpy(Amount.GetData(), Future.GetData(), GetFlatArraySize() * sizeof(float));
}

inline float UDiffuseEqualisationHeightMap::PushDiff(const int x, const int y, const int x1, const int y1,
                                                     const TArray<float>& Source,
                                                     const TArray<float>& GroundLevel) const
{
	const float xSource = Source[InlineCoords(x, y)];
	const float diff = (xSource + GroundLevel[InlineCoords(x, y)]) - GroundLevel[InlineCoords(x1, y1)];
	return diff;
}

void UDiffuseEqualisationHeightMap::Diffuse(const float DeltaTime, TArray<float>& X0, TArray<float>& X,
                                            const TArray<float>& GroundLevel)
{
	int x, y;
	const float a = DeltaTime * Props.Diffuse * MapSize * MapSize;
	for (int k = 0; k < 20; k++)
	{
		FOR_EACH_CELL
			float c = 1;

			float totalNet = FMath::Clamp<float>(PushDiff(x - 1, y, x, y, X, GroundLevel), 0, X[InlineCoords(x - 1, y)])
				+ FMath::Clamp<float>(PushDiff(x + 1, y, x, y, X, GroundLevel), 0, X[InlineCoords(x + 1, y)])
				+ FMath::Clamp<float>(PushDiff(x, y - 1, x, y, X, GroundLevel), 0, X[InlineCoords(x, y - 1)])
				+ FMath::Clamp<float>(PushDiff(x, y + 1, x, y, X, GroundLevel), 0, X[InlineCoords(x, y + 1)]);

			float lFrom = PushDiff(x, y, x - 1, y, X, GroundLevel);
			GAUSS_CLAMP(lFrom, x-1, y);
			float rFrom = PushDiff(x, y, x + 1, y, X, GroundLevel);
			GAUSS_CLAMP(rFrom, x+1, y);
			float uFrom = PushDiff(x, y, x, y - 1, X, GroundLevel);
			GAUSS_CLAMP(uFrom, x, y-1);
			float dFrom = PushDiff(x, y, x, y + 1, X, GroundLevel);
			GAUSS_CLAMP(dFrom, x, y+1)

			X[InlineCoords(x, y)] = (X0[InlineCoords(x, y)] + a * totalNet) / c;

			// X[InlineCoords(x, y)] = (Amount[InlineCoords(x, y)] + a * (X[InlineCoords(x - 1, y)] + X[
			// 		InlineCoords(x + 1, y)] + X[InlineCoords(x, y - 1)] + X[InlineCoords(x, y + 1)]
			// 	+ 2 * GroundLevel[InlineCoords(x - 1, y)] + 2 * GroundLevel[InlineCoords(x + 1, y)] + 2 * GroundLevel[
			// 		InlineCoords(x, y - 1)] + 2 * GroundLevel[InlineCoords(x, y + 1)] - 8 * GroundLevel[
			// 		InlineCoords(x, y)])) / c;
		END_FOR
		SetBound(0, X);
	}
}

void UDiffuseEqualisationHeightMap::Diffuse(float DeltaTime, TArray<float>& X0, TArray<float>& X,
                                            const TArray<float>& GroundLevel, const TArray<float>& DiffuseMap)
{
	int x, y;
	const float a = DeltaTime * Props.Diffuse * MapSize * MapSize;
	for (int k = 0; k < 20; k++)
	{
		FOR_EACH_CELL
			float c = 1;

			float totalNet = FMath::Clamp<float>(PushDiff(x - 1, y, x, y, X, GroundLevel), 0, X[InlineCoords(x - 1, y)])
				* (1-DiffuseMap[InlineCoords(x - 1, y)])
				+ FMath::Clamp<float>(PushDiff(x + 1, y, x, y, X, GroundLevel), 0, X[InlineCoords(x + 1, y)]) *
				(1-DiffuseMap[InlineCoords(x + 1, y)])
				+ FMath::Clamp<float>(PushDiff(x, y - 1, x, y, X, GroundLevel), 0, X[InlineCoords(x, y - 1)]) *
				(1-DiffuseMap[InlineCoords(x, y - 1)])
				+ FMath::Clamp<float>(PushDiff(x, y + 1, x, y, X, GroundLevel), 0, X[InlineCoords(x, y + 1)]) *
				(1-DiffuseMap[InlineCoords(x, y + 1)]);

			const float d = 1-DiffuseMap[InlineCoords(x, y)];
			const float lFrom = PushDiff(x, y, x - 1, y, X, GroundLevel);
			GAUSS_CLAMP_D(lFrom, x-1, y, d);
			const float rFrom = PushDiff(x, y, x + 1, y, X, GroundLevel);
			GAUSS_CLAMP_D(rFrom, x+1, y, d);
			const float uFrom = PushDiff(x, y, x, y - 1, X, GroundLevel);
			GAUSS_CLAMP_D(uFrom, x, y-1, d);
			const float dFrom = PushDiff(x, y, x, y + 1, X, GroundLevel);
			GAUSS_CLAMP_D(dFrom, x, y+1, d);

			X[InlineCoords(x, y)] = (X0[InlineCoords(x, y)] + a * totalNet) / c;

			// X[InlineCoords(x, y)] = (Amount[InlineCoords(x, y)] + a * (X[InlineCoords(x - 1, y)] + X[
			// 		InlineCoords(x + 1, y)] + X[InlineCoords(x, y - 1)] + X[InlineCoords(x, y + 1)]
			// 	+ 2 * GroundLevel[InlineCoords(x - 1, y)] + 2 * GroundLevel[InlineCoords(x + 1, y)] + 2 * GroundLevel[
			// 		InlineCoords(x, y - 1)] + 2 * GroundLevel[InlineCoords(x, y + 1)] - 8 * GroundLevel[
			// 		InlineCoords(x, y)])) / c;
		END_FOR
		SetBound(0, X);
	}
}

void UDiffuseEqualisationHeightMap::Initialize(const int NewMapSize, const FMyResourceProperties NewProps)
{
	Super::Initialize(NewMapSize, NewProps);
	Future.Init(0, GetFlatArraySize());
}
