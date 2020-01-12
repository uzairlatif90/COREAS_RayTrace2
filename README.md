# AnalyticRayTracing from Air to Ice
C++ code which uses analytic ray tracing for tracing rays from any point in the air to any point in the ice

- MultiRayAirIceRefraction.cc and .h  : These two files contain the namespace MultiRayAirIceRefraction that contains all the raytracing functions required by CoREAS to make the table for interpolation.

- CoREASRayTrace.cc: Ths script takes in as arguments the antenna depth and the height of the ice layer in m (a.s.l). Then it calls the function 'MakeRayTracingTable' from the namespace and loops over values of Tx Height and the ray launch angle and prints a file called "TableValues.txt" which contains the following columns;

  - NOTE: The distances are printed in m, Times are printed in ns, Angles are printed in deg and w.r.t to the vertical where 0 deg is straight up.

  - the entry number, the Tx height, Total Horizontal Distance (T.H.D), T.H.D in air, T.H.D in ice, Total Ray Path Distance (T.R.P.D), T.R.P.D in air, T.R.P.D in ice, Total Ray Time (T.R.T), T.R.T in air, T.R.T in ice, Launch angle in air, Incident angle on ice and recieved angle in ice.

  - The step size for Tx height loop is set at 20 m and it starts at the maximum available height from the refractive index data to a step above the ice surface.

  - The step size for the ray launch angle loop goes from 91 deg to 179 deg in steps of 1 deg. Here 0 deg is vertically upwards

- Atmosphere.dat: This file has been generated with GDAS tool that comes with CORSIKA. The command that was used to generate this file was: ./gdastool -t 1533600000 -o Atmosphere.dat -c -89.9588 -109.794 -m -5 -v -g

  - These are the coordinates for the ARA2 station at the South Pole.

## Prerequisites
You will need to have a functioning installation of [GSL](https://www.gnu.org/software/gsl/) ([2.4](https://ftp.gnu.org/gnu/gsl/gsl-2.4.tar.gz) is verified to work).
- You will need to set the enviromnent variable `GSLDIR` to your local installation of GSL.
- You will also need to have `GSLDIR` in your `LD_LIBRARY_PATH`.
- For Mac users: you can locate your GSL installation via `gsl-config --prefix`, i.e. `export GSLDIR=$(gsl-config --prefix)`
- If you have Ubuntu or Linux you can skip all of the above and just get it from the repository by doing: "sudo apt install libgsl-dev"

## Install instructions

### CoREASRayTrace.cc as standalone package
To run you just have to do:
- Make it: `make CoREASRayTrace`
- Run it: `./CoREASRayTrace 200 3000`
- In this case the example arguments are: Antenna Depth is set at 200 m, Ice Layer Height is set as 3000 m
- The main is at the bottom of the code, which you can modify to your liking.