#include "simulation.h"
#include <stdio.h>
#include <math.h>
#include <QtDebug>

#include <QtCore/qmath.h>

const int grid_size = 50;			//size of simulation grid

Simulation::Simulation()
{
        init(grid_size);
}

void Simulation::init(int n)
{
    int i;
    size_t grid_size;

    //Allocate data structures
    grid_size     = n * 2*(n/2+1)*sizeof(fftw_real);
    vx       = (fftw_real*) malloc(grid_size);
    vy       = (fftw_real*) malloc(grid_size);
    vx0      = (fftw_real*) malloc(grid_size);
    vy0      = (fftw_real*) malloc(grid_size);
    grid_size     = n * n * sizeof(fftw_real);
    fx      = (fftw_real*) malloc(grid_size);
    fy      = (fftw_real*) malloc(grid_size);
    rho     = (fftw_real*) malloc(grid_size);
    rho0    = (fftw_real*) malloc(grid_size);
    plan_rc = rfftw2d_create_plan(n, n, FFTW_REAL_TO_COMPLEX, FFTW_IN_PLACE);
    plan_cr = rfftw2d_create_plan(n, n, FFTW_COMPLEX_TO_REAL, FFTW_IN_PLACE);

    //Initialize data structures to 0
    for (i = 0; i < n * n; i++)
    { vx[i] = vy[i] = vx0[i] = vy0[i] = fx[i] = fy[i] = rho[i] = rho0[i] = 0.0f; }
}

void Simulation::FFT(int direction,void* vx)
{
    if(direction==1)
    {
        rfftwnd_one_real_to_complex(plan_rc,(fftw_real*)vx,(fftw_complex*)vx);
    }
    else
    {
        rfftwnd_one_complex_to_real(plan_cr,(fftw_complex*)vx,(fftw_real*)vx);
    }
}

int Simulation::clamp(float x)
{
    return ((x)>=0.0?((int)(x)):(-((int)(1-(x)))));
}

//solve: Solve (compute) one step of the fluid flow simulation
void Simulation::solve(int n, fftw_real* vx, fftw_real* vy, fftw_real* vx0, fftw_real* vy0, fftw_real visc, fftw_real dt)
{
    fftw_real x, y, x0, y0, f, r, U[2], V[2], s, t;
    int i, j, i0, j0, i1, j1;

    for (i=0;i<n*n;i++)
    {
        vx[i] += dt*vx0[i]; vx0[i] = vx[i]; vy[i] += dt*vy0[i]; vy0[i] = vy[i];
    }

    for ( x=0.5f/n,i=0 ; i<n ; i++,x+=1.0f/n )
       for ( y=0.5f/n,j=0 ; j<n ; j++,y+=1.0f/n )
       {
          x0 = n*(x-dt*vx0[i+n*j])-0.5f;
          y0 = n*(y-dt*vy0[i+n*j])-0.5f;
          i0 = clamp(x0); s = x0-i0;
          i0 = (n+(i0%n))%n;
          i1 = (i0+1)%n;
          j0 = clamp(y0); t = y0-j0;
          j0 = (n+(j0%n))%n;
          j1 = (j0+1)%n;
          vx[i+n*j] = (1-s)*((1-t)*vx0[i0+n*j0]+t*vx0[i0+n*j1])+s*((1-t)*vx0[i1+n*j0]+t*vx0[i1+n*j1]);
          vy[i+n*j] = (1-s)*((1-t)*vy0[i0+n*j0]+t*vy0[i0+n*j1])+s*((1-t)*vy0[i1+n*j0]+t*vy0[i1+n*j1]);
       }

    for(i=0; i<n; i++)
      for(j=0; j<n; j++)
      {  vx0[i+(n+2)*j] = vx[i+n*j]; vy0[i+(n+2)*j] = vy[i+n*j]; }

    FFT(1,vx0);
    FFT(1,vy0);

    for (i=0;i<=n;i+=2)
    {
       x = 0.5f*i;
       for (j=0;j<n;j++)
       {
          y = j<=n/2 ? (fftw_real)j : (fftw_real)j-n;
          r = x*x+y*y;
          if ( r==0.0f ) continue;
          f = (fftw_real)exp(-r*dt*visc);
          U[0] = vx0[i  +(n+2)*j]; V[0] = vy0[i  +(n+2)*j];
          U[1] = vx0[i+1+(n+2)*j]; V[1] = vy0[i+1+(n+2)*j];

          vx0[i  +(n+2)*j] = f*((1-x*x/r)*U[0]     -x*y/r *V[0]);
          vx0[i+1+(n+2)*j] = f*((1-x*x/r)*U[1]     -x*y/r *V[1]);
          vy0[i+  (n+2)*j] = f*(  -y*x/r *U[0] + (1-y*y/r)*V[0]);
          vy0[i+1+(n+2)*j] = f*(  -y*x/r *U[1] + (1-y*y/r)*V[1]);
       }
    }

    FFT(-1,vx0);
    FFT(-1,vy0);

    f = 1.0/(n*n);
    for (i=0;i<n;i++)
       for (j=0;j<n;j++)
       { vx[i+n*j] = f*vx0[i+(n+2)*j]; vy[i+n*j] = f*vy0[i+(n+2)*j]; }
}


