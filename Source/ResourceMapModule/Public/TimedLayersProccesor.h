// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TimedLayersProccesor.generated.h"

UINTERFACE(BlueprintType)
class UTimedLayersProccesor : public UInterface
{
	GENERATED_BODY()
};

class ITimedLayersProccesor {
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Proccess(UResourceMapManager* Manager, float DeltaTime);
};