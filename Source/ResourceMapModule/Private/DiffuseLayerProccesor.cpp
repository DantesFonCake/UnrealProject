// Fill out your copyright notice in the Description page of Project Settings.

#include "DiffuseLayerProccesor.h"
#include "ResourceMapModule.h"

DECLARE_CYCLE_STAT(TEXT("Diffuse_Proccessor"),STAT_DIFFUSE,STATGROUP_RMM)
DECLARE_CYCLE_STAT(TEXT("Diffuse_Proccessor_Bound_Routine"),STAT_DIFFUSE_BOUND,STATGROUP_RMM)
DECLARE_CYCLE_STAT(TEXT("Diffuse_Proccessor_Diffuse_Routine"),STAT_DIFFUSE_DIFFUSE,STATGROUP_RMM)
DECLARE_CYCLE_STAT(TEXT("Diffuse_Proccessor_Advect_Routine"),STAT_DIFFUSE_ADVECT,STATGROUP_RMM)

#define IX(x,y) ((y) * (Size+2) + (x))

template<int BoundType>
void UDiffuseLayerProccesor::PlainLinSolve(int Size, float* X, float* X0, float a, float c)
{
	for (int k = 0; k < 20; k++)
	{
#pragma omp parallel for shared(X0,X)
		for (int i = 1; i <= Size; i++) {
#pragma omp parallel for shared(X0,X)
			for (int j = 1; j <= Size; j++) {
				X[IX(i, j)] = (X0[IX(i, j)] + a * (X[IX(i - 1, j)] + X[IX(i + 1, j)] + X[IX(i, j - 1)] + X[IX(i, j + 1)])) / c;
			}
		}
		SetBound<BoundType>(Size, X);
	}
}

template<int BoundType>
void UDiffuseLayerProccesor::PlainDiffuse(int Size, float* X, float* X0, float Diff, float DeltaTime)
{
	const float a = DeltaTime * Diff * Size * Size;
	PlainLinSolve<BoundType>(Size, X, X0, a, 1 + 4 * a);
}

void UDiffuseLayerProccesor::Project(int Size, float* U, float* V, float* P, float* Div)
{
#pragma omp parallel for shared(U,V,P,Div)
	for (int i = 1; i <= Size; i++) {
#pragma omp parallel for shared(U,V,P,Div)
		for (int j = 1; j <= Size; j++) {
			Div[IX(i, j)] = -0.5f * (U[IX(i + 1, j)] - U[IX(i - 1, j)] + V[IX(i, j + 1)] - V[IX(i, j - 1)]) / Size;
			P[IX(i, j)] = 0;
		}
	}
	SetBound<0>(Size, Div);
	SetBound<0>(Size, P);

	PlainLinSolve<0>(Size, P, Div, 1, 4);

#pragma omp parallel for shared(U,V,P,Div)
	for (int i = 1; i <= Size; i++) {
#pragma omp parallel for shared(U,V,P,Div)
		for (int j = 1; j <= Size; j++) {
			U[IX(i, j)] -= 0.5f * Size * (P[IX(i + 1, j)] - P[IX(i - 1, j)]);
			V[IX(i, j)] -= 0.5f * Size * (P[IX(i, j + 1)] - P[IX(i, j - 1)]);
		}
	}
	SetBound<1>(Size, U);
	SetBound<2>(Size, V);
}

void UDiffuseLayerProccesor::VelocityStep(int Size, float* U, float* V, float* U0, float* V0, float Visc, float DeltaTime)
{
	PlainDiffuse<1>(Size, U0, U, Visc, DeltaTime);
	PlainDiffuse<2>(Size, V0, V, Visc, DeltaTime);
	Project(Size, U0, V0, U, V);
	Advect<1>(Size, DeltaTime, U, U0, U0, V0);
	Advect<2>(Size, DeltaTime, V, V0, U0, V0);
	Project(Size, U, V, U0, V0);
}

