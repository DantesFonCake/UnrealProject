#pragma once
#include "FMyResourceProperties.h"
#include "UStaticMap.generated.h"

UCLASS(Blueprintable)
class UStaticMap : public UObject
{
	GENERATED_BODY()
protected:
	constexpr int GetFlatArraySize() const;
	constexpr int InlineCoords(const int X, const int Y) const;
	UPROPERTY(BlueprintReadWrite)
	TArray<float> Amount;
public:
	UStaticMap()=default;
	UStaticMap(const int MapSize, const FMyResourceProperties& Props);
	UFUNCTION(BlueprintCallable)
	virtual void Initialize(int NewMapSize, FMyResourceProperties NewProps);
	UFUNCTION(BlueprintCallable)
	virtual void InitializeLevel(float Level);
	UFUNCTION(BlueprintCallable)
	virtual void SetAreaLevel(int X, int Y, int AreaWidth, int AreaHeight, float NewLevel);
	UFUNCTION(BlueprintCallable)
	virtual void AddAreaLevel(int X, int Y, int AreaWidth, int AreaHeight, float AddLevel);
	UFUNCTION(BlueprintCallable)
	virtual void SetLevel(int X, int Y, float NewLevel);
	UFUNCTION(BlueprintCallable)
	virtual void AddLevel(int X, int Y, float AddLevel);
	UFUNCTION(BlueprintCallable)
	virtual float& At(int X, int Y);
	UFUNCTION(BlueprintCallable)
	virtual const TArray<float>& GetData();
	virtual const float& At(int X, int Y) const;
	UPROPERTY(BlueprintReadOnly)
	int MapSize;
	UPROPERTY(BlueprintReadOnly)
	FMyResourceProperties Props;
	
};

constexpr int UStaticMap::GetFlatArraySize() const
{
	return  (MapSize+2)*(MapSize+2);
}

constexpr int UStaticMap::InlineCoords(const int X, const int Y) const
{
	return Y * (MapSize+2) + X;
}