// diffuse_matter: This function diffuses matter that has been placed in the velocity field. It's almost identical to the
// velocity diffusion step in the function above. The input matter densities are in rho0 and the result is written into rho.
void Simulation::diffuse_matter(int n, fftw_real *vx, fftw_real *vy, fftw_real *rho, fftw_real *rho0, fftw_real dt)
{
    fftw_real x, y, x0, y0, s, t;
    int i, j, i0, j0, i1, j1;

    for ( x=0.5f/n,i=0 ; i<n ; i++,x+=1.0f/n )
        for ( y=0.5f/n,j=0 ; j<n ; j++,y+=1.0f/n )
        {
            x0 = n*(x-dt*vx[i+n*j])-0.5f;
            y0 = n*(y-dt*vy[i+n*j])-0.5f;
            i0 = clamp(x0);
            s = x0-i0;
            i0 = (n+(i0%n))%n;
            i1 = (i0+1)%n;
            j0 = clamp(y0);
            t = y0-j0;
            j0 = (n+(j0%n))%n;
            j1 = (j0+1)%n;
            rho[i+n*j] = (1-s)*((1-t)*rho0[i0+n*j0]+t*rho0[i0+n*j1])+s*((1-t)*rho0[i1+n*j0]+t*rho0[i1+n*j1]);
        }
}

//set_forces: copy user-controlled forces to the force vectors that are sent to the solver.
//            Also dampen forces and matter density to get a stable simulation.
void Simulation::set_forces()
{
    int i;
    for (i = 0; i < grid_size * grid_size; i++)
    {
        rho0[i]  = 0.995 * rho[i];
        fx[i] *= 0.85;
        fy[i] *= 0.85;
        vx0[i]    = fx[i];
        vy0[i]    = fy[i];
    }
}


//do_one_simulation_step: Do one complete cycle of the simulation:
//      - set_forces:
//      - solve:            read forces from the user
//      - diffuse_matter:   compute a new set of velocities
//      - gluPostRedisplay: draw a new visualization frame
void Simulation::do_one_simulation_step(void)
{
    set_forces();
    solve(grid_size, vx, vy, vx0, vy0, visc, dt);
    diffuse_matter(grid_size, vx, vy, rho, rho0, dt);
}

void Simulation::set_input_force_x(int x,int y)
{
    int index = (x) + (y * grid_size);
    fx[index] = 10;
}

void Simulation::set_input_force_y(int x,int y)
{
    int index = (x) + (y * grid_size);
    fy[index] = 10;
}


SimulationData Simulation::get_data()
{
    SimulationData *data = new SimulationData();
    data->density = rho;
    data->velocity_x = vx;
    data->velocity_y = vy;
    data->grid_size = grid_size;
    return *data;
}