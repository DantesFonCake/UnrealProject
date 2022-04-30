//
// Created by aripo on 26.03.2022.
//
#pragma once
#include "FMyResourceProperties.generated.h"
USTRUCT(BlueprintType)
struct FMyResourceProperties
{
    GENERATED_BODY()
    FMyResourceProperties();
    FMyResourceProperties(float Diffuse,float Viscosity/*,float HeatCapacity*/);

    UPROPERTY(BlueprintReadWrite,EditAnywhere)
    float Diffuse;
    UPROPERTY(BlueprintReadWrite,EditAnywhere)
    float Viscosity;
    //UPROPERTY(BlueprintReadWrite,EditAnywhere)
    //float HeatCapacity;
};