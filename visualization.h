#ifndef VISUALIZATION_H
#define VISUALIZATION_H

#include <QObject>
#include "simulation.h"
#include "simulationdata.h"
#include <QtDataVisualization>
#include "surfacegraph.h"
#include "scattergraph.h"

using namespace QtDataVisualization;

class Visualization : public QObject
{
    Q_OBJECT
public:
    explicit Visualization(SurfaceGraph *surfaceGraph= nullptr, Simulation *simulation = nullptr);
    void Start();

private:
    Simulation *simulation;
    SurfaceGraph *surfaceGraph;
    QTimer *timer;
    QScatter3DSeries * arrows;
    QScatterDataArray * arrowsArray;

    void Visualize(SimulationData data);
    QSurfaceDataArray * ConvertSimulationDataToQSurfaceDataArray(SimulationData data);

signals:

public slots:
    void Update();
};

#endif // VISUALIZATION_H
