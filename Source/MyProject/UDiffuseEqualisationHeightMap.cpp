#include "UDiffuseEqualisationHeightMap.h"
//#include "../../Plugins/CustomFluidShader/Source/CustomFluidShader/Private/CustomFluidShaderManager.h"
#include "CustomFluidShader.h"
#define FOR_EACH_CELL for ( i=1 ; i<=MapSize ; i++ ) { for ( j=1 ; j<=MapSize ; j++ ) {
#define END_FOR }}
#define GAUSS_CLAMP(s,x1,y1) if(s>=0) { if(s<=Future[InlineCoords(i,j)]){ totalNet-=GroundLevel[InlineCoords(i,j)]-GroundLevel[InlineCoords(x1,y1)];} c+=a;}
#define GAUSS_CLAMP_D(s,x1,y1,d) if(s>=0) { if(s<=Future[InlineCoords(i,j)]){ totalNet-=(GroundLevel[InlineCoords(i,j)]-GroundLevel[InlineCoords(x1,y1)])*d;} c+=a*d;}

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
	//Future.Init(0, GetFlatArraySize());
	Diffuse(DeltaTime, Amount, Future, GroundLevel, DiffuseMap);
	/*for (size_t i = 0; i < GetFlatArraySize(); i++)
	{
		Amount[i] = Future[i];
	}*/
	//std::memcpy(Amount.GetData(), Future.GetData(), GetFlatArraySize() * sizeof(float));
	Advect(DeltaTime, Future, Amount, VelocityX, VelocityY);
	//CustomFluidShaderManager::Get(MapSize)->Dispatch(this->Future, this->Amount, GroundLevel, DiffuseMap, this->MapSize * this->MapSize * this->Props.Diffuse * DeltaTime, MapSize);
	//FMemory::Memcpy(Amount.GetData(), CustomFluidShaderManager::Get(MapSize)->lastResults.GetData(), GetFlatArraySize() * sizeof(float));

}

inline float UDiffuseEqualisationHeightMap::PushDiff(const int x, const int y, const int x1, const int y1,
                                                     const TArray<float>& Source,
                                                     const TArray<float>& GroundLevel) const
{
	const float xSource = Source[InlineCoords(x, y)];
	const float diff = (xSource + GroundLevel[InlineCoords(x, y)]) - GroundLevel[InlineCoords(x1, y1)];
	return diff;
}

