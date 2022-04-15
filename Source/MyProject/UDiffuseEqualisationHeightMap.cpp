#include "UDiffuseEqualisationHeightMap.h"

#define FOR_EACH_CELL for ( x=1 ; x<=MapSize ; x++ ) { for ( y=1 ; y<=MapSize ; y++ ) {
#define END_FOR }}
#define GAUSS_CLAMP(s,x1,y1) if(s>=0) { if(s<=Future[InlineCoords(x,y)]){ totalNet-=GroundLevel[InlineCoords(x,y)]-GroundLevel[InlineCoords(x1,y1)];} c+=a;}
void UDiffuseEqualisationHeightMap::Rebalance(float DeltaTime)
{
	//Diffuse(DeltaTime);
	int x, y;
	const float a = DeltaTime * Props.Diffuse * MapSize * MapSize;
	const float c = 1 + 4 * a;
	for (int k = 0; k < 20; k++)
	{
		FOR_EACH_CELL
			Future[InlineCoords(x, y)] = (Amount[InlineCoords(x, y)] +
				a * (Future[InlineCoords(x - 1, y)] + Future[InlineCoords(x + 1, y)] + Future[InlineCoords(x, y - 1)] +
					Future[InlineCoords(x, y + 1)])) / c;
		END_FOR
		for (int i = 1; i <= MapSize; i++)
		{
			Future[InlineCoords(0, i)] = Future[InlineCoords(1, i)];
			Future[InlineCoords(MapSize + 1, i)] = Future[InlineCoords(MapSize, i)];
			Future[InlineCoords(i, 0)] = Future[InlineCoords(i, 1)];
			Future[InlineCoords(i, MapSize + 1)] = Future[InlineCoords(i, MapSize)];
		}
		Future[InlineCoords(0, 0)] = 0.5f * (Future[InlineCoords(1, 0)] + Future[InlineCoords(0, 1)]);
		Future[InlineCoords(0, MapSize + 1)] = 0.5f * (Future[InlineCoords(1, MapSize + 1)]
			+ Future[InlineCoords(0, MapSize)]);
		Future[InlineCoords(MapSize + 1, 0)] = 0.5f * (Future[InlineCoords(MapSize, 0)]
			+ Future[InlineCoords(MapSize + 1, 1)]);
		Future[InlineCoords(MapSize + 1, MapSize + 1)] = 0.5f * (Future[InlineCoords(MapSize, MapSize + 1)]
			+ Future[InlineCoords(MapSize + 1, MapSize)]);
	}

	std::memcpy(Amount.GetData(), Future.GetData(), GetFlatArraySize() * sizeof(float));
}


