#include "MultiRayAirIceRefraction.h"

////Define vectors to store data from the file
vector <vector <double>> nh_data;////n(h) refractive index profile of the atmosphere as a function of height
vector <vector <double>> lognh_data;////log(n(h)-1) log of the refractive index profile of the atmosphere as a function of height subtracted by 1
vector <vector <double>> h_data;////height data

////Define Arrays for storing values of ATMLAY and a,b and c parameters taken from the Atmosphere.dat file
double ATMLAY[5];
double abc[5][3];

////define dummy variables which will be filled in later after fitting
double B_air=0;
double C_air=0;

////This Function reads in the values of ATMLAY and a,b and c parameters taken from the Atmosphere.dat file. The a,b and c values are mass overburden values and are not required in this code.
int MultiRayAirIceRefraction::readATMpar(){

  ////Open the file
  ifstream ain("Atmosphere.dat");
  
  int n1=0;////variable for counting total number of data points
  string line;
  double dummya[5]={0,0,0,0,0};////temporary variable for storing data values from the file
  
  //Check if file is open and store data
  if(ain.is_open()){
    
    while (getline(ain,line)){

      if(n1<4){////only read in the lines which contain the ATMLAY and a,b and c values in the file
	ain>>dummya[0]>>dummya[1]>>dummya[2]>>dummya[3]>>dummya[4];
	//cout<<n1<<" "<<dummya[0]<<" , "<<dummya[1]<<" , "<<dummya[2]<<" , "<<dummya[3]<<" , "<<dummya[4]<<endl;
      }

      ////Store the values in their respective arrays
      if(n1==0){
	for (int i=0; i<5; i++){ ATMLAY[i]=dummya[i]; }
      }    
      if(n1==1){
	for (int i=0; i<5; i++){ abc[i][0]=dummya[i]; }
      }
      if(n1==2){
	for (int i=0; i<5; i++){ abc[i][1]=dummya[i]; }
      }
      if(n1==3){
	for (int i=0; i<5; i++){ abc[i][2]=dummya[i]; }
      }
      n1++;
    }////end the while loop
    
    ain.close();
  }////if condition to check if file is open

  return 0;
}

int MultiRayAirIceRefraction::readnhFromFile(){

  ////Open the file
  ifstream ain("Atmosphere.dat");
  ain.precision(10); 

  int n1=0;////variable for counting total number of data points
  int layer=0;
  string line;

  ////Ignore the lines containing ATMLAY and a,b and c values.
  for(int i=0; i<5; i++){ ain.ignore(256,'\n'); }
  
  ////Check if file is open and store data
  if(ain.is_open()){
    ////define dummy/temporary variables for storing data
    double dummy1,dummy2;
    ////define dummy/temporary vectors for storing data.
    vector <double> temp1,temp2,temp3;
    
    while (getline(ain,line)){
      ain>>dummy1>>dummy2;
      
      if(dummy1>=0){////start storing height at above and equal to 0 m
	////push in the height values for a single layer in the temporary vector
	temp1.push_back(dummy1);
	temp2.push_back(dummy2);
	temp3.push_back(log(dummy2-1));
	
	if(dummy1*100>=ATMLAY[layer]){////change the layer once the data of all the heights of that layer has been read in
	  if(layer>0){////now since the layer has finished and the temporary vectors have been filled in. Now we push the vectors in the main 2d height and refractice index vectors
	    h_data.push_back(temp1);
	    nh_data.push_back(temp2);
	    lognh_data.push_back(temp3);

	    ////clear the vectors now for storing the next layer
	    temp1.clear();
	    temp2.clear();
	    temp3.clear();
	  } 
	  layer++;
	}
	n1++;
      } 
    }////end the while loop
    
    if(layer>0){////For storing the last layer
      h_data.push_back(temp1);
      nh_data.push_back(temp2);
      lognh_data.push_back(temp3);
      ////clear the vectors now for storing the next layer
      temp1.clear();
      temp2.clear();
      temp3.clear();
    }
    layer++;
    
    ain.close();
  }////if condition to check if file is open

  ////The file reading condition "while (getline(ain,line))" reads the last the datapoint of the file twice. This is to to remove the last repeat data point in all the data arrays
  h_data[h_data.size()-1].erase(h_data[h_data.size()-1].end() - 1);
  nh_data[nh_data.size()-1].erase(nh_data[nh_data.size()-1].end() - 1);
  lognh_data[lognh_data.size()-1].erase(lognh_data[lognh_data.size()-1].end() - 1);
  
  return 0;
}

