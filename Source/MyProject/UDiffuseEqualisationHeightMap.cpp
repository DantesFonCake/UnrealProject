#include "UDiffuseEqualisationHeightMap.h"
#include "solver.h"

void UDiffuseEqualisationHeightMap::Rebalance(float DeltaTime)
{
	diffuse(MapSize,0, D.GetData(),D0.GetData(), Props.Diffuse,DeltaTime);
}

void UDiffuseEqualisationHeightMap::Initialize(int NewMapSize, FMyResourceProperties NewProps)
{
	Super::Initialize(NewMapSize, NewProps);
	D0.Init(0,GetFlatArraySize());
}
