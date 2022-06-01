// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TimedLayersProccesor.h"
#include "DiffuseLayerProccesor.generated.h"

UCLASS(Blueprintable)
class UDiffuseLayerProccesor : public UObject, public ITimedLayersProccesor
{
	GENERATED_BODY()
	template<int BoundType>
	static inline void SetBound(const int Size, float* Field);
	static inline float PushDiff(const int Size, const int x, const int y, const int x1, const int y1,
		const float* Source,
		const float* GroundLevel);
	static void Diffuse(const int Size, const float DeltaTime, const float Diffuse, float* X0, float* X, const float* GroundLevel, const float* DiffuseMap);
	static void Advect(const int Size, const float DeltaTime, float* X0, float* X, const float* GroundLevel, const float* U, const float* V);/*	template<int BoundType>
	static inline void SetBound(const int Size, TArray<float>& Field);
	static inline float PushDiff(const int Size, const int x, const int y, const int x1, const int y1,
		const TArray<float>& Source,
		const TArray<float>& GroundLevel);
	static void Diffuse(const int Size, const float DeltaTime, const float Diffuse, TArray<float>& X0, TArray<float>& X, const TArray<float>& GroundLevel, const TArray<float>& DiffuseMap);
	static void Advect(const int Size, const float DeltaTime, TArray<float>& X0, TArray<float>& X, const TArray<float>& GroundLevel, const TArray<float>& U, const TArray<float>& V);*/

protected:
	UPROPERTY()
	FName LayerToProcces;
public:
	UDiffuseLayerProccesor(){}
	UFUNCTION(BlueprintCallable)
	void Initialize(FName layerName) {
		LayerToProcces = layerName;
	}
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Proccess(UResourceMapManager* Manager, float DeltaTime); virtual void Proccess_Implementation(UResourceMapManager* Manager, float DeltaTime) override;
};