////Get the value of the B parameter for the ice refractive index model
double MultiRayAirIceRefraction::GetB_ice(double z){
  //double zabs=fabs(z);
  double B=0;

  B=-0.43;
  return B;
}

////Get the value of the C parameter for the ice refractive index model
double MultiRayAirIceRefraction::GetC_ice(double z){
  //double zabs=fabs(z);
  double C=0;
  
  C=0.0132;
  return C;
}

////Get the value of refractive index model for a given depth in ice
double MultiRayAirIceRefraction::Getnz_ice(double z){
  double zabs=fabs(z);
  return MultiRayAirIceRefraction::A_ice+GetB_ice(zabs)*exp(-GetC_ice(zabs)*zabs);
}

////Get the value of the B parameter for the air refractive index model
double MultiRayAirIceRefraction::GetB_air(double z){
  //double zabs=fabs(z);
  double B=0;
  
  B=B_air;
  return B;
}

////Get the value of the C parameter for the air refractive index model
double MultiRayAirIceRefraction::GetC_air(double z){
  //double zabs=fabs(z);
  double C=0;

  C=C_air;
  return C;
}

////Get the value of refractive index model for a given depth in air
double MultiRayAirIceRefraction::Getnz_air(double z){
  double zabs=fabs(z);
  return MultiRayAirIceRefraction::A_air+GetB_air(zabs)*exp(-GetC_air(zabs)*zabs);
}

////Use GSL minimiser which uses Brent's Method to find root for a given function
double MultiRayAirIceRefraction::FindFunctionRoot(gsl_function F,double x_lo, double x_hi)
{
  int status;
  int iter = 0, max_iter = 100;
  const gsl_root_fsolver_type *T;
  gsl_root_fsolver *s;
  double r = 0;

  T = gsl_root_fsolver_brent;
  s = gsl_root_fsolver_alloc (T);
  gsl_set_error_handler_off();
  gsl_root_fsolver_set (s, &F, x_lo, x_hi);

  //printf ("using %s method\n", gsl_root_fsolver_name (s));
  //printf ("%5s [%9s, %9s] %9s %9s\n","iter", "lower", "upper", "root", "err(est)");

  do
    {
      iter++;
      status = gsl_root_fsolver_iterate (s);
      r = gsl_root_fsolver_root (s);
      x_lo = gsl_root_fsolver_x_lower (s);
      x_hi = gsl_root_fsolver_x_upper (s);
      status = gsl_root_test_interval (x_lo, x_hi,0, 0.0001);

      if (status == GSL_SUCCESS){
	//printf ("Converged:");
	//printf ("%5d [%.7f, %.7f] %.7f %.7f\n",iter, x_lo, x_hi,r,x_hi - x_lo);
      }
    }
  while (status == GSL_CONTINUE && iter < max_iter);

  gsl_root_fsolver_free (s);

  return r;
}

/////Functions used for Raytracing in Ice using the analytical solution

////Analytical solution describing the ray path in ice
double MultiRayAirIceRefraction::fDnfR(double x,void *params){
  
  struct MultiRayAirIceRefraction::fDnfR_params *p= (struct MultiRayAirIceRefraction::fDnfR_params *) params;
  double A = p->a;
  double B = p->b;
  double C = p->c;
  double L = p->l;
  
  return (L/C)*(1.0/sqrt(A*A-L*L))*(C*x-log(A*(A+B*exp(C*x))-L*L+sqrt(A*A-L*L)*sqrt(pow(A+B*exp(C*x),2)-L*L)));;
}

////Define the function that will be minimised to calculate the angle of reciept (from the vertical) on the antenna and the hit point of the ray on the ice surface given a ray incident angle
double MultiRayAirIceRefraction::fdxdz(double x,void *params){
  
  struct MultiRayAirIceRefraction::fdxdz_params *p= (struct MultiRayAirIceRefraction::fdxdz_params *) params;
  double Lang = p->lang;
  double Z0 = p->z0;
  double Z1 = p->z1;
  int AirOrIce = p->airorice;
  
   double output=0;
  if(AirOrIce==0){
    output=tan(asin( (Getnz_ice(Z0)*sin(x))/Getnz_ice(Z1) ) ) - tan(Lang);
  }
  
  if(AirOrIce==1){
    output=tan(asin( (Getnz_air(Z0)*sin(x))/Getnz_air(Z1) ) ) - tan(Lang);
  }
  return output;
}

