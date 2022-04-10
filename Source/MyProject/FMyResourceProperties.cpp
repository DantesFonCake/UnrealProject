//
// Created by aripo on 26.03.2022.
//

#include "FMyResourceProperties.h"

FMyResourceProperties::FMyResourceProperties():Diffuse(0.005),Viscosity(0.005)
{
}

FMyResourceProperties::FMyResourceProperties(float Diffuse, float Viscosity):Diffuse(Diffuse),Viscosity(Viscosity)
{
}
