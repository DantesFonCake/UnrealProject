// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PreRenderLayersProccesor.h"
#include "WaterPreRenderProccesor.generated.h"

UCLASS(Blueprintable)
class UWaterPreRenderProccesor : public UObject, public IPreRenderLayersProccesor
{
	GENERATED_BODY()


protected:
	UPROPERTY()
	FName TemperatureLayerName;
public:
	static const float WaterFreezingTemperature;
	UWaterPreRenderProccesor(){}
	UFUNCTION(BlueprintCallable)
	void Initialize(FName temperatureLayer) {
		TemperatureLayerName = temperatureLayer;
	}
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Proccess(AResourceMapRendererISMC* Renderer, UDrawableLayer* layer); virtual void Proccess_Implementation(AResourceMapRendererISMC* Renderer, UDrawableLayer* layer) override;
};
