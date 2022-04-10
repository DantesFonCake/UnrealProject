#pragma once
#include "UDiffuseEqualisationHeightMap.h"
#include "UFluidHeightMap.generated.h"
UCLASS(Blueprintable)
class UFluidHeightMap : public UDiffuseEqualisationHeightMap
{
	GENERATED_BODY()
protected:
	UPROPERTY()
	TArray<float> V;
	UPROPERTY()
	TArray<float> U;
	UPROPERTY()
	TArray<float> V0;
	UPROPERTY()
	TArray<float> U0;
public:
	virtual void Rebalance(float DeltaTime) override;
	virtual void Initialize(int NewMapSize, FMyResourceProperties NewProps) override;
};
