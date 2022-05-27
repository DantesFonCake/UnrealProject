// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RenderCommandFence.h"
class CUSTOMFLUIDSHADER_API CustomFluidShaderManager
{
public:
	//Get the instance
	static CustomFluidShaderManager* Get(int size)
	{
		if (!instance)
			instance = new CustomFluidShaderManager(size);
		return instance;
	};

private:
	//Private constructor to prevent client from instanciating
	CustomFluidShaderManager(int size) {
		lastResults.Init(0, (size+2)* (size + 2));
	};
	FRenderCommandFence ReleaseResourcesFence;
	//The singleton instance
	static CustomFluidShaderManager* instance;
	void DispatchInternal(FRHICommandListImmediate& cmd, const TArray<float>& x, const TArray<float>& x0, const TArray<float>& ground, const TArray<float>& diffuseMap, float diffuseProps,int size);

public:
	TArray<float> lastResults;
	void Dispatch(TArray<float>& x, const TArray<float>& x0, const TArray<float> ground, const TArray<float> diffuseMap, float diffuseProps,int size);
};