////The function used to calculate ray propogation time in ice
double MultiRayAirIceRefraction::ftimeD(double x,void *params){
  
  struct MultiRayAirIceRefraction::ftimeD_params *p= (struct MultiRayAirIceRefraction::ftimeD_params *) params;
  double A = p->a;
  //double B = p->b;
  double C = p->c;
  double Speedc = p->speedc;
  double L = p->l;
  int AirOrIce=p->airorice;

  double result=0;
  if(AirOrIce==0){//in ice
    result=(1.0/(Speedc*C*sqrt(pow(Getnz_ice(x),2)-L*L)))*(pow(Getnz_ice(x),2)-L*L+(C*x-log(A*Getnz_ice(x)-L*L+sqrt(A*A-L*L)*sqrt(pow(Getnz_ice(x),2)-L*L)))*(A*A*sqrt(pow(Getnz_ice(x),2)-L*L))/sqrt(A*A-L*L) +A*sqrt(pow(Getnz_ice(x),2)-L*L)*log(Getnz_ice(x)+sqrt(pow(Getnz_ice(x),2)-L*L)) );
  }
  if(AirOrIce==1){//in air
    result=(1.0/(Speedc*C*sqrt(pow(Getnz_air(x),2)-L*L)))*(pow(Getnz_air(x),2)-L*L+(C*x-log(A*Getnz_air(x)-L*L+sqrt(A*A-L*L)*sqrt(pow(Getnz_air(x),2)-L*L)))*(A*A*sqrt(pow(Getnz_air(x),2)-L*L))/sqrt(A*A-L*L) +A*sqrt(pow(Getnz_air(x),2)-L*L)*log(Getnz_air(x)+sqrt(pow(Getnz_air(x),2)-L*L)) );
  }
  
  return result;
  
}

