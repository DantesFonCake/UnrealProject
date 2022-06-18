// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TimedLayersProccesor.h"
#include "SunInterface.h"
#include "TemperatureRelationC.generated.h"

UCLASS(Blueprintable)
class UTemperatureRelationC : public UObject, public ITimedLayersProccesor
{
	GENERATED_BODY()
	

protected:
	UPROPERTY()
	FName WaterName;
	UPROPERTY()
	FName TemperatureName;
	UPROPERTY()
	FName GroundName;
	UPROPERTY()
	FName DiffuseMapName;
	UPROPERTY(Instanced)
	UCurveFloat* MonthDayTemp;
	UPROPERTY(Instanced)
	UCurveFloat* MonthNightTemp;
	UPROPERTY(Instanced)
	UCurveFloat* MonthDay;
	UPROPERTY(Instanced)
	UCurveFloat* Evaporation;
	UPROPERTY()
	TScriptInterface<ISunInterface> Sun;
	
public:
	UFUNCTION(BlueprintCallable)
	void Initialize(FName water, FName temperature, FName ground, FName diffuseMap, UCurveFloat* mD, UCurveFloat* mDT, UCurveFloat* mNT, UCurveFloat* evap, UPARAM(ref)TScriptInterface<ISunInterface>& s);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Proccess(UResourceMapManager* Manager, float DeltaTime); virtual void Proccess_Implementation(UResourceMapManager* Manager, float DeltaTime) override;
};
