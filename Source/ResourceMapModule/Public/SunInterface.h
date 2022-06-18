// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SunInterface.generated.h"

UINTERFACE(BlueprintType)
class USunInterface : public UInterface
{
	GENERATED_BODY()
};

class ISunInterface {
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void GetCosine(float& Cosine);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void GetDate(int& Month, int& Day);
};