
#ifndef _INCLUDE_MULTIRAYAIRICEREFRACTION_H_
#define _INCLUDE_MULTIRAYAIRICEREFRACTION_H_

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <vector>
#include <cstdlib>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_roots.h>
#include <gsl/gsl_deriv.h>
#include <gsl/gsl_fit.h>
#include <gsl/gsl_spline.h>
#include <sys/time.h>

using namespace std;

namespace MultiRayAirIceRefraction{
  
static constexpr double pi=4.0*atan(1.0); /**< Gives back value of Pi */
static constexpr double spedc=299792458.0; /**< Speed of Light in m/s */

////This Function reads in the values of ATMLAY and a,b and c parameters taken from the GDAS Atmosphere.dat file. The a,b and c values are mass overburden values and are not required in this code.
int readATMpar();

////This Function reads in the tavulated refractive index profile from the GDAS Atmosphere.dat file and fills in the nh_data, lognh_data and h_data vectors
int readnhFromFile();

////Set the value of the asymptotic parameter of the ice refractive index model
static constexpr double A_ice=1.78;

////Get the value of the B parameter for the ice refractive index model
double GetB_ice(double z);

////Get the value of the C parameter for the ice refractive index model
double GetC_ice(double z);

////Get the value of refractive index model for a given depth in ice
double Getnz_ice(double z);

////Set the value of the asymptotic parameter of the air refractive index model
static constexpr double A_air=1.00;

////Get the value of the B parameter for the air refractive index model
double GetB_air(double z);

////Get the value of the C parameter for the air refractive index model
double GetC_air(double z);

////Get the value of refractive index model for a given depth in air
double Getnz_air(double z);

////Use GSL minimiser which uses Brent's Method to find root for a given function
double FindFunctionRoot(gsl_function F,double x_lo, double x_hi);

/////Functions used for Raytracing in Ice using the analytical solution

////Analytical solution describing the ray path in ice
struct fDnfR_params { double a, b, c, l; };
double fDnfR(double x,void *params);

////Define the function that will be minimised to calculate the angle of reciept (from the vertical) on the antenna and the hit point of the ray on the ice surface given a ray incident angle
struct fdxdz_params { double lang, z0,z1; int airorice;};
double fdxdz(double x,void *params);

////The function used to calculate ray propogation time in ice
struct ftimeD_params { double a, b, c, speedc,l; int airorice; };
double ftimeD(double x,void *params);

////Get the distance on the 2ndLayer at which the ray should hit given an incident angle such that it hits an target at depth of z0 m in the second layer.
//// n_layer1 is the refractive index value of the previous layer at the boundary of the two mediums
//// A,B and C values are the values required for n(z)=A+B*exp(Cz) for the second layer
//// TxDepth is the starting height or depth
//// RxDepth is the final height or depth
//// AirOrIce variable is used to determine whether we are working in air or ice as that sets the range for the GSL root finder.
double *GetLayerHitPointPar(double n_layer1, double RxDepth,double TxDepth, double IncidentAng, int AirOrIce);

////This function flattens out 2d vectors into 1d vectors
vector<double> flatten(const vector<vector<double>>& v);

////This is the main function which will make the RayTracing Table that will be used for interpolation by COREAS
////The arguments are:
////1. AntennaDepth is depth of antenna in the ice and is given in m and is positive
////2. IceLayerHeight is the height in m in a.s.l where the ice layer starts off
int MakeRayTracingTable(double AntennaDepth, double IceLayerHeight);

}
#endif