#define FLAT_SIZE(s) ((s)+2)*((s)+2)
void UDiffuseLayerProccesor::Proccess_Implementation(UResourceMapManager* Manager, float DeltaTime)
{
	SCOPE_CYCLE_COUNTER(STAT_DIFFUSE);


	const auto layer = Manager->GetNamedDynamicLayer(LayerToProcces); 
	auto layerAmountArray = layer->GetAmountArray().GetData();
	auto layerFutureArray = layer->GetAmountArray().GetData();
	const int Size = Manager->GetSize();
	const auto ground = Manager->GetNamedStaticLayer(layer->GetAssociatedGroundName());
	auto groundArray = ground->GetAmountArray().GetData();
	const auto diffuseMap = Manager->GetNamedStaticLayer(layer->GetAssociatedDiffuseMapName());
	auto diffuseMapArray = diffuseMap->GetAmountArray().GetData();

	SetBound<0>(Size, layerAmountArray);
	SetBound<0>(Size, groundArray);
	SetBound<0>(Size, diffuseMapArray);
	Diffuse(Size, DeltaTime, layer->GetDiffuse(), layerAmountArray, layerFutureArray, groundArray, diffuseMapArray);
	if (layer->GetAssociatedVelocityFieldLayer() != Manager->GetNoneVelocityFieldName()) {
		const auto velocityField = Manager->GetNamedVelocityField(layer->GetAssociatedVelocityFieldLayer());
		auto uArray = velocityField->GetUArray().GetData();
		auto vArray = velocityField->GetVArray().GetData();
		auto u0Array = velocityField->GetU0Array().GetData();
		auto v0Array = velocityField->GetV0Array().GetData();
		VelocityStep(Size, uArray, vArray, u0Array, v0Array, velocityField->GetViscosity(), DeltaTime);
		Advect<0>(Size, DeltaTime, layerFutureArray, layerAmountArray, uArray, vArray);
		return;
	}
	FMemory::Memcpy(layerAmountArray, layerFutureArray, FLAT_SIZE(Size)*sizeof(float));
	layer->MarkDirty();
}

#define GAUSS_CLAMP(s,g) if((s)>=0) { if((s)<=xCenter){ totalNet-=gCenter-g;} c+=a;}
#define GAUSS_CLAMP_D(s,g) if((s)>=0) { if((s)<=xCenter){ totalNet-=(gCenter-g)*dCenter;} c+=a*dCenter;}

void UDiffuseLayerProccesor::Diffuse(const int Size,const float DeltaTime, const float Diffuse,float* X0, float* X,
	const float* GroundLevel, const float* DiffuseMap)
{
	SCOPE_CYCLE_COUNTER(STAT_DIFFUSE_DIFFUSE);

	const float a = DeltaTime * Diffuse * Size * Size;
	for (int k = 0; k < 20; k++)
	{
#pragma omp parallel for shared(X0,X,GroundLevel,DiffuseMap)
		for (int i = 1; i <= Size; i++) {
#pragma omp parallel for shared(X0,X,GroundLevel,DiffuseMap)
			for (int j = 1; j <= Size; j++) {
				float c = 1;
				const int centerI = IX(i, j);
				const float xCenter = X[centerI];
				const float gCenter = GroundLevel[centerI];
				const float dCenter = 1 - DiffuseMap[centerI];
				const int leftI = IX(i - 1, j);
				const float xLeft = X[leftI];
				const float gLeft = GroundLevel[leftI];
				const float dLeft = 1 - DiffuseMap[leftI];
				const int rightI = IX(i + 1, j);
				const float xRight = X[rightI];
				const float gRight = GroundLevel[rightI];
				const float dRight = 1 - DiffuseMap[rightI];
				const int upI = IX(i, j - 1);
				const float xUp = X[upI];
				const float gUp = GroundLevel[upI];
				const float dUp = 1 - DiffuseMap[upI];
				const int downI = IX(i, j + 1);
				const float xDown = X[downI];
				const float gDown = GroundLevel[downI];
				const float dDown = 1 - DiffuseMap[downI];

				float totalNet = FMath::Clamp<float>(xLeft + gLeft - gCenter, 0, xLeft) * dLeft
					+ FMath::Clamp<float>(xRight + gRight - gCenter, 0, xRight) * dRight
					+ FMath::Clamp<float>(xUp + gUp - gCenter, 0, xUp) * dUp
					+ FMath::Clamp<float>(xDown + gDown - gCenter, 0, xDown) * dDown;

				const float lFrom = xCenter + gCenter - gLeft;
				GAUSS_CLAMP_D(lFrom, gLeft);

				const float rFrom = xCenter + gCenter - gRight;
				GAUSS_CLAMP_D(rFrom, gRight);

				const float uFrom = xCenter + gCenter - gUp;
				GAUSS_CLAMP_D(uFrom, gUp);

				const float dFrom = xCenter + gCenter - gDown;
				GAUSS_CLAMP_D(dFrom, gDown);

				X[centerI] = (X0[centerI] + a * totalNet) / c;
			}
		}
		SetBound<0>(Size, X);
	}
}

