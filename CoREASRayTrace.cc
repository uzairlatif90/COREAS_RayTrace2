#include </home/uzairlatif/Documents/COREAS_RayTrace2/MultiRayAirIceRefraction.cc>

int main(int argc, char **argv){
  
  if(argc==1){
    cout<<"No Extra Command Line Argument Passed Other Than Program Name"<<endl;
    cout<<"Example run command: ./MultiRayAirIceRefraction 200 3000"<<endl;
    cout<<"Here 200 is Antenna Depth in m and 3000 is Ice Layer Height in m"<<endl;
    return 0;
  }
  if(argc<3){
    cout<<"More Arguments needed!"<<endl;
    cout<<"Example run command: ./MultiRayAirIceRefraction 200 3000"<<endl;
    cout<<"Here 200 is Antenna Depth in m and 3000 is Ice Layer Height in m"<<endl;
    return 0;
  }
  if(argc==3){
    cout<<"Antenna Depth is set at "<<atof(argv[1])<<" m, Ice Layer Height is set as "<<atof(argv[2])<<" m"<<endl;
  } 
  if(argc>3){
    cout<<"More Arguments than needed!"<<endl;
    cout<<"Example run command: ./MultiRayAirIceRefraction 200 3000"<<endl;
    cout<<"Here 200 is Antenna Depth in m and 3000 is Ice Layer Height in m"<<endl;
    return 0;
  }
  
  double AntennaDepth=atof(argv[1]);//200;////Depth of antenna in the ice
  double IceLayerHeight=atof(argv[2]);//3000;////Height where the ice layer starts off

  int output=MultiRayAirIceRefraction::MakeRayTracingTable(AntennaDepth,IceLayerHeight);
  //MakeRayTracingTable(AntennaDepth,IceLayerHeight);
  
}