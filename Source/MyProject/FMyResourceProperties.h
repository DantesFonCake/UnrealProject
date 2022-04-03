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
    FMyResourceProperties(float FlowSpeed,float Courant,float Kappa, float Gamma, float Kappa_Absorbing,float Gamma_Absorbing);

    UPROPERTY(BlueprintReadWrite,EditAnywhere)
    float FlowSpeed;
    UPROPERTY(BlueprintReadWrite,EditAnywhere)
    float Courant;
    UPROPERTY(BlueprintReadOnly)
    float Courant2;
    UPROPERTY(BlueprintReadWrite,EditAnywhere)
    float Kappa;
    UPROPERTY(BlueprintReadWrite,EditAnywhere)
    float Gamma;
    UPROPERTY(BlueprintReadWrite,EditAnywhere)
    float Kappa_Absorbing;
    UPROPERTY(BlueprintReadWrite,EditAnywhere)
    float Gamma_Absorbing;
};