template<int BoundType>
void UDiffuseLayerProccesor::Advect(const int Size, const float DeltaTime, float* X0, float* X, const float* U, const float* V) {
	SCOPE_CYCLE_COUNTER(STAT_DIFFUSE_ADVECT);

	int i0, j0, i1, j1;
	float x, y, s0, t0, s1, t1, dt0;

	dt0 = DeltaTime * Size;
#pragma omp parallel for shared(X0,X,U,V)
	for (int i = 1; i <= Size; i++) {
#pragma omp parallel for shared(X0,X,U,V)
		for (int j = 1; j <= Size; j++) {
			x = i - dt0 * U[IX(i, j)]; y = j - dt0 * V[IX(i, j)];
			if (x < 0.5f) x = 0.5f; if (x > Size + 0.5f) x = Size + 0.5f; i0 = (int)x; i1 = i0 + 1;
			if (y < 0.5f) y = 0.5f; if (y > Size + 0.5f) y = Size + 0.5f; j0 = (int)y; j1 = j0 + 1;
			s1 = x - i0; s0 = 1 - s1; t1 = y - j0; t0 = 1 - t1;
			X[IX(i, j)] = s0 * (t0 * X0[IX(i0, j0)] + t1 * X0[IX(i0, j1)]) +
				s1 * (t0 * X0[IX(i1, j0)] + t1 * X0[IX(i1, j1)]);
		}
	}
	SetBound<BoundType>(Size, X);
}

inline float UDiffuseLayerProccesor::PushDiff(const int Size, const int x, const int y, const int x1, const int y1,
	const float* Source,
	const float* GroundLevel)
{
	const float xSource = Source[IX(x, y)];
	const float diff = (xSource + GroundLevel[IX(x, y)]) - GroundLevel[IX(x1, y1)];
	return diff;
}

template<int BoundType>
inline void UDiffuseLayerProccesor::SetBound(const int Size, float* Field)
{
	SCOPE_CYCLE_COUNTER(STAT_DIFFUSE_BOUND);

	for (int i = 1; i <= Size; i++)
	{
		Field[IX(0, i)] = BoundType == 1 ? -Field[IX(1, i)] : Field[IX(1, i)];
		Field[IX(Size + 1, i)] = BoundType == 1
			? -Field[IX(Size, i)]
			: Field[IX(Size, i)];
		Field[IX(i, 0)] = BoundType == 2 ? -Field[IX(i, 1)] : Field[IX(i, 1)];
		Field[IX(i, Size + 1)] = BoundType == 2
			? -Field[IX(i, Size)]
			: Field[IX(i, Size)];
	}
	Field[IX(0, 0)] = 0.5f * (Field[IX(1, 0)] + Field[IX(0, 1)]);
	Field[IX(0, Size + 1)] = 0.5f * (Field[IX(1, Size + 1)] + Field[
		IX(0, Size)]);
	Field[IX(Size + 1, 0)] = 0.5f * (Field[IX(Size, 0)] + Field[IX(
		Size + 1, 1)]);
	Field[IX(Size + 1, Size + 1)] = 0.5f * (Field[IX(Size, Size + 1)] + Field[
		IX(Size + 1, Size)]);
}