void UDiffuseEqualisationHeightMap::RebalanceWithGround(float DeltaTime, const TArray<float>& GroundLevel)
{
	//DiffuseWithGround(DeltaTime, GroundLevel);

	int x, y;
	const float a = 1*DeltaTime * Props.Diffuse * MapSize * MapSize;
	for (int k = 0; k < 20; k++)
	{
		FOR_EACH_CELL
			float c = 1;
			// Future[InlineCoords(x, y)] = (Amount[InlineCoords(x, y)] + GroundLevel[InlineCoords(x, y)] +
			// 	a * (Future[InlineCoords(x - 1, y)] + GroundLevel[InlineCoords(x - 1, y)] + Future[
			// 			InlineCoords(x + 1, y)] + GroundLevel[InlineCoords(x + 1, y)] + Future[InlineCoords(x, y - 1)] +
			// 		GroundLevel[InlineCoords(x, y - 1)] + Future[InlineCoords(x, y + 1)] + GroundLevel[
			// 			InlineCoords(x, y + 1)])) / c - GroundLevel[InlineCoords(x, y)];
			float totalNet= FMath::Clamp<float>(PushDiff(x - 1, y, x, y, Future, GroundLevel),0,Future[InlineCoords(x-1,y)])
							+ FMath::Clamp<float>(PushDiff(x + 1, y, x, y, Future, GroundLevel),0,Future[InlineCoords(x+1,y)])
							+ FMath::Clamp<float>(PushDiff(x, y - 1, x, y, Future, GroundLevel),0,Future[InlineCoords(x,y-1)])
							+ FMath::Clamp<float>(PushDiff(x, y + 1, x, y, Future, GroundLevel),0,Future[InlineCoords(x,y+1)]);
		
			float lFrom = PushDiff(x,y,x-1,y,Future,GroundLevel);
			GAUSS_CLAMP(lFrom,x-1,y);
			float rFrom = PushDiff(x,y,x+1,y,Future,GroundLevel);
			GAUSS_CLAMP(rFrom,x+1,y);
			float uFrom = PushDiff(x,y,x,y-1,Future,GroundLevel);
			GAUSS_CLAMP(uFrom,x,y-1);
			float dFrom = PushDiff(x,y,x,y+1,Future,GroundLevel);
			GAUSS_CLAMP(dFrom,x,y+1)
		
			Future[InlineCoords(x,y)]=(Amount[InlineCoords(x,y)]+a*totalNet)/c;
			
			// Future[InlineCoords(x, y)] = (Amount[InlineCoords(x, y)] + a * (Future[InlineCoords(x - 1, y)] + Future[
			// 		InlineCoords(x + 1, y)] + Future[InlineCoords(x, y - 1)] + Future[InlineCoords(x, y + 1)]
			// 	+ 2 * GroundLevel[InlineCoords(x - 1, y)] + 2 * GroundLevel[InlineCoords(x + 1, y)] + 2 * GroundLevel[
			// 		InlineCoords(x, y - 1)] + 2 * GroundLevel[InlineCoords(x, y + 1)] - 8 * GroundLevel[
			// 		InlineCoords(x, y)])) / c;
		END_FOR
		for (int i = 1; i <= MapSize; i++)
		{
			Future[InlineCoords(0, i)] = Future[InlineCoords(1, i)];
			Future[InlineCoords(MapSize + 1, i)] = Future[InlineCoords(MapSize, i)];
			Future[InlineCoords(i, 0)] = Future[InlineCoords(i, 1)];
			Future[InlineCoords(i, MapSize + 1)] = Future[InlineCoords(i, MapSize)];
		}
		Future[InlineCoords(0, 0)] = 0.5f * (Future[InlineCoords(1, 0)] + Future[InlineCoords(0, 1)]);
		Future[InlineCoords(0, MapSize + 1)] = 0.5f * (Future[InlineCoords(1, MapSize + 1)] + Future[
			InlineCoords(0, MapSize)]);
		Future[InlineCoords(MapSize + 1, 0)] = 0.5f * (Future[InlineCoords(MapSize, 0)] + Future[InlineCoords(
			MapSize + 1, 1)]);
		Future[InlineCoords(MapSize + 1, MapSize + 1)] = 0.5f * (Future[InlineCoords(MapSize, MapSize + 1)] + Future[
			InlineCoords(MapSize + 1, MapSize)]);
	}

	std::memcpy(Amount.GetData(), Future.GetData(), GetFlatArraySize() * sizeof(float));
}

