#pragma once
#include "UStaticHeightMap.h"
#include "UDiffuseEqualisationHeightMap.generated.h"

UCLASS(Blueprintable)
class UDiffuseEqualisationHeightMap : public UStaticHeightMap
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadWrite)
	TArray<float> Future;
	float Difference(int x, int y, int x1, int y1, const TArray<float>& Source, const TArray<float>& GroundLevel) const;
	void Pull(int x, int y, int x1, int y1, float d);
	inline float PushDiff(int x, int y, int x1, int y1, const TArray<float>& Source,
													   const TArray<float>& GroundLevel) const;
public:
	UFUNCTION(BlueprintCallable)
	virtual void Rebalance(float DeltaTime);
	UFUNCTION(BlueprintCallable)
	virtual void RebalanceWithGround(float DeltaTime, const TArray<float>& GroundLevel);
	UFUNCTION(BlueprintCallable)
	virtual void Diffuse(float DeltaTime);
	UFUNCTION(BlueprintCallable)
	virtual void DiffuseWithGround(float DeltaTime, const TArray<float>& GroundLevel);
	virtual void Initialize(int NewMapSize, FMyResourceProperties NewProps) override;
};
