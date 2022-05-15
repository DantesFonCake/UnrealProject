#pragma once
#include "FMyResourceProperties.h"
#include "UStaticMap.generated.h"

UCLASS(Blueprintable)
class UStaticMap : public UObject
{
	GENERATED_BODY()
protected:
	UFUNCTION(BlueprintCallable)
	inline int GetFlatArraySize() const;
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
	virtual void GetData(TArray<float>& OutData);
	UFUNCTION(BlueprintCallable)
	virtual float& FlatAt(int Index);
	UFUNCTION(BlueprintCallable)
	virtual void FlatSet(int Index, float Value);
	virtual const float& At(int X, int Y) const;
	UPROPERTY(BlueprintReadOnly)
	int MapSize;
	UPROPERTY(BlueprintReadOnly)
	FMyResourceProperties Props;
	UFUNCTION(BlueprintCallable)
	void UpdateBound(int BoundType) {
		for (int i = 1; i <= MapSize; i++)
		{
			Amount[InlineCoords(0, i)] = BoundType == 1 ? -Amount[InlineCoords(1, i)] : Amount[InlineCoords(1, i)];
			Amount[InlineCoords(MapSize + 1, i)] = BoundType == 1
				? -Amount[InlineCoords(MapSize, i)]
				: Amount[InlineCoords(MapSize, i)];
			Amount[InlineCoords(i, 0)] = BoundType == 2 ? -Amount[InlineCoords(i, 1)] : Amount[InlineCoords(i, 1)];
			Amount[InlineCoords(i, MapSize + 1)] = BoundType == 2
				? -Amount[InlineCoords(i, MapSize)]
				: Amount[InlineCoords(i, MapSize)];
		}
		Amount[InlineCoords(0, 0)] = 0.5f * (Amount[InlineCoords(1, 0)] + Amount[InlineCoords(0, 1)]);
		Amount[InlineCoords(0, MapSize + 1)] = 0.5f * (Amount[InlineCoords(1, MapSize + 1)] + Amount[
			InlineCoords(0, MapSize)]);
		Amount[InlineCoords(MapSize + 1, 0)] = 0.5f * (Amount[InlineCoords(MapSize, 0)] + Amount[InlineCoords(
			MapSize + 1, 1)]);
		Amount[InlineCoords(MapSize + 1, MapSize + 1)] = 0.5f * (Amount[InlineCoords(MapSize, MapSize + 1)] + Amount[
			InlineCoords(MapSize + 1, MapSize)]);
	}
	
};

inline int UStaticMap::GetFlatArraySize() const
{
	return  (MapSize+2)*(MapSize+2);
}

constexpr int UStaticMap::InlineCoords(const int X, const int Y) const
{
	return Y * (MapSize+2) + X;
}