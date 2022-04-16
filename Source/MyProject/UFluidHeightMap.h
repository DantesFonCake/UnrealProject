#pragma once
#include "UDiffuseEqualisationHeightMap.h"
#include "UFluidHeightMap.generated.h"
UCLASS(Blueprintable)
class UFluidHeightMap : public UDiffuseEqualisationHeightMap
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadWrite)
	TArray<float> V;
	UPROPERTY(BlueprintReadWrite)
	TArray<float> U;
	UPROPERTY(BlueprintReadWrite)
	TArray<float> V0;
	UPROPERTY(BlueprintReadWrite)
	TArray<float> U0;
public:
	virtual void Rebalance(float DeltaTime, const TArray<float>& GroundLevel, const TArray<float>& DiffuseMap) override;
	virtual void PrepareSources();
	UFUNCTION(BlueprintCallable)
	virtual void FluidStep(float DeltaTime);
	virtual void Initialize(int NewMapSize, FMyResourceProperties NewProps) override;
};
