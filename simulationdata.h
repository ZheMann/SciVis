#ifndef SIMULATIONDATA_H
#define SIMULATIONDATA_H

#include "rfftw.h"

class SimulationData
{
public:
    SimulationData();

    fftw_real *density;
    fftw_real *velocity_x;
    fftw_real *velocity_y;
    int grid_size;
};

#endif // SIMULATIONDATA_H
