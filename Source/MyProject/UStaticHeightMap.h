#pragma once
#include "FMyResourceProperties.h"
#include "UStaticHeightMap.generated.h"

UCLASS(Blueprintable)
class UStaticHeightMap : public UObject
{
	GENERATED_BODY()
protected:
	constexpr int GetFlatArraySize() const;
	UPROPERTY()
	TArray<float> D;
public:
	UStaticHeightMap()=default;
	UStaticHeightMap(int MapSize, FMyResourceProperties& Props);
	UFUNCTION(BlueprintCallable)
	virtual void Initialize(int NewMapSize, FMyResourceProperties NewProps);
	constexpr  int InlineCoords(int X, int Y) const;
	UFUNCTION(BlueprintCallable)
	virtual void InitializeLevel(float Level);
	UFUNCTION(BlueprintCallable)
	virtual void SetAreaResourceLevel(int X, int Y, int AreaWidth, int AreaHeight, float NewLevel);
	UFUNCTION(BlueprintCallable)
	virtual void AddAreaResourceLevel(int X, int Y, int AreaWidth, int AreaHeight, float AddLevel);
	UFUNCTION(BlueprintCallable)
	virtual void SetResourceLevel(int X, int Y, float NewLevel);
	UFUNCTION(BlueprintCallable)
	virtual void AddResourceLevel(int X, int Y, float AddLevel);
	UFUNCTION(BlueprintCallable)
	virtual float& At(int X, int Y);
	virtual const float& At(int X, int Y) const;
	UPROPERTY(BlueprintReadOnly)
	int MapSize;
	UPROPERTY(BlueprintReadOnly)
	FMyResourceProperties Props;
	
};

constexpr int UStaticHeightMap::GetFlatArraySize() const
{
	return  (MapSize+2)*(MapSize+2);
}
