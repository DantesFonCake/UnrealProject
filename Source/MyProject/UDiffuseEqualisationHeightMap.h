#pragma once
#include "UStaticMap.h"
#include "UDiffuseEqualisationHeightMap.generated.h"

UCLASS(Blueprintable)
class UDiffuseEqualisationHeightMap : public UStaticMap
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadWrite)
	TArray<float> Future;
	inline float PushDiff(int x, int y, int x1, int y1, const TArray<float>& Source,
													   const TArray<float>& GroundLevel) const;
	virtual void Diffuse(float DeltaTime, TArray<float>& X0, TArray<float>& X, const TArray<float>& GroundLevel);
	void Diffuse(float DeltaTime, TArray<float>& X0, TArray<float>& X, const TArray<float>& GroundLevel, const TArray<float>& DiffuseMap);
	inline void SetBound(int BoundType, TArray<float>& Field) const;
public:
	UFUNCTION(BlueprintCallable)
	virtual void Rebalance(float DeltaTime, const TArray<float>& GroundLevel, const TArray<float>& DiffuseMap);
	virtual void Initialize(int NewMapSize, FMyResourceProperties NewProps) override;
};
