// Fill out your copyright notice in the Description page of Project Settings.

#include "WaterPreRenderProccesor.h"
#include "ResourceMapRendererISMC.h"

#define IX_INTERNAL(x,y) ((y) * (Size) + (x))
const float UWaterPreRenderProccesor::WaterFreezingTemperature=0;
void UWaterPreRenderProccesor::Proccess_Implementation(AResourceMapRendererISMC * Renderer, UDrawableLayer* layer)
{
	auto temperature = Renderer->GetManager()->GetNamedStaticLayer(TemperatureLayerName);
	auto mesh = layer->GetMesh();
	auto Size = layer->GetSize();

#pragma omp parralel for
	for (int y = 0; y < Size; y++)
	{
#pragma omp parralel for
		for (int x = 0; x < Size; x++)
		{
			if (temperature->GetAmount(x + 1, y + 1) <= WaterFreezingTemperature)
				mesh->PerInstanceSMCustomData[IX_INTERNAL(x, y)] = 1;
			else
				mesh->PerInstanceSMCustomData[IX_INTERNAL(x, y)] = 0;
		}
	}

}
