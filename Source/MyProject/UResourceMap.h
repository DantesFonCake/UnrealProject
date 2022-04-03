// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BoundaryCondition.h"
#include "FMyResourceProperties.h"
#include "UResourceMap.generated.h"

UCLASS(Blueprintable)
class MYPROJECT_API UResourceMap : public UObject
{
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	UPROPERTY()
	TArray<float> Phi;
	UPROPERTY()
	TArray<float> Psi;
	UPROPERTY()
	TArray<float> Phi2;
	UPROPERTY()
	TArray<float> Psi2;
	UPROPERTY(BlueprintReadWrite)
	bool IsInitialized;
	UPROPERTY(BlueprintReadWrite)
	bool bOscillateLeft;
	UPROPERTY(BlueprintReadWrite)
	float OscillationAmplitude;
	UPROPERTY(BlueprintReadWrite)
	float OscillationFrequency;
	UPROPERTY(BlueprintReadWrite)
	TEnumAsByte<BoundaryCondition> Boundary;
	UPROPERTY(BlueprintReadWrite)
	bool bUseResourceMax;
	UPROPERTY(BlueprintReadWrite)
	float ResourceMax;

	virtual void FillSingeCellDelta(int X, int Y, float DeltaTime);
	constexpr int InlineCoords(int X, int Y) const;
	

public:
	UFUNCTION(BlueprintCallable)
	void Initialize(const int NewWidth, const int NewHeight,
							  const FMyResourceProperties NewResourceProperties,
							  const bool IsOscillateLeft, const float NewOscillationAmplitude,
							  const BoundaryCondition NewBoundaryCondition, const bool IsUsingResourceMax,
							  const float NewResourceMax,const float NewOscillationFrequency);
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
	virtual void EqualDistributionRebalance(float DeltaTime);
	UFUNCTION(BlueprintCallable)
	virtual void WaveRebalance(float DeltaTime);
	void evolve_wave_half(TArray<float>& phi_in, TArray<float>& psi_in, TArray<float>& phi_out, TArray<float>& psi_out);
	virtual const float& At(int X, int Y) const;
	virtual const float& PsiAt(int X, int Y) const;
	UFUNCTION(BlueprintCallable)
	virtual float& PsiAt(int X, int Y);
	UFUNCTION(BlueprintCallable)
	virtual float& At(int X, int Y);

};
