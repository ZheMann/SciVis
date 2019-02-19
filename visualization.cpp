#include "visualization.h"
#include "simulation.h"
#include "simulationdata.h"
#include <QtDataVisualization>


using namespace QtDataVisualization;



Visualization::Visualization(SurfaceGraph *surfaceGraph, Simulation *simulation)
{
    this->simulation = simulation;
    this->surfaceGraph = surfaceGraph;
}

void Visualization::Start()
{
    timer = new QTimer(this);

    // setup signal and slot
    connect(timer, SIGNAL(timeout()),
            this, SLOT(Update()));

    timer->start(1000/30);


}

void Visualization::Update()
{
    //Apply demo forces
    int i,j;
    for(int i =0; i < 25; i++){
        simulation->set_input_force_x(i*2,0);
        simulation->set_input_force_y(0,i * 4 -1);
    }

    //Simulation and visualization
    simulation->do_one_simulation_step();
    SimulationData data = simulation->get_data();

    this->Visualize(data);
}

void Visualization::Visualize(SimulationData data)
{
    QSurfaceDataArray *surfaceData = ConvertSimulationDataToQSurfaceDataArray(data);

    surfaceGraph->SetArrayData(surfaceData);
}

QSurfaceDataArray * Visualization::ConvertSimulationDataToQSurfaceDataArray(SimulationData data)
{
    QSurfaceDataArray *dataArray = new QSurfaceDataArray();
    int size = data.grid_size;
    dataArray->reserve(size);

    int i, j, idx;
    for (int i = 0 ; i < size ; i++) {
        QSurfaceDataRow *newRow = new QSurfaceDataRow(size);

        for (j = 0; j < size; j++) {
               QSurfaceDataRow *newRow = new QSurfaceDataRow(size);
               for (i = 0; i < size; i++) {
                   idx = (j * size) + i;
                   (*newRow)[i].setPosition(QVector3D(j, data.density[idx], i));
               }
               *dataArray << newRow;
        }
        *dataArray << newRow;
    }

    return dataArray;
}