//invalid
void UDiffuseEqualisationHeightMap::Diffuse(float DeltaTime)
{
	if (Props.Diffuse == 0)
	{
		return;
	}
	//std::memcpy(Future.GetData(), Amount.GetData(), GetFlatArraySize() * sizeof(float));
	for (int i = 0; i < GetFlatArraySize(); i++)
	{
		Future[i] = Amount[i];
	}
	for (int i = 1; i <= MapSize; i++)
	{
		for (int j = 1; j <= MapSize; j++)
		{
			float d = Amount[InlineCoords(i, j)]
				- Amount[InlineCoords(i + 1, j)];
			if (d < 0)
			{
				d = 0;
			}
			float u = Amount[InlineCoords(i, j)]
				- Amount[InlineCoords(i - 1, j)];
			if (u < 0)
			{
				u = 0;
			}
			float l = Amount[InlineCoords(i, j)]
				- Amount[InlineCoords(i, j - 1)];
			if (l < 0)
			{
				l = 0;
			}
			float r = Amount[InlineCoords(i, j)]
				- Amount[InlineCoords(i, j + 1)];
			if (r < 0)
			{
				r = 0;
			}
			const float totalDiff = u + d + l + r;
			UE_LOG(LogTemp, Warning, TEXT("Total diff {%f} at {%d,%d}"), totalDiff, i, j);
			float ratio = 0;
			if (totalDiff != 0)
			{
				ratio = Amount[InlineCoords(i, j)] * Props.Diffuse * DeltaTime / (totalDiff);
				UE_LOG(LogTemp, Warning, TEXT("Total ratio {%f} at {%d,%d}"), ratio, i, j);
			}

			Pull(i, j, i - 1, j, u * ratio);
			Pull(i, j, i + 1, j, d * ratio);
			Pull(i, j, i, j - 1, l * ratio);
			Pull(i, j, i, j + 1, r * ratio);
		}
	}

	//std::memcpy(Amount.GetData(), Future.GetData(), GetFlatArraySize() * sizeof(float));
	for (int i = 0; i < GetFlatArraySize(); i++)
	{
		Amount[i] = Future[i];
	}
}

inline float UDiffuseEqualisationHeightMap::Difference(int x, int y, int x1, int y1, const TArray<float>& Source,
                                                       const TArray<float>& GroundLevel) const
{
	const float diff = Source[InlineCoords(x, y)] + GroundLevel[InlineCoords(x, y)]
		- Source[InlineCoords(x1, y1)] - GroundLevel[InlineCoords(x1, y1)];
	if (diff < 0)
	{
		return 0;
	}
	return diff;
}

inline float UDiffuseEqualisationHeightMap::PushDiff(int x, int y, int x1, int y1, const TArray<float>& Source,
                                                     const TArray<float>& GroundLevel) const
{
	const float xSource = Source[InlineCoords(x, y)];
	const float diff = (xSource + GroundLevel[InlineCoords(x, y)]) - GroundLevel[InlineCoords(x1, y1)];
	return diff;
}

void inline UDiffuseEqualisationHeightMap::Pull(int x, int y, int x1, int y1, float d)
{
	Future[InlineCoords(x, y)] -= d;
	Future[InlineCoords(x1, y1)] += d;
}

//invalid
void UDiffuseEqualisationHeightMap::DiffuseWithGround(float DeltaTime, const TArray<float>& GroundLevel)
{
	if (Props.Diffuse == 0)
	{
		return;
	}
	std::memcpy(Future.GetData(), Amount.GetData(), GetFlatArraySize() * sizeof(float));
	for (int i = 1; i <= MapSize; i++)
	{
		for (int j = 1; j <= MapSize; j++)
		{
			float d = Difference(i, j, i + 1, j, Amount, GroundLevel);
			float u = Difference(i, j, i - 1, j, Amount, GroundLevel);
			float l = Difference(i, j, i, j - 1, Amount, GroundLevel);
			float r = Difference(i, j, i, j + 1, Amount, GroundLevel);

			const float totalDiff = u + d + l + r;
			float ratio = 0;
			if (totalDiff != 0)
			{
				ratio = Amount[InlineCoords(i, j)] * Props.Diffuse * DeltaTime / (totalDiff);
			}

			Pull(i, j, i - 1, j, u * ratio);
			Pull(i, j, i + 1, j, d * ratio);
			Pull(i, j, i, j - 1, l * ratio);
			Pull(i, j, i, j + 1, r * ratio);
		}
	}

	std::memcpy(Amount.GetData(), Future.GetData(), GetFlatArraySize() * sizeof(float));
}


void UDiffuseEqualisationHeightMap::Initialize(int NewMapSize, FMyResourceProperties NewProps)
{
	Super::Initialize(NewMapSize, NewProps);
	Future.Init(0, GetFlatArraySize());
}
