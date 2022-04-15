#include "UFluidHeightMap.h"
#include "solver.h"
void UFluidHeightMap::Rebalance(float DeltaTime)
{
	FluidStep(DeltaTime);
}

void UFluidHeightMap::PrepareSources()
{
	for (int i = 0; i < GetFlatArraySize(); ++i)
	{
		Future[i]=0;
		V0[i]=0;
		U0[i]=0;
	}
}

void UFluidHeightMap::FluidStep(float DeltaTime)
{
	PrepareSources();
	vel_step(MapSize,U.GetData(),V.GetData(),U0.GetData(),V0.GetData(),Props.Viscosity,DeltaTime);
	dens_step(MapSize, Amount.GetData(),Future.GetData(),U.GetData(),V.GetData(),Props.Diffuse,DeltaTime);
}

void UFluidHeightMap::Initialize(int NewMapSize, FMyResourceProperties NewProps)
{
	Super::Initialize(NewMapSize, NewProps);
	U.Init(0,GetFlatArraySize());
	V.Init(0,GetFlatArraySize());
	U0.Init(0,GetFlatArraySize());
	V0.Init(0,GetFlatArraySize());
}
