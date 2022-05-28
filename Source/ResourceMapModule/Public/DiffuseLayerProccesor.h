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
	static inline void SetBound(int Size, TArray<float>& Field);
	static inline float PushDiff(int Size, const int x, const int y, const int x1, const int y1,
		const TArray<float>& Source,
		const TArray<float>& GroundLevel);
	static void Diffuse(int Size, float DeltaTime, float Diffuse, TArray<float>& X0, TArray<float>& X, const TArray<float>& GroundLevel, const TArray<float>& DiffuseMap);
	static void Advect(int Size, float DeltaTime, TArray<float>& X0, TArray<float>& X, const TArray<float>& GroundLevel, const TArray<float>& U, const TArray<float>& V);

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
