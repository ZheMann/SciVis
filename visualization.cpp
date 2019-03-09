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


    if (hist_counter > 10 && (avg_min > 0 && avg_max > 0 && avg_max <=1))
    {
        double max_clamp = avg_max * (1 - this->clamp_threshold);
        double min_clamp = avg_min * (1 + this->clamp_threshold);
        if(min_clamp + 0.001 < max_clamp )
        {
            qDebug("%f", min_clamp);
            surfaceGraph->setClamp(min_clamp, max_clamp );
        }
    }
    else if ((min > 0 && max > 0 ))
    {
        surfaceGraph->setClamp(min * (1 + this->clamp_threshold), max * (1 - this->clamp_threshold));
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

void Visualization::setClampingThreshold(int threshold)
{
    qDebug("threshold: %i", threshold);

    this->clamp_threshold = threshold / 100.0;

}

