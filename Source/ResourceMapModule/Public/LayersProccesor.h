// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ResourceMapManager.h"

class LayersProccesor
{
public:
	virtual void Proccess(UResourceMapManager* manager) = 0;
};
