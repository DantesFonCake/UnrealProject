// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PreRenderLayersProccesor.generated.h"


UINTERFACE(BlueprintType)
class UPreRenderLayersProccesor : public UInterface
{
	GENERATED_BODY()
};

class IPreRenderLayersProccesor {
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Proccess(AResourceMapRendererPMC* renderer, UDrawableLayer* layer);
};
