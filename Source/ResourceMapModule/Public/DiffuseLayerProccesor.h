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
	static void PlainDiffuse(int Size, float* X, float* X0, float Diff, float DeltaTime);
	template<int BoundType>
	static void PlainLinSolve(int Size, float* X, float* X0, float a, float c);
	template<int BoundType>
	static inline void SetBound(const int Size, float* Field);
	static inline float PushDiff(const int Size, const int x, const int y, const int x1, const int y1,
		const float* Source,
		const float* GroundLevel);
	static void Project(int Size, float* U, float* V, float* P, float* Div);
	static void Diffuse(const int Size, const float DeltaTime, const float Diffuse, float* X0, float* X, const float* GroundLevel, const float* DiffuseMap);
	template<int BoundType>
	static void Advect(const int Size, const float DeltaTime, float* X0, float* X, const float* U, const float* V);
	static void VelocityStep(int Size, float* U, float* V, float* U0, float* V0, float Visc, float DeltaTime);

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
