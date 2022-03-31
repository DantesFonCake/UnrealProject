//
// Created by aripo on 26.03.2022.
//
#pragma once
#include "FMyResourceProperties.generated.h"
USTRUCT(BlueprintType)
struct FMyResourceProperties
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite,EditAnywhere)
    float FlowSpeed;
    
};