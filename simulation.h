#ifndef SIMULATION_H
#define SIMULATION_H

#include <QObject>
#include "rfftw.h"
#include "simulationdata.h"
#include <QTimer>

class Simulation : public QObject
{
    Q_OBJECT

public:
    explicit Simulation();
    void do_one_simulation_step(void);
    SimulationData get_data();

    void set_input_force_x(int x,int y);
    void set_input_force_y(int x,int y);
private:
    float dt = 0.4f;                                 //simulation time step
    float visc = 0.001f;                             //fluid viscosity
    fftw_real *vx, *vy;                             //(vx,vy)   = velocity field at the current moment
    fftw_real *vx0, *vy0;                           //(vx0,vy0) = velocity field at the previous moment
    fftw_real *fx, *fy;                             //(fx,fy)   = user-controlled simulation forces, steered with the mouse
    fftw_real *rho, *rho0;                          //smoke density at the current (rho) and previous (rho0) moment
    rfftwnd_plan plan_rc, plan_cr;                  //simulation domain discretization



    void init(int n);
    void FFT(int direction,void* vx);
    int clamp(float x);
    void solve(int n, fftw_real* vx, fftw_real* vy, fftw_real* vx0, fftw_real* vy0, fftw_real visc, fftw_real dt);
    void diffuse_matter(int n, fftw_real *vx, fftw_real *vy, fftw_real *rho, fftw_real *rho0, fftw_real dt);
    void set_forces(void);

signals:

public slots:
};

#endif // SIMULATION_H
