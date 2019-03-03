#ifndef SURFACEGRAPH_H
#define SURFACEGRAPH_H

#include <QtDataVisualization/Q3DSurface>
#include <QtDataVisualization/QSurfaceDataProxy>
#include <QtDataVisualization/QHeightMapSurfaceDataProxy>
#include <QtDataVisualization/QSurface3DSeries>
#include <QtDataVisualization/Q3DInputHandler>
#include <QtDataVisualization>
#include <QtWidgets/QSlider>

using namespace QtDataVisualization;

class SurfaceGraph : public QObject
{
    Q_OBJECT
public:
    explicit SurfaceGraph(Q3DSurface *surface);
    ~SurfaceGraph();

    void SetArrayData(QSurfaceDataArray *data);

    void enableSqrtSinModel(bool enable);

    void toggleModeItem() { m_graph->setSelectionMode(QAbstract3DGraph::SelectionItem); }

    void setClamp(float min, float max);

    void setBlackToYellowGradient();
    void setGreenToRedGradient();
    void setRainbowGradient();
    void setHeatmapGradient();
    void setHeatmapRedGradient();

public Q_SLOTS:
    void changeTheme(int theme);

private:
    Q3DSurface *m_graph;

    QSurfaceDataProxy *m_graph_data;
    QSurface3DSeries *m_sqrtSinSeries;
    Q3DInputHandler *input_handler;


    float m_rangeMinX;
    float m_rangeMinZ;
    float m_stepX;
    float m_stepZ;

    void fillSqrtSinProxy();
};

#endif // SURFACEGRAPH_H
