//
// Created by aripo on 26.03.2022.
//

#include "FMyResourceProperties.h"

FMyResourceProperties::FMyResourceProperties():Diffuse(0.005),Viscosity(0.005)//,HeatCapacity(0)
{
}

FMyResourceProperties::FMyResourceProperties(float Diffuse, float Viscosity/*,float HeatCapacity*/):Diffuse(Diffuse),Viscosity(Viscosity)//,HeatCapacity(HeatCapacity)
{
}
