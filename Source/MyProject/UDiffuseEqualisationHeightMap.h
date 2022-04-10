#pragma once
#include "UStaticHeightMap.h"
#include "UDiffuseEqualisationHeightMap.generated.h"

UCLASS(Blueprintable)
class UDiffuseEqualisationHeightMap : public UStaticHeightMap
{
	GENERATED_BODY()
protected:
	UPROPERTY()
	TArray<float> D0;
public:
	UFUNCTION(BlueprintCallable)
	virtual void Rebalance(float DeltaTime); 
	virtual void Initialize(int NewMapSize, FMyResourceProperties NewProps) override;
};
