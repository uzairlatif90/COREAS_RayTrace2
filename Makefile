CC=g++
INC_DIR= /home/uzairlatif/Documents/COREAS_RayTrace2
CFLAGS=-O -g -Wall -I$(INC_DIR)
DEPS= MultiRayAirIceRefraction.h
SOURCES= MultiRayAirIceRefraction.cc CoREASRayTrace.cc
OBJ= $(SOURCES:.cc = .o)
LDFLAGS=-L${GSLDIR}/lib
CXXFLAGS=-I${GSLDIR}/include
LDLIBS=-lgsl -lgslcblas

CoREASRayTrace: $(OBJ)
	$(CC) $(CFLAGS) -o CoREASRayTrace $(OBJ) $(LDLIBS)
