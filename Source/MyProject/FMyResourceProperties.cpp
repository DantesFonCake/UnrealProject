//
// Created by aripo on 26.03.2022.
//

#include "FMyResourceProperties.h"

FMyResourceProperties::FMyResourceProperties():FlowSpeed(1),Courant(0.02),
Courant2(Courant*Courant),Kappa(0.0),Gamma(0.0)
,Kappa_Absorbing(5.0e-4),Gamma_Absorbing(1e-4)
{
}

FMyResourceProperties::FMyResourceProperties(float FlowSpeed,float Courant,float Kappa, float Gamma, float Kappa_Absorbing,float Gamma_Absorbing):
FlowSpeed(FlowSpeed),Courant(Courant),Courant2(Courant*Courant),Kappa(Kappa),Gamma(Gamma),Kappa_Absorbing(Kappa_Absorbing),Gamma_Absorbing(Gamma_Absorbing)
{
}
