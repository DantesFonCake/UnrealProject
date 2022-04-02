// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FMyResourceProperties.h"
#include "UResourceMap.generated.h"

UCLASS(Blueprintable)
class MYPROJECT_API UResourceMap : public UObject
{
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	UPROPERTY()
	TArray<float> Data;
	UPROPERTY()
	TArray<float> Delta;
	UPROPERTY(BlueprintReadWrite)
	bool IsInitialized;

	virtual void FillSingeCellDelta(int X, int Y, float DeltaTime);
	constexpr int InlineCoords(int X, int Y) const;
	

public:
	UFUNCTION(BlueprintCallable)
	void Initialize(int Width, int Height, FMyResourceProperties ResourceProperties);
	UResourceMap();
	UPROPERTY(BlueprintReadWrite)
	int Width;
	UPROPERTY(BlueprintReadWrite)
	int Height;
	UPROPERTY(BlueprintReadWrite)
	FMyResourceProperties ResourceProperties;
	constexpr bool IsInRange(int X, int Y) const;
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
	void ClearDelta();
	UFUNCTION(BlueprintCallable)
	virtual void Rebalance(float DeltaTime);
	virtual const float& At(int X, int Y) const;
	virtual const float& DeltaAt(int X, int Y) const;
	UFUNCTION(BlueprintCallable)
	virtual float& DeltaAt(int X, int Y);
	UFUNCTION(BlueprintCallable)
	virtual float& At(int X, int Y);

};