void UDiffuseEqualisationHeightMap::Diffuse(float DeltaTime, TArray<float>& X0, TArray<float>& X,
                                            const TArray<float>& GroundLevel, const TArray<float>& DiffuseMap)
{
	int i, j;
	const float a = DeltaTime * Props.Diffuse * MapSize * MapSize;
	for (int k = 0; k < 20; k++)
	{
#pragma omp parallel for private(i)
		for (i = 1; i <= MapSize; i++) {
#pragma omp parallel for private(j)
			for (j = 1; j <= MapSize; j++) {
			float c = 1;

			float totalNet = FMath::Clamp<float>(PushDiff(i - 1, j, i, j, X, GroundLevel), 0, X[InlineCoords(i - 1, j)])
				* (1-DiffuseMap[InlineCoords(i - 1, j)])
				+ FMath::Clamp<float>(PushDiff(i + 1, j, i, j, X, GroundLevel), 0, X[InlineCoords(i + 1, j)]) *
				(1-DiffuseMap[InlineCoords(i + 1, j)])
				+ FMath::Clamp<float>(PushDiff(i, j - 1, i, j, X, GroundLevel), 0, X[InlineCoords(i, j - 1)]) *
				(1-DiffuseMap[InlineCoords(i, j - 1)])
				+ FMath::Clamp<float>(PushDiff(i, j + 1, i, j, X, GroundLevel), 0, X[InlineCoords(i, j + 1)]) *
				(1-DiffuseMap[InlineCoords(i, j + 1)]);

			const float d = 1-DiffuseMap[InlineCoords(i, j)];
			const float lFrom = PushDiff(i, j, i - 1, j, X, GroundLevel);
			GAUSS_CLAMP_D(lFrom, i-1, j, d);
			const float rFrom = PushDiff(i, j, i + 1, j, X, GroundLevel);
			GAUSS_CLAMP_D(rFrom, i+1, j, d);
			const float uFrom = PushDiff(i, j, i, j - 1, X, GroundLevel);
			GAUSS_CLAMP_D(uFrom, i, j-1, d);
			const float dFrom = PushDiff(i, j, i, j + 1, X, GroundLevel);
			GAUSS_CLAMP_D(dFrom, i, j+1, d);

			X[InlineCoords(i, j)] = (X0[InlineCoords(i, j)] + a * totalNet) / c;
		END_FOR
		SetBound(0, X);
	}
}

void UDiffuseEqualisationHeightMap::Advect(float DeltaTime, TArray<float>&X0, TArray<float>&X, const TArray<float>&U, const TArray<float>&V)
{
	/*int i, j;

	const float dt0 = DeltaTime * MapSize;
	FOR_EACH_CELL
	float backCenterX = i - dt0 * U[InlineCoords(i, j)];
	float backCenterY = j - dt0 * V[InlineCoords(i, j)];
	if (backCenterX < 0.5f)
	{
		backCenterX = 0.5f;
	}
	if (backCenterX > MapSize + 0.5f)
	{
		backCenterX = MapSize + 0.5f;
	}
	const int xFrom = static_cast<int>(backCenterX);
	const int xTo = xFrom + 1;
	if (backCenterY < 0.5f)
	{
		backCenterY = 0.5f;
	}
	if (backCenterY > MapSize + 0.5f)
	{
		backCenterY = MapSize + 0.5f;
	}
	const int yFrom = static_cast<int>(backCenterY);
	const int yTo = yFrom + 1;
	const float s1 = backCenterX - xFrom;
	const float s0 = 1 - s1;
	const float t1 = backCenterY - yFrom;
	const float t0 = 1 - t1;
	X[InlineCoords(i, j)] = s0 * (t0 * X0[InlineCoords(xFrom, yFrom)] + t1 * X0[InlineCoords(xFrom, yTo)]) +
							s1 * (t0 * X0[InlineCoords(xTo, yFrom)] + t1 * X0[InlineCoords(xTo, yTo)]);
	END_FOR
		SetBound( 0, X);*/

	int i, j, i0, j0, i1, j1;
	float x, y, s0, t0, s1, t1, dt0;

	dt0 = DeltaTime * MapSize;
	FOR_EACH_CELL
		x = i - dt0 * U[InlineCoords(i, j)]; y = j - dt0 * V[InlineCoords(i, j)];
	if (x < 0.5f) x = 0.5f; if (x > MapSize + 0.5f) x = MapSize + 0.5f; i0 = (int)x; i1 = i0 + 1;
	if (y < 0.5f) y = 0.5f; if (y > MapSize + 0.5f) y = MapSize + 0.5f; j0 = (int)y; j1 = j0 + 1;
	s1 = x - i0; s0 = 1 - s1; t1 = y - j0; t0 = 1 - t1;
	X[InlineCoords(i, j)] = s0 * (t0 * X0[InlineCoords(i0, j0)] + t1 * X0[InlineCoords(i0, j1)]) +
		s1 * (t0 * X0[InlineCoords(i1, j0)] + t1 * X0[InlineCoords(i1, j1)]);
	END_FOR
		SetBound(0, X);
}

void UDiffuseEqualisationHeightMap::Initialize(const int NewMapSize, const FMyResourceProperties NewProps)
{
	Super::Initialize(NewMapSize, NewProps);
	Future.Init(0, GetFlatArraySize());
	VelocityX.Init(0, GetFlatArraySize());
	VelocityY.Init(0, GetFlatArraySize());
	SetBound(1, VelocityX);
	SetBound(2, VelocityY);
}