////Get the distance on the 2ndLayer at which the ray should hit given an incident angle such that it hits an target at depth of z0 m in the second layer.
//// n_layer1 is the refractive index value of the previous layer at the boundary of the two mediums
//// A,B and C values are the values required for n(z)=A+B*exp(Cz) for the second layer
//// TxDepth is the starting height or depth
//// RxDepth is the final height or depth
//// AirOrIce variable is used to determine whether we are working in air or ice as that sets the range for the GSL root finder.
double *MultiRayAirIceRefraction::GetLayerHitPointPar(double n_layer1, double RxDepth,double TxDepth, double IncidentAng, int AirOrIce){

  double *output=new double[4];
  
  //double x0=0;////Starting horizontal point of the ray. Always set at zero
  double x1=0;////Variable to store the horizontal distance that will be traveled by the ray
  
  double ReceiveAngle=0;////Angle from the vertical at which the target will recieve the ray
  double Lvalue=0;//// L parameter of the ray for that layer
  double RayTimeIn2ndLayer=0;////Time of propagation in 2ndLayer 
  //double AngleOfEntryIn2ndLayer=0;////Angle at which the ray enters the layer

  double SurfaceRayIncidentAngle=IncidentAng*(MultiRayAirIceRefraction::pi/180.0);////Angle at which the ray is incident on the second layer
  double RayAngleInside2ndLayer=0;////Use Snell's Law to find the angle of transmission in the 2ndlayer

  double A=0;
  double nzRx=0;
  double nzTx=0;
  double GSLFnLimit=0;

  if(AirOrIce==0){
    //cout<<"in ice"<<endl;
    A=MultiRayAirIceRefraction::A_ice;
    nzRx=Getnz_ice(RxDepth);
    nzTx=Getnz_ice(TxDepth);
  }
  if(AirOrIce==1){
    //cout<<"in air"<<endl;
    A=MultiRayAirIceRefraction::A_air;
    nzRx=Getnz_air(RxDepth);
    nzTx=Getnz_air(TxDepth);
  }

  ////LimitAngle sets a limit on the range to which the GSL minimisation will work. This limit comes from the fact that in fdxdx() you have tan(asin(x)) which goes to infinity at x=1. In our case x=(nz(Z0)*sin(Angle))/nz(Z1) . Solving for Angle gives us our limit.
  double LimitAngle=asin(nzTx/nzRx);
  
  GSLFnLimit=LimitAngle;
  RayAngleInside2ndLayer=asin((n_layer1/nzTx)*sin(SurfaceRayIncidentAngle));////Use Snell's Law to find the angle of transmission in the 2ndlayer
  
  ////calculate the angle at which the target receives the ray
  gsl_function F1;
  struct MultiRayAirIceRefraction::fdxdz_params params1 = {RayAngleInside2ndLayer, RxDepth, TxDepth, AirOrIce};
  F1.function = &fdxdz;
  F1.params = &params1;
  ReceiveAngle=FindFunctionRoot(F1,0*(MultiRayAirIceRefraction::pi/180),GSLFnLimit);
  //cout<<"The angle from vertical at which the target recieves the ray is "<<ReceiveAngle*(180/MultiRayAirIceRefraction::pi)<<" deg"<<endl;
  
  ////calculate the distance of the point of incidence on the 2ndLayer surface and also the value of the L parameter of the solution
  Lvalue=nzRx*sin(ReceiveAngle);
  struct MultiRayAirIceRefraction::fDnfR_params params2a;
  struct MultiRayAirIceRefraction::fDnfR_params params2b;
  if(AirOrIce==0){
    //cout<<"in ice"<<endl;
    params2a = {A, GetB_ice(RxDepth), -GetC_ice(RxDepth), Lvalue};
    params2b = {A, GetB_ice(TxDepth), -GetC_ice(TxDepth), Lvalue};
  }
  if(AirOrIce==1){
    //cout<<"in air"<<endl;
    params2a = {A, GetB_air(RxDepth), -GetC_air(RxDepth), Lvalue};
    params2b = {A, GetB_air(TxDepth), -GetC_air(TxDepth), Lvalue};
  }
  x1=+fDnfR(RxDepth,&params2a)-fDnfR(TxDepth,&params2b);
  if(AirOrIce==1){
    x1*=-1;
  }
  
  //cout<<"The hit point horizontal distance is from the Rx target "<<x1<<" m  on the surface"<<endl;
  
  ////calculate the propagation time in 2ndLayer
  struct MultiRayAirIceRefraction::ftimeD_params params3a;
  struct MultiRayAirIceRefraction::ftimeD_params params3b;
  if(AirOrIce==0){
    //cout<<"in ice"<<endl;
    params3a = {A, GetB_ice(RxDepth), -GetC_ice(RxDepth), MultiRayAirIceRefraction::spedc, Lvalue,0};
    params3b = {A, GetB_ice(TxDepth), -GetC_ice(TxDepth), MultiRayAirIceRefraction::spedc, Lvalue,0};
  }
  if(AirOrIce==1){
    //cout<<"in air"<<endl;
    params3a = {A, GetB_air(RxDepth), -GetC_air(RxDepth), MultiRayAirIceRefraction::spedc, Lvalue,1};
    params3b = {A, GetB_air(TxDepth), -GetC_air(TxDepth), MultiRayAirIceRefraction::spedc, Lvalue,1};
  }
  RayTimeIn2ndLayer=+ftimeD(RxDepth,&params3a)-ftimeD(TxDepth,&params3b);
  if(AirOrIce==1){
    RayTimeIn2ndLayer*=-1;
  }
  //cout<<"The propagation time in 2ndLayer is: "<<RayTimeIn2ndLayer<<" s"<<endl;

  ///////calculate the initial angle when the ray enters the 2ndLayer. This should be the same as RayAngleInside2ndLayer. This provides a good sanity check to make sure things have worked out.
  // gsl_function F4;
  // double result, abserr;
  // F4.function = &MultiRayAirIceRefraction::fDnfR;
  // F4.params = &params2b;
  // gsl_deriv_central (&F4, TxDepth, 1e-8, &result, &abserr);
  // AngleOfEntryIn2ndLayer=atan(result)*(180.0/MultiRayAirIceRefraction::pi);
  // if(TxDepth==RxDepth && TMath::IsNaN(AngleOfEntryIn2ndLayer)==true){
  //   AngleOfEntryIn2ndLayer=180-ReceiveAngle;
  // }
  // if(TxDepth!=RxDepth && TMath::IsNaN(AngleOfEntryIn2ndLayer)==true){
  //   AngleOfEntryIn2ndLayer=90;
  // }
  //cout<<"AngleOfEntryIn2ndLayer= "<<AngleOfEntryIn2ndLayer<<" ,RayAngleInside2ndLayer="<<RayAngleInside2ndLayer*(180/MultiRayAirIceRefraction::pi)<<endl;

  output[0]=x1;
  output[1]=ReceiveAngle*(180/MultiRayAirIceRefraction::pi);
  output[2]=Lvalue;
  output[3]=RayTimeIn2ndLayer;
  
  return output;
}

