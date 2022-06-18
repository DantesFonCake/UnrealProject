// Fill out your copyright notice in the Description page of Project Settings.

#include "TemperatureRelationC.h"
#include "ResourceMapModule.h"

DECLARE_CYCLE_STAT(TEXT("Temperature_Proccessor"),STAT_DIFFUSE,STATGROUP_RMM)

#define FLAT_SIZE(s) ((s)+2)*((s)+2)
void UTemperatureRelationC::Initialize(FName water, FName temperature, FName ground, FName diffuseMap, UCurveFloat* monthDay, UCurveFloat* monthDayTemp, UCurveFloat* monthNightTemp, UCurveFloat* evap, TScriptInterface<ISunInterface>& s)
{
	WaterName = water;
	TemperatureName = temperature;
	GroundName = ground;
	DiffuseMapName = diffuseMap;
	MonthDay = monthDay;
	MonthDayTemp = monthDayTemp;
	MonthNightTemp = monthNightTemp;
	Evaporation = evap;
	Sun = s;
}

void UTemperatureRelationC::Proccess_Implementation(UResourceMapManager* Manager, float DeltaTime)
{
	auto size = Manager->GetSize();
	auto ground = Manager->GetNamedStaticLayer(GroundName);
	auto water = Manager->GetNamedStaticLayer(WaterName);
	auto temperature = Manager->GetNamedStaticLayer(TemperatureName);
	auto diffuseMap = Manager->GetNamedStaticLayer(DiffuseMapName);
	int month, day;
	Sun->Execute_GetDate(Sun.GetObject(), month, day);
	auto daysInMonth = MonthDay->GetFloatValue(month);
	float dateF = month + day / daysInMonth;
	auto maxTemp = MonthDayTemp->GetFloatValue(dateF);
	auto minTemp = MonthNightTemp->GetFloatValue(dateF);

	float cosine;
	Sun->Execute_GetCosine(Sun.GetObject(), cosine);


	auto temp = FMath::Lerp(minTemp, maxTemp, FMath::Clamp<float>(cosine, 0, 1));
	//UE_LOG(LogTemp, Warning, TEXT("%d %d %f %f %f %f"), day, month, cosine, temp, maxTemp, minTemp);

	for (int i = 0; i < FLAT_SIZE(size); i++)
	{
		auto g = (ground->FlatGetAmount(i) - 100)*0.006f;

		auto realTemp = temp - g;
		temperature->FlatSetAmount(i, realTemp);
		auto evap = Evaporation->GetFloatValue(realTemp)*DeltaTime/60000;
		auto w = water->FlatGetAmount(i);
		w = FMath::Clamp<float>(w - evap, 0, w);
		water->FlatSetAmount(i, w);

		diffuseMap->FlatSetAmount(i, realTemp >= 0 ? 0 : 1);
	}
}
