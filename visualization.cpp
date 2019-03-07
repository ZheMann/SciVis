#include <iostream>     // std::cout
#include <algorithm>    // std::minmax_element
#include <array>        // std::array

#include "visualization.h"
#include "simulation.h"
#include "simulationdata.h"
#include <QtDataVisualization>
#include "scattergraph.h"



using namespace QtDataVisualization;



Visualization::Visualization(SurfaceGraph *surfacegraph, Simulation *simulation):
    hist_counter(0),
    clamp_threshold(0)
{
    this->simulation = simulation;
    this->surfaceGraph = surfacegraph;


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
    simulation->do_one_simulation_step();
    SimulationData data = simulation->get_data();

    this->Visualize(data);
}

void Visualization::Visualize(SimulationData data)
{
    QSurfaceDataArray * surfaceData = ConvertSimulationDataToQSurfaceDataArray(data);

    float min = 1;
    float max = 0;
    for (int i = 0; i < data.grid_size * data.grid_size; ++i) {
        float value = data.density[i];
        if( value > max)
        {
            max = value;
        }
        if( value < min)
        {
            min = value;
        }
    }
    int index = hist_counter % 10;
    this->min_history[index] = min;
    this->max_history[index] = max;
    this->hist_counter++;

    float avg_max = std::accumulate(this->max_history.begin(), max_history.end(), 0.0f) / max_history.size();
    float avg_min = std::accumulate(this->min_history.begin(), min_history.end(), 0.0f) / min_history.size();

    if (hist_counter > 10)
    {
        surfaceGraph->setClamp(avg_min + this->clamp_threshold, avg_max - this->clamp_threshold);
    }
    else
    {
        surfaceGraph->setClamp(min + this->clamp_threshold, max - this->clamp_threshold);
    }

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