////This function flattens out 2d vectors into 1d vectors
vector<double> MultiRayAirIceRefraction::flatten(const vector<vector<double>>& v) {
    size_t total_size = 0;
    for (const auto& sub : v)
        total_size += sub.size();
    vector<double> result;
    result.reserve(total_size);
    for (const auto& sub : v)
        result.insert(result.end(), sub.begin(), sub.end());
    return result;
}

////This function is used to measure the amount of time the code takes
typedef unsigned long long timestamp_t;
static timestamp_t get_timestamp (){
  struct timeval now;
  gettimeofday (&now, NULL);
  return  now.tv_usec + (timestamp_t)now.tv_sec * 1000000;
}

int MultiRayAirIceRefraction::MakeRayTracingTable(double AntennaDepth, double IceLayerHeight){

  ////For recording how much time the process took
  timestamp_t t0 = get_timestamp();
  
  ////Print out the entry number, the Tx height, ice layer height, Tx height above the icelayer height, total horizontal distance on surface, total horizontal distance in ice, RayLaunchAngle at Tx, incident angle on ice and recievd angle in ice at the antenna inside this file 
  ofstream aout("TableValues.txt");
  
  ////Fill in the n(h) and h arrays and ATMLAY and a,b and c (these 3 are the mass overburden parameters) from the data file
  readATMpar();
  readnhFromFile();
  int MaxLayers=h_data.size();////store the total number of layers present in the data
  
  ////Flatten out the height and the refractive index vectors to be used for setting the up the spline interpolation.
  vector <double> flattened_h_data=flatten(h_data);
  vector <double> flattened_nh_data=flatten(nh_data);

  ////Set up the GSL cubic spline interpolation. This used for interpolating values of refractive index at different heights.
  gsl_interp_accel * accelerator =  gsl_interp_accel_alloc();
  gsl_spline *spline = gsl_spline_alloc (gsl_interp_cspline,flattened_h_data.size());
  gsl_spline_init(spline, flattened_h_data.data(), flattened_nh_data.data(), flattened_h_data.size());

  ////Define variables for the loop over Tx height and ray launch angle
  double RayLaunchAngleInAir=0;////Set zero for now and 0 deg straight up. This variable defines the initial launch angle of the ray w.r.t to the vertical in the atmosphere. 0 deg is straight up
  double TxHeight=h_data[h_data.size()-1][h_data[h_data.size()-1].size()-1];////Maximum height available with the refractive index data

  ////Set the variables for the for loop that will loop over the launch angle values. All values are in degrees
  double AngleStepSize=1;
  double LoopStartAngle=91;
  double LoopStopAngle=179;
  int TotalAngleSteps=floor((LoopStopAngle-LoopStartAngle)/AngleStepSize);

  ////Set the variables for the for loop that will loop over the Tx height values above the ice layer. All values are in degrees
  double HeightStepSize=20;
  double LoopStartHeight=TxHeight;
  double LoopStopHeight=IceLayerHeight;
  int TotalHeightSteps=floor((LoopStartHeight-LoopStopHeight)/HeightStepSize);

  int ifileentry=0;
  ////Start looping over the Tx Height and Launch angle values
  for(int ihei=0;ihei<TotalHeightSteps;ihei++){
    TxHeight=LoopStartHeight-HeightStepSize*ihei;
    for(int iang=0;iang<TotalAngleSteps;iang++){
      RayLaunchAngleInAir=LoopStartAngle+AngleStepSize*iang;
      
      /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      ////Section for propogating the ray through the atmosphere
  
      ////Find out how many atmosphere layers are above the source or Tx which we do not need
      int skiplayer=0;
      for(int ilayer=MaxLayers;ilayer>-1;ilayer--){
	//cout<<ilayer<<" "<<ATMLAY[ilayer]/100<<" "<<ATMLAY[ilayer-1]/100<<endl;
	if(TxHeight<ATMLAY[ilayer]/100 && TxHeight>ATMLAY[ilayer-1]/100){
	  //cout<<"Tx Height is in this layer with a height range of "<<MultiRayAirIceRefraction::ATMLAY[ilayer]/100<<" m to "<<MultiRayAirIceRefraction::ATMLAY[ilayer-1]/100<<" m and is at a height of "<<TxHeight<<" m"<<endl;
	  ilayer=-100;
	}
	if(ilayer>-1){
	  skiplayer++;
	}
      }
      int SkipLayersAbove=skiplayer;
      //cout<<"The tota number of layers that need to be skipped from above is "<<skiplayer<<endl;
      
      ////Find out how many atmosphere layers are below the ice height which we do not need
      skiplayer=0;
      for(int ilayer=0;ilayer<MaxLayers;ilayer++){
	//cout<<ilayer<<" "<<ATMLAY[ilayer]/100<<" "<<ATMLAY[ilayer+1]/100<<endl;
	if(IceLayerHeight>ATMLAY[ilayer]/100 && IceLayerHeight<ATMLAY[ilayer+1]/100){
	  //cout<<"Ice Layer is in the layer with a height range of "<<ATMLAY[ilayer]/100<<" m to "<<ATMLAY[ilayer+1]/100<<" m and is at a height of "<<IceLayerHeight<<" m"<<endl;
	  ilayer=100;
	}
	if(ilayer<MaxLayers){
	  skiplayer++;
	}
      }
      int SkipLayersBelow=skiplayer;
      //cout<<"The tota number of layers that need to be skipped from below is "<<skiplayer<<endl;
      
      ////Define variables for ray propogation through mutliple layers in the atmosphere
      double Start_nh=0;
      double StartHeight=0;
      double StopHeight=0;
      double StartAngle=0;
      double TotalHorizontalDistanceInAir=0;
      double TimeInAir=0;
      vector <double> layerAs,layerBs,layerCs,layerLs;////vector for storing the A,B,C and L values of each of the atmosphere layes as the ray passes through them
      
      double c0, c1;////variables to store the fit results from the GSL linear fitter for y=c0+c1*x. Here we are trying to fit the function: log(n(h)-1)=log(B)+C*h which basically comes from n(h)=A+B*exp(C*h)
      double cov00, cov01, cov11, chisq;////variables to store the covariance matrix elements outputted the GSL linear fitting function
      
      ////Start loop over the atmosphere layers and analyticaly propagate the ray through the atmosphere
      //cout<<"Fitting the atmosphere refrative index profile with multiple layers and propogate the ray"<<endl;
      for(int ilayer=MaxLayers-SkipLayersAbove-1;ilayer>SkipLayersBelow-1;ilayer--){
	
	////Run the GSL fitter to get the refractive index profile for the layer
	gsl_fit_linear (h_data[ilayer].data(), 1, lognh_data[ilayer].data() ,1, h_data[ilayer].size(),
			&c0, &c1, &cov00, &cov01, &cov11,
			&chisq);
	////Store the results from the GSL fitter
	////A, B and C parameters for the refractive index profile
	B_air=exp(c0);
	C_air=-c1;
	
	////Set the starting height of the ray for propogation for that layer
	if(ilayer==MaxLayers-SkipLayersAbove-1){
	  ////If this is the first layer then set the start height to be the height of the source
	  StartHeight=TxHeight;
	}else{
	  ////If this is any layer after the first layer then set the start height to be the starting height of the layer
	  StartHeight=ATMLAY[ilayer+1]/100;
	}
	
	////Since we have the starting height now we can find out the refactive index at that height from data using spline interpolation
	Start_nh=gsl_spline_eval(spline, StartHeight, accelerator);
	
	////Set the staopping height of the ray for propogation for that layer
	if(ilayer==(SkipLayersBelow-1)+1){
	  ////If this is the last layer then set the stopping height to be the height of the ice layer
	  StopHeight=IceLayerHeight;
	}else{
	  ////If this is NOT the last layer then set the stopping height to be the end height of the layer
	  StopHeight=ATMLAY[ilayer]/100;
	}
	
	////If this is the first layer then set the initial launch angle of the ray through the layers. I calculate the final launch angle by doing 180-RayLaunchAngleInAir since my raytracer only works with 0 to 90 deg. Setting an angle of 95 deg w.r.t to the vertical where 0 is up means that my raytraces takes in an launch angle of 85.
	if(ilayer==MaxLayers-SkipLayersAbove-1){
	  StartAngle=180-RayLaunchAngleInAir;
	}
	//cout<<ilayer<<" Starting n(h)="<<Start_nh<<" ,A="<<A<<" ,B="<<B<<" ,C="<<C<<" StartingHeight="<<StartHeight<<" ,StoppingHeight="<<StopHeight<<" ,RayLaunchAngle"<<StartAngle<<endl;
	
	////Get the hit parameters from the function. The output is:
	//// How much horizontal distance did the ray travel in the layer
	//// The angle of reciept/incidence at the end or the starting angle for propogation through the next layer
	//// The value of the L parameter for that layer
	double* GetHitPar=GetLayerHitPointPar(Start_nh, StopHeight, StartHeight, StartAngle, 1);
	TotalHorizontalDistanceInAir+=GetHitPar[0];
	StartAngle=GetHitPar[1];
	TimeInAir+=GetHitPar[3];
	
	////Store in the values of A,B,C and L for tha layer
	layerLs.push_back(GetHitPar[2]);
	layerAs.push_back(MultiRayAirIceRefraction::A_air);
	layerBs.push_back(B_air);
	layerCs.push_back(C_air);
	
	// printf ("# best fit: Y = %g + %g X\n", c0, c1);
	// // printf ("# covariance matrix:\n");
	// // printf ("# [ %g, %g\n#   %g, %g]\n",cov00, cov01, cov01, cov11);
	// printf ("# chisq = %g\n", chisq);
	
	////dont forget to delete the pointer!
	delete []GetHitPar;
      }
      
      double IncidentAngleonIce=StartAngle;
      //cout<<"Total horizontal distance travelled by the ray using Multiple Layer fitting is "<<TotalHorizontalDistance<<endl;
      
      ////The Single Layer fitting of the whole atmospheric refractive index profile has been commented out to make the loop faster
      
      // cout<<"Now treating the atmosphere refrative index profile as a single layer and fitting it and propogating the ray"<<endl;
      
      // ////Run the GSL fitter to get the refractive index profile for the whole atmosphere
      // gsl_fit_linear (flatten(h_data).data(), 1, flatten(lognh_data).data() ,1, flatten(h_data).size() ,
      // 		    &c0, &c1, &cov00, &cov01, &cov11,
      // 		    &chisq);
      // ////A, B and C parameters for the refractive index profile
      // B_air=exp(c0);
      // C_air=-c1;
      
      // ////Set the starting height of the ray for propogation to be the height of the transmitter
      // StartHeight=TxHeight;
      // ////Since we have the starting height now we can find out the refactive index at that height from data using spline interpolation
      // Start_nh=gsl_spline_eval(spline, StartHeight, accelerator);
      // ////Set the stopping height of the ray for propogation to be the height of the ice layer
      // StopHeight=IceLayerHeight;
      // ////Set the initial launch angle of the ray
      // StartAngle=180-RayLaunchAngleInAir;
      // //cout<<ilayer<<" Starting n(h)="<<Start_nh<<" ,A="<<A<<" ,B="<<B<<" ,C="<<C<<" StartingHeight="<<StartHeight<<" ,StoppingHeight="<<StopHeight<<" ,RayLaunchAngle"<<StartAngle<<endl;
      
      // ////Get the hit parameters from the function. The output is:
      // //// How much horizontal distance did the ray travel through the whole atmosphere
      // //// The angle of reciept/incidence at the end 
      // //// The value of the L parameter for whole atmosphere fit
      // double* GetHitPar=GetLayerHitPointPar(Start_nh, StopHeight, StartHeight, StartAngle, 1);
      
      // ////SLF here stands for Single Layer Fitting. These variables store the hit parameters
      // double TotalHorizontalDistanceSLF=GetHitPar[0];
      // double StartAngleSLF=GetHitPar[1];
      // double LvalueSLF=GetHitPar[2];
      
      // //printf ("# best fit: Y = %g + %g X\n", c0, c1);
      // ////printf ("# covariance matrix:\n");
      // ////printf ("# [ %g, %g\n#   %g, %g]\n",cov00, cov01, cov01, cov11);
      // //printf ("# chisq = %g\n", chisq);
      
      // cout<<"Total horizontal distance travelled by the ray using Single Layer fitting is  "<<TotalHorizontalDistanceSLF<<endl;
      
      //cout<<"Now propagating the ray through the ice towards the antenna"<<endl;
      
      /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      ////Section for propogating the ray through the ice
            
      ////Set the starting depth of the ray for propogation to at the ice surface
      double StartDepth=0.0;
      ////Since we have the starting height of the ice layer we can find out the refactive index of air at that height from data using spline interpolation
      //double Start_nh=gsl_spline_eval(spline, IceLayerHeight, accelerator);
      Start_nh=gsl_spline_eval(spline, IceLayerHeight, accelerator);
      ////Set the stopping depth of the ray for propogation to be the depth of the antenna
      StopHeight=AntennaDepth;
      ////Set the initial launch angle or the angle of incidence of the ray
      StartAngle=IncidentAngleonIce;
      //cout<<"Starting n(h)="<<Start_nh<<" ,A="<<A<<" ,B="<<B<<" ,C="<<C<<" StartingDepth="<<StartDepth<<" ,StoppingDepth="<<AntennaDepth<<" ,RayLaunchAngle="<<StartAngle<<endl;
    
      ////Get the hit parameters from the function. The output is:
      //// How much horizontal distance did the ray travel through ice to hit the antenna
      //// The angle of reciept/incidence at the end at the antenna
      //// The value of the L parameter for whole atmosphere fit
      double *GetHitPar=GetLayerHitPointPar(Start_nh, AntennaDepth,StartDepth, StartAngle, 0);
      
      ////SLF here stands for Single Layer Fitting. These variables store the hit parameters
      double TotalHorizontalDistanceInIce=GetHitPar[0];
      double RecievdAngleInIce=GetHitPar[1];
      double LvalueIce=GetHitPar[2];
      double TimeInIce=GetHitPar[3];
      
      //cout<<"Total horizontal distance travelled by the ray in ice is  "<<TotalHorizontalDistanceInIce<<endl;
      
      //cout<<ifileentry<<" "<<TxHeight<<" "<<IceLayerHeight<<" "<<TxHeight-IceLayerHeight<<" "<<TotalHorizontalDistance<<" "<<TotalHorizontalDistanceInIce<<" "<<RayLaunchAngleInAir<<" "<<IncidentAngleonIce<<" "<<RecievdAngleInIce<<endl;
      //aout<<ifileentry<<" "<<TxHeight<<" "<<IceLayerHeight<<" "<<TxHeight-IceLayerHeight<<" "<<TotalHorizontalDistance<<" "<<TotalHorizontalDistanceInIce<<" "<<RayLaunchAngleInAir<<" "<<IncidentAngleonIce<<" "<<RecievdAngleInIce<<endl;
      aout<<ifileentry<<" "<<TxHeight<<" "<<TotalHorizontalDistanceInAir + TotalHorizontalDistanceInIce<<" "<<TotalHorizontalDistanceInAir<<" "<<TotalHorizontalDistanceInIce<<" "<<(TimeInIce+TimeInAir)*MultiRayAirIceRefraction::spedc<<" "<<TimeInAir*MultiRayAirIceRefraction::spedc<<" "<<TimeInIce*MultiRayAirIceRefraction::spedc<<" "<<(TimeInIce+TimeInAir)*pow(10,9)<<" "<<TimeInAir*pow(10,9)<<" "<<TimeInIce*pow(10,9)<<" "<<RayLaunchAngleInAir<<" "<<IncidentAngleonIce<<" "<<RecievdAngleInIce<<endl;

      delete[] GetHitPar;
      layerAs.clear();
      layerBs.clear();
      layerCs.clear();
      layerLs.clear();
     
      ifileentry++;
    }//// end of iang loop
  }//// end of ihei loop

  delete accelerator;
  delete spline;
  flattened_h_data.clear();
  flattened_nh_data.clear();

  timestamp_t t1 = get_timestamp();
  
  double secs = (t1 - t0) / 1000000.0L;
  cout<<"total time taken by the script: "<<secs<<" s"<<endl;
  return 0;
  
}
