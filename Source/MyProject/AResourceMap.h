// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FMyResourceProperties.h"
#include "GameFramework/Actor.h"
#include "AResourceMap.generated.h"

UCLASS(Blueprintable)
class MYPROJECT_API AResourceMap : public AActor
{
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	UPROPERTY()
	TArray<float> Data;
	UPROPERTY()
	TArray<float> DataSwap;
	
	virtual void BalanceSingleCell(int x, int y, float dT);
	constexpr int InlineCoords(int x, int y) const;
	

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;// Sets default values for this actor's properties
	AResourceMap();
	UPROPERTY(BlueprintReadWrite)
	int Width;
	UPROPERTY(BlueprintReadWrite)
	int Height;
	UPROPERTY(BlueprintReadWrite)
	FMyResourceProperties ResourceProperties;
	constexpr bool IsInRange(int x, int y) const;
	UFUNCTION(BlueprintCallable)
	virtual void InitializeLevel(float level);
	UFUNCTION(BlueprintCallable)
	virtual void SetAreaResourceLevel(int x, int y, int areaWidth, int areaHeight, float newLevel);
	UFUNCTION(BlueprintCallable)
	virtual void AddAreaResourceLevel(int x, int y, int areaWidth, int areaHeight, float addLevel);
	UFUNCTION(BlueprintCallable)
	virtual void SetResourceLevel(int x, int y, float newLevel);
	UFUNCTION(BlueprintCallable)
	virtual void AddResourceLevel(int x, int y, float addLevel);
	UFUNCTION(BlueprintCallable)
	virtual void Rebalance(float dT);
	virtual const float& At(int x, int y) const;
	UFUNCTION(BlueprintCallable)
	virtual float& At(int x, int y);

};
